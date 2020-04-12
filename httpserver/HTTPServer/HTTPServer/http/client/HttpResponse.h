#pragma once
#include "..\static\Tools.h"
#include "json/json.h"

class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse();
	int set_version(const rstring& version);
	int set_status(const rstring& status, const rstring& reason);
	int add_head(const rstring& name, const rstring& attr);
	int del_head(const rstring& name);
	int set_body(const char* body, size_t bodylen);
	size_t size();
	//序列化报文
	const char* serialize();

private:
	size_t startline_stringsize();
	size_t headers_stringsize();

private:
	typedef struct {
		rstring version;
		rstring status;
		rstring reason;
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