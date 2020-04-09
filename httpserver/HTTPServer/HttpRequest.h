#pragma once
#include <map>
#include "HttpClient.h"
#include "Tools.h"

class HttpRequest 
{
	
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
	int load_packet(const char* msg, size_t msglen);
	//��ȡ������ʼ��
	const rstring& start_line();
	//��ȡhttp���ķ���
	const rstring& method();
	//��ȡ����url
	const rstring& url();
	//��ȡhttp�汾
	const rstring& version();
	//��ȡͷ��map
	const HttpHead_t& headers();
	//�Ƿ���ͷ��
	bool has_head(const rstring& name);
	//���������ҵ�ͷ������
	const rstring& head_content(const rstring& name);
	//body����
	const size_t body_len();
	//body����
	const char* body();
private:
	int parse_startline(const char* start, const char* end);
	int parse_headers(const char* start, const char* end);
	int parse_body(const char* start, const char* end);
private:
	HttpClient* m_client;

	rstring m_strerr;
	rstring m_startline;
	rstring m_method;
	rstring m_url;
	rstring m_version;
	HttpHead_t m_headers;
	char* m_body;
};