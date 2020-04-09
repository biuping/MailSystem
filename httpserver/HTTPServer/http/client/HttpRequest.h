#pragma once
#include <map>
#include "HttpClient.h"
#include "..\static\Tools.h"

class HttpRequest 
{
	
public:

	HttpRequest();
	~HttpRequest();

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
	//�Ƿ�ͷ��ĳ����
	bool has_head(const rstring& key);
	//���������ҵ�ͷ������
	const rstring& head_content(const rstring& key);
	//body����
	const size_t body_len();
	//body����
	const char* body();
private:
	/*
	POST /hello?name=12&age=34 HTTP/1.1\r\nContent-Type: application/json\r\nHost: 127.0.0.1:8006\r\nConnection: keep-alive\r\nContent-Length: 31\r\n\r\n{\n\t\"name\":\"13\",\n\t\"type\":\"pop\"\n}
	*/
	int parse_startline(const char* start, const char* end);
	int parse_headers(const char* start, const char* end);
	int parse_body(const char* start, const char* end);
private:

	rstring m_strerr;
	rstring m_startline;
	rstring m_method;
	rstring m_url;
	rstring m_version;
	HttpHead_t m_headers;
	char* m_body;
	size_t m_bodylen;
};