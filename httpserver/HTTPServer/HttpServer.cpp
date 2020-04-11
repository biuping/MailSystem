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

	//�����׽��֣�ʧ�ܷ���-1
	//��ʽ�׽���: �����ڿͻ�����˳���͵ģ����ҵ����˳����һ�µġ�
	//�������ڿͻ����ȷ���1���ٷ���2����ô�ڷ������˵Ľ���˳�����Ƚ��յ�1��
	//�ٽ��յ�2����ʽ�׽����ǿɿ��ģ����������ӵ�
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == SOCKET_ERROR)
	{
		std::cout << "create socket error: " << WSAGetLastError() << std::endl;
		return -1;
	}


	sockaddr_in sock_addr = { 0 };
	//ָ����ַ��
	sock_addr.sin_family = AF_INET;
	//��ʼ��IP��ַ
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//��ʼ���˿ں�
	sock_addr.sin_port = htons(m_port);

	//��
	if (::bind(sock, (sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		::closesocket(sock);
		std::cout << "bind error: " + WSAGetLastError() << std::endl;
		return -1;
	}

	//����
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

	if (::closesocket(m_socket) == SOCKET_ERROR)
	{
		Tools::report("close server socket error: " + WSAGetLastError());
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
	}
	else {
		Tools::report("start success");
	}

	HttpClient* client;
	HttpServerHandler* handler;
	while (NULL != (client = this->accept()))
	{
		handler = new HttpServerHandler(client);
		handler->handle_client();
		client->close();
	}
}

HttpClient* HttpServer::accept()
{
	if (m_socket == INVALID_SOCKET)
		return NULL;
	//���ÿͻ���
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	//���ܿͻ�������
	SOCKET client_sock = ::accept(m_socket, (sockaddr*)&client_addr, (socklen_t*)&client_addr_size);
	if (client_sock == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		std::cout << "accept error: " + WSAGetLastError() << std::endl;
		return NULL;
	}

	HttpClient* client = new HttpClient(client_sock);

	return client;
}

