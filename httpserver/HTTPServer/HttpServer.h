#pragma once
#include "HttpSocket.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServerHandler.h"
#include "config.h"

class HttpServer
{
private:
	USHORT m_port;
	SOCKET m_socket;
	
public:
	HttpServer(USHORT port=PORT_NUM);
	~HttpServer();
	SOCKET get_socket();
	//��ʼ��
	int start(int backlog=BACKLOG_SIZE);
	//�رշ����
	int close();
	bool is_close();
	//��ʼ���У�������ʼ����
	void run();
protected:
	HttpClient* accept();

};


