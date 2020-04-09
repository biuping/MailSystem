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
	//获取报文起始行
	const rstring& start_line();
	//获取http报文方法
	const rstring& method();
	//获取报文url
	const rstring& url();
	//获取http版本
	const rstring& version();
	//获取头部map
	const HttpHead_t& headers();
	//是否有头部
	bool has_head(const rstring& name);
	//根据名称找到头部内容
	const rstring& head_content(const rstring& name);
	//body长度
	const size_t body_len();
	//body内容
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