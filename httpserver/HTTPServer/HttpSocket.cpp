#include "HttpSocket.h"

HttpSocket::HttpSocket()
{
	WSAStartup(MAKEWORD(2, 0), &_wsa);
}

HttpSocket::~HttpSocket()
{
	WSACleanup();
}
