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
	//流式套接字: 数据在客户端是顺序发送的，并且到达的顺序是一致的。
	//比如你在客户端先发送1，再发送2，那么在服务器端的接收顺序是先接收到1，
	//再接收到2，流式套接字是可靠的，是面向连接的
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == SOCKET_ERROR)
	{
		Tools::report("create socket error: ", WSAGetLastError());
		return -1;
	}


	sockaddr_in sock_addr = { 0 };
	//指定地址族
	sock_addr.sin_family = AF_INET;
	//初始化IP地址
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//初始化端口号
	sock_addr.sin_port = htons(m_port);

	//绑定
	if (::bind(sock, (sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		::closesocket(sock);
		Tools::report("bind error: ", WSAGetLastError());
		return -1;
	}

	//监听
	if (::listen(sock, backlog) < 0) {
		::closesocket(sock);
		Tools::report("listen error: ", WSAGetLastError());
		return -1;
	}

	m_socket = sock;

	// 设置非阻塞模式
	/*DWORD arg = 1;
	int nRet = ::ioctlsocket(m_socket, FIONBIO, &arg);
	if (nRet == SOCKET_ERROR)
	{
		Tools::report("ioctlsocket failed! error:", WSAGetLastError());
		return -1;
	}*/

	return 0;
}

int HttpServer::close()
{
	if (m_socket == INVALID_SOCKET)
		return -1;

	if (::closesocket(m_socket) == SOCKET_ERROR)
	{
		Tools::report("close server socket error: ", WSAGetLastError());
		return -1;
	}

	m_socket = INVALID_SOCKET;
	delete this;
	return 0;
}

bool HttpServer::is_close()
{
	if (m_socket == INVALID_SOCKET)
		return true;
	return false;
}

void HttpServer::run()
{
	if (this->start() < 0) {
		Tools::report("start failed");
		this->close();
	}
	else {
		Tools::report("start success");
	}

	HttpClient* client;
	HttpServerHandler* handler;
	HttpRequest* request;
	//while (true)
	//{
	//	if (nullptr != (client = this->accept()))
	//	{
	//		request = new HttpRequest(client);
	//		request->handle_request();
	//		client->close();
	//	}
	//}

	while (nullptr != (client = this->accept()))
	{
		handler = new HttpServerHandler(client);
		handler->handle_client();
		client->close();
	}
}

HttpClient* HttpServer::accept()
{
	if (m_socket == INVALID_SOCKET)
		return nullptr;
	//设置客户端
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	//接受客户端请求
	SOCKET client_sock = ::accept(m_socket, (sockaddr*)&client_addr, (socklen_t*)&client_addr_size);
	if (client_sock == INVALID_SOCKET)
	{
		//int error = WSAGetLastError();
		//Tools::report( "accept error: ", WSAGetLastError());
		return nullptr;
	}


	u_long unblock = 1;    //非0为非阻塞模式
	if (ioctlsocket(client_sock, FIONBIO, &unblock) == SOCKET_ERROR)
	{
		Tools::report("set client unblock error: ", WSAGetLastError());
		return nullptr;
	}

	HttpClient* client = new HttpClient(client_sock);

	return client;
}


