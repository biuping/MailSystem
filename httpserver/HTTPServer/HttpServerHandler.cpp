#include "HttpServerHandler.h"

HttpServerHandler::HttpServerHandler(HttpClient* client):
	m_client(client)
{
	m_readbuff = nullptr;
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
	request.load_packet(m_readbuff, nread);

	HttpResponse* response = handle_request(request);
}

HttpResponse* HttpServerHandler::handle_request(HttpRequest& request)
{
	return nullptr;
}
