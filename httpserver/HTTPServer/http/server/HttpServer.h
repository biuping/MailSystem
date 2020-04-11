#pragma once
#include "..\socket\HttpSocket.h"
#include "..\client\HttpClient.h"
#include "..\client\HttpRequest.h"
#include "..\client\HttpResponse.h"
#include "HttpServerHandler.h"
#include "..\static\config.h"

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


