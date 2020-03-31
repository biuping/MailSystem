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

// ������������ַ��ip���������Ͷ˿ڣ������׽��ֲ����ӵ���������ַ
// TODO: �����Ƿ���Ҫʹ�÷�����ģʽ
bool TCPClientSocket::connect2ServerAddr(const char* addr, int port)
{
	addrinfo* res = NULL,
		* ptr = NULL,
		hints;
	int iRet;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // δָ����������
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_protocol = IPPROTO_TCP;
	const char* portName = std::to_string(port).c_str();

	// ��ȡ��������ַ��Ϣ
	iRet = getaddrinfo(addr, portName, &hints, &res);
	if (iRet != 0) {
		// ��ȡ��������ַ��Ϣʧ��
		rstring errstr = "connect to server failed: getaddrinfo returned ";
		errstr.append(std::to_string(iRet));
		report(errstr);
		return false;
	}
	// ��ȡ�ɹ�
	ptr = res;
	while (ptr) {
		// ���ʵ�ַ��Ϣ������������׽��ּ�����ֱ��ȫ���ɹ�
		mSocket = INVALID_SOCKET;
		mSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		
		if (mSocket == INVALID_SOCKET) {
			rstring errstr = "connect to server failed: socket returned ";
			errstr.append(std::to_string(WSAGetLastError()));
			report(errstr);
		}
		else {
			// �׽��ִ����ɹ�
			report("create socket success");

			// ���ӵ���������ַ
			iRet = connect(mSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iRet == SOCKET_ERROR) {
				// ����ʧ��
				rstring errstr = "connect to server failed: connect returned ";
				errstr.append(std::to_string(WSAGetLastError()));
				report(errstr);
				closesocket(mSocket); // �ر��׽���
				mSocket = INVALID_SOCKET;
			}
			else {
				// ���ӳɹ�
				report("connect success");

				mServerAddr = *ptr->ai_addr; // �����������ַ
				break;
			}
		}
		// ȡ��һ����ַ���г���
		ptr = ptr->ai_next;
	}

	freeaddrinfo(res);
	// ptr��Ϊ��˵�����ӳɹ�
	if (ptr)
		return true;

	return false;
}

// ���׽����ж�ȡһ�е�����
// tmo Ϊ��ʱʱ�䣨��λ�룩��Ϊ0ʱ���޵ȴ�
int TCPClientSocket::readLine(char* buf, int len, int tmo)
{
	if (mSocket == INVALID_SOCKET) {
		return -1;
	}

	int sz = 0;
	fd_set rdset;  // ����׽����Ƿ�ɶ��ļ���

	// ��������ǰ����ȡ����
	while (sz < len) {
		FD_ZERO(&rdset);  // ���
		FD_SET(mSocket, &rdset);  // ���׽�����ӵ������н��ж�����

		// ���ó�ʱʱ��
		struct timeval timeout;
		timeout.tv_sec = tmo;
		timeout.tv_usec = 0;

		// �����Ƿ�ɶ�
		int status = select(mSocket+1, &rdset, NULL, NULL, tmo != 0 ? &timeout : NULL);
		if (status <= 0) {
			// ����ʧ��
			if (status == 0)
				report("readline failed: select timeout");
			else {
				rstring errstr = "readline failed: select returned ";
				errstr.append(std::to_string(WSAGetLastError()));
				report(errstr);
			}

			return -1;
		}
		// ȡ������
		int r = recv(mSocket, buf + sz, len - 1 - sz, 0);
		if (r <= 0) {
			// recvʧ��
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
		buf[sz] = '\0';  // ��ʱ�Ľ������

		char* str = strstr(buf, "\r\n");  // ���н������
		if (str) {
			*str = '\0';  // ��������
			return sz;
		}
	}

	return sz;
}

// 
// tmo Ϊ��ʱʱ�䣨��λ�룩��Ϊ0ʱ���޵ȴ�
int TCPClientSocket::readBlock(char* buf, int len, int tmo)
{
	if (mSocket == INVALID_SOCKET) {
		return -1;
	}

	int cur_len = 0;

	fd_set rdset;  // ����׽����Ƿ�ɶ��ļ���
	FD_ZERO(&rdset);
	FD_SET(mSocket, &rdset);

	// ���ó�ʱʱ��
	struct timeval timeout;
	timeout.tv_sec = tmo;
	timeout.tv_usec = 0;

	int status = select(mSocket + 1, &rdset, NULL, NULL, tmo != 0 ? &timeout : NULL);  // ����Ƿ�ɶ�

	if (status <= 0) {
		// ����ʧ��
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
		// recvʧ��
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

// ���׽�����д����
// tmo Ϊ��ʱʱ�䣨��λ�룩��Ϊ0ʱ���޵ȴ�
int TCPClientSocket::write(char* buf, int len, int tmo) 
{
	if (mSocket == INVALID_SOCKET) {
		return -1;
	}

	int sz = 0;  // �Ѿ�д�˵��ֽ���
	fd_set wrset;  // ����׽����Ƿ��д�ļ���

	// ����дֱ��д��
	while (sz < len) {
		FD_ZERO(&wrset);
		FD_SET(mSocket, &wrset);  // ���׽�����ӵ����Լ�����

		// ���ó�ʱʱ��
		struct timeval timeout;
		timeout.tv_sec = tmo;
		timeout.tv_usec = 0;

		// ����Ƿ��д
		int status = select(mSocket + 1, NULL, &wrset, NULL, tmo != 0 ? &timeout : NULL);
		if (status <= 0) {
			// ����ʧ��
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
			// sendʧ��
			rstring errstr = "write failed: send returned ";
			errstr.append(std::to_string(WSAGetLastError()));
			report(errstr);
		}

		sz += r;
	}

	return sz;
}

// ������Ϣ
void TCPClientSocket::report(rstring info)
{
	std::cout << "TCPClientSocket " << info << std::endl;
}