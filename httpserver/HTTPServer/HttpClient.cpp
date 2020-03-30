#include "HttpClient.h"

HttpClient::HttpClient():m_socket(INVALID_SOCKET)
{
}

HttpClient::HttpClient(SOCKET sock):m_socket(sock)
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
	return ::recv(m_socket,buf,len,flags);
}

int HttpClient::send(const char* buf, int len, int flags)
{
	if (m_socket == INVALID_SOCKET)
		return -1;
	return ::send(m_socket, buf, len, flags);
}

int HttpClient::close()
{
	if(m_socket==INVALID_SOCKET)
		return 0;
	int ret = ::closesocket(m_socket);
	if (ret == 0x00)
		m_socket = INVALID_SOCKET;
	return ret;
}
