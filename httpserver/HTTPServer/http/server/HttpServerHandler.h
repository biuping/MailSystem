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
	//����ͻ�
	void handle_client();
	//��������
	HttpResponse* handle_request(HttpRequest& request);
};

