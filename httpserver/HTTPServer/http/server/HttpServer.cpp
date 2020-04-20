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

	return 0;
}

int HttpServer::start_non_block(int backlog)
{

	if (start() < 0)
	{
		return -1;
	}

	// 设置非阻塞模式
	u_long unblock = 1;
	int nRet = ::ioctlsocket(m_socket, FIONBIO, &unblock);
	if (nRet == SOCKET_ERROR)
	{
		Tools::report("ioctlsocket failed! error:", WSAGetLastError());
		return -1;
	}

	HttpClient* client;
	HttpServerHandler handler;

	SOCKET max_sd, new_sd; //记录最大描述符和新建描述符
	int  desc_ready, rc = 0;
	bool  close_conn = false, end_server = false;
	struct timeval timeout;//超时时间
	fd_set master_set, working_set;

	//初始化
	FD_ZERO(&master_set);
	SOCKET& listen_sd = m_socket;
	max_sd = listen_sd;
	FD_SET(listen_sd, &master_set); //将服务器套接字放入读集合

	//设置超时时间3min
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;

	do {
		memcpy(&working_set, &master_set, sizeof(master_set));

		Tools::report("Waiting on select");
		rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);

		if (rc == SOCKET_ERROR)
		{
			Tools::report("Select Error: ", WSAGetLastError());
			break;
		}
		if (rc == 0)
		{
			Tools::report("select() timed out.  End Server.");
			break;
		}
		//有可用描述符
		desc_ready = rc;
		for (int i = 0; i <= max_sd && desc_ready > 0; ++i)
		{
			if (FD_ISSET(i, &working_set))
			{
				//可用数量-1
				desc_ready -= 1;

				//有可用连接
				if (i == listen_sd)
				{
					do
					{
						//接受连接
						new_sd = ::accept(listen_sd, NULL, NULL);
						if (new_sd == INVALID_SOCKET)
						{
							if (WSAGetLastError() != WSAEWOULDBLOCK)
							{
								Tools::report("accept() failed", WSAGetLastError());
								end_server = true;
							}
							break;
						}

						//将连接加入描述符集
						FD_SET(new_sd, &master_set);
						if (new_sd > max_sd)
							max_sd = new_sd;

						//返回接受其他连接
					} while (new_sd != INVALID_SOCKET);
				}
				
				//有数据可以接受
				else
				{
					client = new HttpClient(i);
					handler.set_client(client);
					handler.handle_client();

					client->close();
					FD_CLR(i, &master_set);
					if (i == max_sd)
					{
						while (FD_ISSET(max_sd, &master_set) == FALSE)
							max_sd -= 1;
					}
				}
			}/* End of if (FD_ISSET(i, &working_set)) */
		}/*End of loop through selectable descriptors*/
	} while (end_server != true);

	//关闭所有套接字
	//for (int i = 0; i <= max_sd; ++i)
	//{
	//	if (FD_ISSET(i, &master_set))
	//		::closesocket(i);
	//}

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

void HttpServer::run(bool block)
{
	if (!block)
	{
		if (this->start_non_block() < 0) {
			Tools::report("start failed");
			this->close();
		}
		else {
			Tools::report("Server shutdown");
		}
	}

	if (block) {
		if (this->start()< 0) {
			Tools::report("start failed");
			this->close();
		}
		else {
			Tools::report("start success");
		}

		HttpClient* client;
		HttpServerHandler* handler;

		while (nullptr != (client = this->accept()))
		{
			handler = new HttpServerHandler(client);
			handler->handle_client();
			delete handler;
		}
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
	//SOCKET client_sock = ::accept(m_socket, nullptr, nullptr);
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


