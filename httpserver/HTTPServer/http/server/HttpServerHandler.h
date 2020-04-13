#pragma once
#include "..\server\HttpServer.h"

class HttpServerHandler
{
private:
	HttpClient* m_client;
	char* m_readbuff;
public:
	HttpServerHandler(HttpClient* client);
	~HttpServerHandler();
	//处理客户
	void handle_client();
	//处理请求
	HttpResponse* handle_request(HttpRequest& request);

};

