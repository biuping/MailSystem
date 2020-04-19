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
	//初始化
	int start(int backlog=BACKLOG_SIZE);
	//非阻塞方式启动
	int start_non_block(int backlog = BACKLOG_SIZE);
	//关闭服务端
	int close();
	bool is_close();
	//开始运行（包含初始化）
	//默认非阻塞形式
	void run(bool block=false);

protected:
	HttpClient* accept();

};


