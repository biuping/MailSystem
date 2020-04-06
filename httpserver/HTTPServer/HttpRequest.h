#pragma once
#include <map>
#include "HttpClient.h"
#include "Tools.h"


typedef std::map<rstring, rstring> HttpHead_t;
typedef std::pair<rstring, rstring> HttpHeadPair_t;

class HttpRequest 
{
	
private:
	HttpClient* m_client;
public:

	rstring request_str;
	rstring response;
	HttpRequest();
	HttpRequest(HttpClient* client);
	int recv_data();
	int send_data(rstring res);
	rstring get_url();
	void url_router(rstring const& url);
	void handle_request();
	//----------------
	int load_packet(char* msg, size_t msglen);
	//��ȡ������ʼ��
	const rstring& start_line();
	//��ȡhttp���ķ���
	const rstring& method();
	//��ȡ����url
	const rstring& url();
	const rstring& version();
	const HttpHead_t& headers();
	bool has_head(const rstring& name);
	const rstring& head_content(const rstring& name);
	const size_t body_len();
	const char* body();
	const char* strerror();
};