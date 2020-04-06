#include "TCPClientSocket.h"
#include <iostream>


TCPClientSocket::TCPClientSocket()
{
	ZeroMemory(&mServerAddr, sizeof(mServerAddr));
	mSocket = INVALID_SOCKET;
}

TCPClientSocket::TCPClientSocket(const TCPClientSocket& tcpcliSocket)
{
	mServerAddr = tcpcliSocket.mServerAddr;
	mSocket = tcpcliSocket.mSocket;
}

// 构造 TCPClientSocket 对象，并连接到 (addr,port,TCP) 指定的网络进程套接字
// addr 可为域名或ip地址
// tmo 为超时时间（单位秒），为0时无限等待
TCPClientSocket::TCPClientSocket(const char* addr, USHORT port, int tmo) : TCPClientSocket()
{
	connect2Server(addr, port, tmo);
}

TCPClientSocket::~TCPClientSocket() {
	this->closeSocket();
}

sockaddr TCPClientSocket::getServerAddr()
{
	return mServerAddr;
}

// 关闭套接字
void TCPClientSocket::closeSocket()
{
	if (mSocket != INVALID_SOCKET) {
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
}

// 返回套接字是否已连接（套接字创建成功）
bool TCPClientSocket::connected()
{
	return mSocket == INVALID_SOCKET;
}

// 创建非阻塞模式的套接字并连接到 info 中指定地址的套接字
// 创建成功返回 true，mSocket设为对应套接字
// 创建失败返回 false，mSocket设为 INVALID_SOCKET
// tmo 为超时时间（单位秒），为0时无限等待
bool TCPClientSocket::createNBSocketAndConnect(addrinfo* info, int tmo) 
{
	mSocket = INVALID_SOCKET;
	mSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

	if (mSocket == INVALID_SOCKET) {
		// 创建套接字失败
		rstring errstr = "connect to server failed: socket returned ";
		errstr.append(std::to_string(WSAGetLastError()));
		report(errstr);
		return false;
	}
	else {
		// 套接字创建成功
		report("create socket success");

		DWORD arg = 1;
		ioctlsocket(mSocket, FIONBIO, &arg);  // 设置非阻塞模式

		// 连接到服务器地址
		int iRet = connect(mSocket, info->ai_addr, (int)info->ai_addrlen);
		if (iRet == SOCKET_ERROR) {
			// 尝试连接失败
			int error = WSAGetLastError();
			
			if (error != WSAEWOULDBLOCK) {
				// 非 WSAEWOULDBLOCK 错误，无法连接
				rstring errstr = "connect to server failed: connect returned ";
				errstr.append(std::to_string(error));
				report(errstr);
				this->closeSocket();

				return false;
			}
			else {
				// WSAEWOULDBLOCK 错误，连接无法立即建立，在之后 select 调用对套接字写时会进行连接
				fd_set wrset, exset;   // 套接字读测试、错误检查测试集合
				// 非阻塞模式下，connect 试图连接的失败会表现在 exceptfds 中
				FD_ZERO(&wrset); FD_ZERO(&exset);
				FD_SET(mSocket, &wrset); FD_SET(mSocket, &exset);

				// 设置 select 超时时间
				struct timeval timeout;
				timeout.tv_sec = tmo;
				timeout.tv_usec = 0;

				int status = select(mSocket + 1, nullptr, &wrset, &exset, tmo != 0 ? &timeout : nullptr);
				if (status <= 0) {
					if (status == 0) {
						report("connect to server failed: select timeout");
					} else {
						rstring errstr = "connect to server failed: select returned ";
						errstr.append(std::to_string(error));
						report(errstr);
					}

					this->closeSocket();
					return false;
				}

				if (FD_ISSET(mSocket, &exset)) {
					// 检查到连接失败的错误
					report("connect to server failed: select found error in exceptfds");

					this->closeSocket();
					return false;
				}
			}
		}
		// 连接成功
		report("connect to server success");

		mServerAddr = *info->ai_addr; // 保存服务器地址

		return true;
	}
}

// 给定服务器地址（ip或域名）和端口，创建套接字并连接到服务器地址
// addr 可以为域名或ip地址
// 返回是否连接成功
bool TCPClientSocket::connect2Server(const char* addr, USHORT port, int tmo)
{
	closeSocket();  // 如果之前存在已建立的套接字则关闭

	addrinfo* res = nullptr,
		* ptr = nullptr,
		hints;
	int iRet;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // 未指明返回类型
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_protocol = IPPROTO_TCP;

	// 获取服务器地址信息
	iRet = getaddrinfo(addr, std::to_string(port).c_str(), &hints, &res);
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
		// 访问地址信息链表，逐个创建套接字及连接直到成功连接
		if (createNBSocketAndConnect(ptr)) {
			// 创建套接字并连接成功
			break;
		}

		// 连接失败，取下一个地址进行尝试
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
// 返回读取的字节数，失败返回-1
int TCPClientSocket::readline(char* buf, int len, int tmo)
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
		int status = select(mSocket+1, &rdset, nullptr, nullptr, tmo != 0 ? &timeout : nullptr);
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

// 读取一块数据
// tmo 为超时时间（单位秒），为0时无限等待
// 返回读取的字节数，失败返回-1
int TCPClientSocket::readblock(char* buf, int len, int tmo)
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

	int status = select(mSocket + 1, &rdset, nullptr, nullptr, tmo != 0 ? &timeout : nullptr);  // 检查是否可读

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
// 返回写入的字节数，失败返回-1
int TCPClientSocket::write(const char* buf, int len, int tmo) 
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
		int status = select(mSocket + 1, nullptr, &wrset, nullptr, tmo != 0 ? &timeout : nullptr);
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