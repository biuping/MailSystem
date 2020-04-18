#pragma once
#include "..\server\HttpServer.h"

class HttpServerHandler
{
private:
	HttpClient* m_client;
	char* m_readbuff;
	Json::Value m_object;
	Json::String m_err;
public:
	HttpServerHandler();
	HttpServerHandler(HttpClient* client);
	~HttpServerHandler();
	//处理客户
	void handle_client();
	void set_client(HttpClient* client);
	//处理请求
	HttpResponse* handle_request(HttpRequest& request);

public:
	void Login(HttpResponse* response);
	void SendWithAttach(HttpResponse* response);
	void SendNoAttach(HttpResponse* response);
	void RecvWitnAttach(HttpResponse* response);
	void RecvNoAttach(HttpResponse* response);
	void DownloadAttach(HttpResponse* response);
	void DeleteMail(HttpResponse* response);
};

