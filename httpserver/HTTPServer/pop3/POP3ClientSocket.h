#ifndef  _POP3SOCKET_H_
#define _POP3SOCKET_H_
#endif // ! _POP3SOCKET_H_

#define DEFAULT_POP3_PORT 110

#include "TCPClientSocket.h"

using namespace std;


class POP3ClientSocket : TCPClientSocket
{
public:
	POP3ClientSocket(const char* addr, bool useIP, int port=DEFAULT_POP3_PORT);
	POP3ClientSocket(sockaddr_in serverAddr);
	POP3ClientSocket();

	void sendRequest();
	void recvResponse();

private:
	


};

