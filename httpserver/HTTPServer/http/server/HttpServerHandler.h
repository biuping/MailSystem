#pragma once
#include "..\server\HttpServer.h"
#include "..\static\MultipartReader.h"
#include "..\static\MultipartRecord.h"
#include "..\..\mail_cli\MailClient.h"
#include "..\..\mail_cli\MailSender.h"
#include "..\..\mail_cli\MailReceiver.h"
#include "..\..\mail_cli\pop3\POP3Client.h"
#include "..\..\mail_cli\smtp\SMTPClient.h"

//存储用户信息
static UserStore userStore;

class HttpServerHandler
{
private:
	HttpClient* m_client;
	char* m_readbuff;
	Json::Value m_object;
	Json::String m_err;
public:
	HttpServerHandler();
	HttpServerHandler(HttpClient* client);
	~HttpServerHandler();

public:
	//处理客户
	void handle_client();
	//设置客户
	void set_client(HttpClient* client);
	//处理请求
	HttpResponse* handle_request(HttpRequest& request);
	//通过uuid找到用户
	UserInfo* FindUserByUUID(rstring& uuid);
	//添加用户，返回uuid
	rstring AddUser(rstring& email_addr, rstring& pass);
	//清理用户
	void ClearUsers();
	//通过uuid清理用户，登出效果
	void ClearUserByUUID(rstring& uuid);

public:
	void Login(HttpResponse* response);
	void SendWithAttach(HttpResponse* response,MultipartRecord& record);
	void SendNoAttach(HttpResponse* response);
	void RecvWithAttach(HttpResponse* response);
	void RecvNoAttach(HttpResponse* response);
	void DownloadAttach(HttpResponse* response);
	void DeleteMail(HttpResponse* response);
	UserInfo* AuthUserById(HttpResponse* response, rstring& uuid);
	bool AuthLogin(HttpResponse* response, MailClient& client, MailSender* sender, MailReceiver* receiver, rstring& uuid);

};

