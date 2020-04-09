#include "HttpServerHandler.h"

HttpServerHandler::HttpServerHandler(HttpClient* client):
	m_client(client)
{
	m_readbuff = new char[DEFAULT_BUFF_SIZE+1];
}

HttpServerHandler::~HttpServerHandler()
{
	delete m_client;
	m_client = nullptr;

	delete m_readbuff;
	m_readbuff = nullptr;
}

void HttpServerHandler::handle_client()
{
	int nread = m_client->recv(m_readbuff, DEFAULT_BUFF_SIZE, 0);
	m_readbuff[nread] = 0x00;

	HttpRequest request;
	if (request.load_packet(m_readbuff, nread) < 0)
	{
		Tools::report("Parse package error");
		return;
	}

	HttpResponse* response = handle_request(request);
	if (nullptr != response)
	{
		//m_client->send(response->serialize(), response->size(), 0);
		//delete response;
	}
}

HttpResponse* HttpServerHandler::handle_request(HttpRequest& request)
{
	return nullptr;
}
