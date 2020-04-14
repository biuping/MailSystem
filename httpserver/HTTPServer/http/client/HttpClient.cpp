#include "HttpClient.h"

HttpClient::HttpClient() :m_socket(INVALID_SOCKET)
{
}

HttpClient::HttpClient(SOCKET sock) : m_socket(sock)
{
}

HttpClient::HttpClient(const HttpClient& httpclient)
{
	m_socket = httpclient.m_socket;
}

HttpClient::~HttpClient()
{
	close();
}

SOCKET HttpClient::get_socket()
{
	return m_socket;
}

int HttpClient::recv(char* buf, int len, int flags)
{
	if (m_socket == INVALID_SOCKET)
		return -1;
	int res = ::recv(m_socket, buf, len, flags);

	return res;
}

int HttpClient::send(const char* buf, int len, int flags)
{
	if (m_socket == INVALID_SOCKET)
		return -1;
	int res = ::send(m_socket, buf, len, flags);
	if (SOCKET_ERROR == res)
		Tools::report("send error: " + std::to_string(WSAGetLastError()));
	return res;
}

int HttpClient::close()
{
	if (m_socket == INVALID_SOCKET)
		return -1;

	if (::closesocket(m_socket) == SOCKET_ERROR)
	{
		Tools::report("close client socket error: " + WSAGetLastError());
		return -1;
	}
	m_socket = INVALID_SOCKET;
	delete this;
	return 0;
}
