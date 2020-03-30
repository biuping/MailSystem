#include "HttpServer.h"

HttpServer::HttpServer(USHORT port) :
	m_port(port), m_socket(INVALID_SOCKET)
{
}

HttpServer::~HttpServer()
{
	close();
}

SOCKET HttpServer::get_socket()
{
	return m_socket;
}

int HttpServer::start(int backlog)
{

	if (m_socket != INVALID_SOCKET)
		return 0;

	//建立套接字，失败返回-1
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		return -1;

	sockaddr_in sock_addr = { 0 };
	//指定地址族
	sock_addr.sin_family = AF_INET;
	//初始化IP地址
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	//初始化端口号
	sock_addr.sin_port = htons(m_port);

	//绑定
	if (::bind(sock, (sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		::closesocket(sock);
		std::cout << "bind error: " + WSAGetLastError() << std::endl;
		return -1;
	}

	//监听
	if (::listen(sock, backlog) < 0) {
		::closesocket(sock);
		std::cout << "listen error: " + WSAGetLastError() << std::endl;
		return -1;
	}
	m_socket = sock;
	return 0;
}

int HttpServer::close()
{
	if (m_socket == INVALID_SOCKET)
		return -1;

	if (::closesocket(m_socket) < 0)
		return -1;

	m_socket == INVALID_SOCKET;

	return 0;
}

bool HttpServer::is_close()
{
	if (m_socket == INVALID_SOCKET)
		return true;
	return false;
}

HttpClient* HttpServer::accept()
{
	if (m_socket == INVALID_SOCKET)
		return NULL;
	//设置客户端
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	//接受客户端请求
	SOCKET client_sock = ::accept(m_socket, (sockaddr*)&client_addr, (socklen_t*)&client_addr_size);
	if (client_sock == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		std::cout<<"accept error: "+ WSAGetLastError()<<std::endl;
		return NULL;
	}
	
	HttpClient* client = new HttpClient(client_sock);

	return client;
}


