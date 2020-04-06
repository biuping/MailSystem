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

// ���� TCPClientSocket ���󣬲����ӵ� (addr,port,TCP) ָ������������׽���
// addr ��Ϊ������ip��ַ
// tmo Ϊ��ʱʱ�䣨��λ�룩��Ϊ0ʱ���޵ȴ�
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

// �ر��׽���
void TCPClientSocket::closeSocket()
{
	if (mSocket != INVALID_SOCKET) {
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
}

// �����׽����Ƿ������ӣ��׽��ִ����ɹ���
bool TCPClientSocket::connected()
{
	return mSocket == INVALID_SOCKET;
}

// ����������ģʽ���׽��ֲ����ӵ� info ��ָ����ַ���׽���
// �����ɹ����� true��mSocket��Ϊ��Ӧ�׽���
// ����ʧ�ܷ��� false��mSocket��Ϊ INVALID_SOCKET
// tmo Ϊ��ʱʱ�䣨��λ�룩��Ϊ0ʱ���޵ȴ�
bool TCPClientSocket::createNBSocketAndConnect(addrinfo* info, int tmo) 
{
	mSocket = INVALID_SOCKET;
	mSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

	if (mSocket == INVALID_SOCKET) {
		// �����׽���ʧ��
		rstring errstr = "connect to server failed: socket returned ";
		errstr.append(std::to_string(WSAGetLastError()));
		report(errstr);
		return false;
	}
	else {
		// �׽��ִ����ɹ�
		report("create socket success");

		DWORD arg = 1;
		ioctlsocket(mSocket, FIONBIO, &arg);  // ���÷�����ģʽ

		// ���ӵ���������ַ
		int iRet = connect(mSocket, info->ai_addr, (int)info->ai_addrlen);
		if (iRet == SOCKET_ERROR) {
			// ��������ʧ��
			int error = WSAGetLastError();
			
			if (error != WSAEWOULDBLOCK) {
				// �� WSAEWOULDBLOCK �����޷�����
				rstring errstr = "connect to server failed: connect returned ";
				errstr.append(std::to_string(error));
				report(errstr);
				this->closeSocket();

				return false;
			}
			else {
				// WSAEWOULDBLOCK ���������޷�������������֮�� select ���ö��׽���дʱ���������
				fd_set wrset, exset;   // �׽��ֶ����ԡ���������Լ���
				// ������ģʽ�£�connect ��ͼ���ӵ�ʧ�ܻ������ exceptfds ��
				FD_ZERO(&wrset); FD_ZERO(&exset);
				FD_SET(mSocket, &wrset); FD_SET(mSocket, &exset);

				// ���� select ��ʱʱ��
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
					// ��鵽����ʧ�ܵĴ���
					report("connect to server failed: select found error in exceptfds");

					this->closeSocket();
					return false;
				}
			}
		}
		// ���ӳɹ�
		report("connect to server success");

		mServerAddr = *info->ai_addr; // �����������ַ

		return true;
	}
}

// ������������ַ��ip���������Ͷ˿ڣ������׽��ֲ����ӵ���������ַ
// addr ����Ϊ������ip��ַ
// �����Ƿ����ӳɹ�
bool TCPClientSocket::connect2Server(const char* addr, USHORT port, int tmo)
{
	closeSocket();  // ���֮ǰ�����ѽ������׽�����ر�

	addrinfo* res = nullptr,
		* ptr = nullptr,
		hints;
	int iRet;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // δָ����������
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_protocol = IPPROTO_TCP;

	// ��ȡ��������ַ��Ϣ
	iRet = getaddrinfo(addr, std::to_string(port).c_str(), &hints, &res);
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
		// ���ʵ�ַ��Ϣ������������׽��ּ�����ֱ���ɹ�����
		if (createNBSocketAndConnect(ptr)) {
			// �����׽��ֲ����ӳɹ�
			break;
		}

		// ����ʧ�ܣ�ȡ��һ����ַ���г���
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
// ���ض�ȡ���ֽ�����ʧ�ܷ���-1
int TCPClientSocket::readline(char* buf, int len, int tmo)
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
		int status = select(mSocket+1, &rdset, nullptr, nullptr, tmo != 0 ? &timeout : nullptr);
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

// ��ȡһ������
// tmo Ϊ��ʱʱ�䣨��λ�룩��Ϊ0ʱ���޵ȴ�
// ���ض�ȡ���ֽ�����ʧ�ܷ���-1
int TCPClientSocket::readblock(char* buf, int len, int tmo)
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

	int status = select(mSocket + 1, &rdset, nullptr, nullptr, tmo != 0 ? &timeout : nullptr);  // ����Ƿ�ɶ�

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
// ����д����ֽ�����ʧ�ܷ���-1
int TCPClientSocket::write(const char* buf, int len, int tmo) 
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
		int status = select(mSocket + 1, nullptr, &wrset, nullptr, tmo != 0 ? &timeout : nullptr);
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