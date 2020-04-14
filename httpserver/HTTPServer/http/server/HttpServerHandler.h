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
	//´¦Àí¿Í»§
	void handle_client();
	//´¦ÀíÇëÇó
	HttpResponse* handle_request(HttpRequest& request);
};

