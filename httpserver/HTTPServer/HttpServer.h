#pragma once
#include "HttpSocket.h"
#include "HttpClient.h"
#include "config.h"
class HttpServer
{
private:
	USHORT m_port;
	SOCKET m_socket;
public:
	HttpServer(USHORT port=PORT_NUM);
	~HttpServer();
	SOCKET get_socket();
	int start(int backlog=BACKLOG_SIZE);
	int close();
	bool is_close();
	HttpClient* accept();
};

