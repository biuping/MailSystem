#include "TCPClientSocket.h"
#include <iostream>
#include <WS2tcpip.h>


TCPClientSocket::TCPClientSocket()
{
	ZeroMemory(&mServerAddr, sizeof(mServerAddr));
	mSocket = INVALID_SOCKET;
}

TCPClientSocket::TCPClientSocket(const char* addr, int port)
{
	ZeroMemory(&mServerAddr, sizeof(mServerAddr));
	connect2ServerAddr(addr, port);
}

TCPClientSocket::~TCPClientSocket() {
	this->closeSocket();
}

sockaddr TCPClientSocket::getServerAddr()
{
	return mServerAddr;
}

void TCPClientSocket::closeSocket()
{
	if (mSocket != INVALID_SOCKET) {
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
}

// 给定服务器地址（ip或域名）和端口，创建套接字并连接到服务器地址
// TODO: 考虑是否需要使用非阻塞模式
bool TCPClientSocket::connect2ServerAddr(const char* addr, int port)
{
	addrinfo* res = NULL,
		* ptr = NULL,
		hints;
	int iRet;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // 未指明返回类型
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_protocol = IPPROTO_TCP;
	const char* portName = std::to_string(port).c_str();

	// 获取服务器地址信息
	iRet = getaddrinfo(addr, portName, &hints, &res);
	if (iRet != 0) {
		// 获取服务器地址信息失败
		rstring errstr = "connect to server failed: getaddrinfo returned ";
		errstr.append(std::to_string(iRet));
		report(errstr);
		return false;
	}
	// 获取成功
	ptr = res;
	while (ptr) {
		// 访问地址信息链表，逐个创建套接字及连接直到全部成功
		mSocket = INVALID_SOCKET;
		mSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		
		if (mSocket == INVALID_SOCKET) {
			rstring errstr = "connect to server failed: socket returned ";
			errstr.append(std::to_string(WSAGetLastError()));
			report(errstr);
		}
		else {
			// 套接字创建成功
			report("create socket success");

			// 连接到服务器地址
			iRet = connect(mSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iRet == SOCKET_ERROR) {
				// 连接失败
				rstring errstr = "connect to server failed: connect returned ";
				errstr.append(std::to_string(WSAGetLastError()));
				report(errstr);
				closesocket(mSocket); // 关闭套接字
				mSocket = INVALID_SOCKET;
			}
			else {
				// 连接成功
				report("connect success");

				mServerAddr = *ptr->ai_addr; // 保存服务器地址
				break;
			}
		}
		// 取下一个地址进行尝试
		ptr = ptr->ai_next;
	}

	freeaddrinfo(res);
	// ptr不为空说明连接成功
	if (ptr)
		return true;

	return false;
}

// 从套接字中读取一行的数据
// tmo 为超时时间（单位秒），为0时无限等待
int TCPClientSocket::readLine(char* buf, int len, int tmo)
{
	if (mSocket == INVALID_SOCKET) {
		return -1;
	}

	int sz = 0;
	fd_set rdset;  // 检查套接字是否可读的集合

	// 读到换行前不断取数据
	while (sz < len) {
		FD_ZERO(&rdset);  // 清空
		FD_SET(mSocket, &rdset);  // 将套接字添加到集合中进行读测试

		// 设置超时时间
		struct timeval timeout;
		timeout.tv_sec = tmo;
		timeout.tv_usec = 0;

		// 测试是否可读
		int status = select(mSocket+1, &rdset, NULL, NULL, tmo != 0 ? &timeout : NULL);
		if (status <= 0) {
			// 调用失败
			if (status == 0)
				report("readline failed: select timeout");
			else {
				rstring errstr = "readline failed: select returned ";
				errstr.append(std::to_string(WSAGetLastError()));
				report(errstr);
			}

			return -1;
		}
		// 取新数据
		int r = recv(mSocket, buf + sz, len - 1 - sz, 0);
		if (r <= 0) {
			// recv失败
			if (r == 0) {
				report("readline failed: recv returned 0 (connection terminated)");
			}
			else {
				rstring errstr = "readline failed: recv returned ";
				errstr.append(std::to_string(WSAGetLastError()));
				report(errstr);
			}
			return -1;
		}

		sz += r;
		buf[sz] = '\0';  // 暂时的结束标记

		char* str = strstr(buf, "\r\n");  // 找行结束标记
		if (str) {
			*str = '\0';  // 掐掉换行
			return sz;
		}
	}

	return sz;
}

// 
// tmo 为超时时间（单位秒），为0时无限等待
int TCPClientSocket::readBlock(char* buf, int len, int tmo)
{
	if (mSocket == INVALID_SOCKET) {
		return -1;
	}

	int cur_len = 0;

	fd_set rdset;  // 检查套接字是否可读的集合
	FD_ZERO(&rdset);
	FD_SET(mSocket, &rdset);

	// 设置超时时间
	struct timeval timeout;
	timeout.tv_sec = tmo;
	timeout.tv_usec = 0;

	int status = select(mSocket + 1, &rdset, NULL, NULL, tmo != 0 ? &timeout : NULL);  // 检查是否可读

	if (status <= 0) {
		// 调用失败
		if (status == 0)
			report("readblock failed: select timeout");
		else {
			rstring errstr = "readblock failed: select returned ";
			errstr.append(std::to_string(WSAGetLastError()));
			report(errstr);
		}

		return -1;
	}

	cur_len = recv(mSocket, buf, len, 0);
	if (cur_len <= 0) {
		// recv失败
		if (cur_len == 0) {
			report("readblock failed: recv returned 0 (connection terminated)");
		}
		else {
			rstring errstr = "readblock failed: recv returned ";
			errstr.append(std::to_string(WSAGetLastError()));
			report(errstr);
		}
		return -1;
	}

	return cur_len;
}

// 往套接字中写数据
// tmo 为超时时间（单位秒），为0时无限等待
int TCPClientSocket::write(char* buf, int len, int tmo) 
{
	if (mSocket == INVALID_SOCKET) {
		return -1;
	}

	int sz = 0;  // 已经写了的字节数
	fd_set wrset;  // 检查套接字是否可写的集合

	// 不断写直到写完
	while (sz < len) {
		FD_ZERO(&wrset);
		FD_SET(mSocket, &wrset);  // 将套接字添加到测试集合中

		// 设置超时时间
		struct timeval timeout;
		timeout.tv_sec = tmo;
		timeout.tv_usec = 0;

		// 检查是否可写
		int status = select(mSocket + 1, NULL, &wrset, NULL, tmo != 0 ? &timeout : NULL);
		if (status <= 0) {
			// 调用失败
			if (status == 0)
				report("wriet failed: select timeout");
			else {
				rstring errstr = "write failed: select returned ";
				errstr.append(std::to_string(WSAGetLastError()));
				report(errstr);
			}

			return -1;
		}

		int r = send(mSocket, buf + sz, len - sz, 0);
		if (r <= 0) {
			// send失败
			rstring errstr = "write failed: send returned ";
			errstr.append(std::to_string(WSAGetLastError()));
			report(errstr);
		}

		sz += r;
	}

	return sz;
}

// 报告信息
void TCPClientSocket::report(rstring info)
{
	std::cout << "TCPClientSocket " << info << std::endl;
}