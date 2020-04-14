#include "HttpSocket.h"

HttpSocket::HttpSocket()
{
	
	int err = WSAStartup(MAKEWORD(2, 0), &_wsa);
	if (err != 0) 
	{
		std::cout << "Init error: " << err << std::endl;
		return;
	}
}

HttpSocket::~HttpSocket()
{
	int err = WSACleanup();
	if (err != 0)
	{

		std::cout << "Clean error: " << std::to_string(WSAGetLastError()) << std::endl;
		return;
	}
}
