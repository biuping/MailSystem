#ifndef _TCPCLIENT_SOCKET_H_
#define _TCPCLIENT_SOCKET_H_

#define DEFAULT_READ_TIMEOUT 10
#define DEFAULT_WRITE_TIMEOUT 10

#include <WinSock2.h>
#include <WS2tcpip.h>
#include "../tools/LogUtils.h"


class TCPClientSocket
{
public:
	TCPClientSocket();
	TCPClientSocket(const TCPClientSocket& tcpcliSocket);
	TCPClientSocket(const char* addr, USHORT port, int tmo = DEFAULT_WRITE_TIMEOUT);
	virtual ~TCPClientSocket();

	virtual bool connect(const char* addr, USHORT port, int tmo = DEFAULT_WRITE_TIMEOUT);
	const sockaddr& getServerAddr();
	
	void closeSocket();
	int readline(char* buf, int len, int tmo = DEFAULT_READ_TIMEOUT);
	int readblock(char* buf, int len, int tmo = DEFAULT_READ_TIMEOUT);
	int write(const char* buf, int len, int tmo = DEFAULT_WRITE_TIMEOUT);

	bool connected();

protected:
	sockaddr mServerAddr;
	SOCKET mSocket;

private:
	bool createNBSocketAndConnect(addrinfo* info, int tmo = DEFAULT_WRITE_TIMEOUT);
	virtual void report(const rstring& msg);
};


#endif // !_TCPCLIENT_SOCKET_H_