#pragma once
#include "HttpSocket.h"
class HttpClient 
{
private:
	SOCKET m_socket;
public:
	HttpClient();
	HttpClient(SOCKET sock);
	HttpClient(const HttpClient& httpclient);
	~HttpClient();
	SOCKET get_socket();
	int recv(char* buf, int len, int flags);
	int send(const char* buf, int len, int flags);
	int close();
};