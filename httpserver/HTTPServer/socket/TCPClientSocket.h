#ifndef _TCPCLIENT_SOCKET_H_
#define _TCPCLIENT_SOCKET_H_

#define DEFAULT_READ_TIMEOUT 10
#define DEFAULT_WRITE_TIMEOUT 10

#include <WinSock2.h>
#include <string>
#include <WS2tcpip.h>

typedef std::string rstring;


class TCPClientSocket
{
public:
	TCPClientSocket();
	TCPClientSocket(const TCPClientSocket& tcpcliSocket);
	TCPClientSocket(const char* addr, USHORT port, int tmo = DEFAULT_WRITE_TIMEOUT);
	virtual ~TCPClientSocket();

	virtual bool connect2Server(const char* addr, USHORT port, int tmo = DEFAULT_WRITE_TIMEOUT);
	const sockaddr& getServerAddr();
	
	void closeSocket();
	int readline(char* buf, int len, int tmo = DEFAULT_READ_TIMEOUT);
	int readblock(char* buf, int len, int tmo = DEFAULT_READ_TIMEOUT);
	int write(const char* buf, int len, int tmo = DEFAULT_WRITE_TIMEOUT);

	bool connected();
protected:
	sockaddr mServerAddr;
	SOCKET mSocket;
	virtual void report(rstring info);

private:
	bool createNBSocketAndConnect(addrinfo* info, int tmo = DEFAULT_WRITE_TIMEOUT);
};


#endif // !_TCPCLIENT_SOCKET_H_