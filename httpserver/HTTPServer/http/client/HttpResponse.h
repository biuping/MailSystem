#pragma once
#include "..\static\Tools.h"

class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse();
	//设置版本号
	void set_version(const rstring& version);
	//设置状态码和短语
	void set_status(const rstring& code, const rstring& phrase);
	//添加头字段
	int add_head(const rstring& name, const rstring& attr);
	//删除头字段
	int del_head(const rstring& name);
	//设置响应体
	int set_body(const char* body, size_t bodylen);
	size_t size();
	//序列化报文
	const char* serialize();


private:
	enum Config {
		MAXLINE = 1024,
		BODY_MAXSIZE = 64 * 1024,
	};
	typedef struct {
		rstring version;
		rstring code;
		rstring phrase;
		HttpHead_t headers;
		char* body;
		size_t bodylen;
		char* data;
		size_t datalen;
		size_t totalsize;
		bool dirty;
	}respose_package_t;

private:
	respose_package_t m_package;
};