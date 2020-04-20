#include "HttpResponse.h"

HttpResponse::HttpResponse()
{

	m_package.body = nullptr;
	m_package.bodylen = 0x00;
	m_package.data = nullptr;
	m_package.datalen = 0x00;
}

HttpResponse::~HttpResponse()
{
	if (m_package.body != nullptr)
		delete[] m_package.body;
	if (m_package.data != nullptr)
		delete[] m_package.data;
}

void HttpResponse::set_version(const rstring& version)
{
	m_package.version = version;
}

void HttpResponse::set_status(const rstring& code, const rstring& phrase)
{
	m_package.code = code;
	m_package.phrase = phrase;
}

int HttpResponse::add_head(const rstring& name, const rstring& attr)
{
	if (name.empty() || attr.empty())
		return -1;
	m_package.headers[name] = attr;
	return 0;
}

int HttpResponse::del_head(const rstring& name)
{
	HttpHead_t::iterator itor = m_package.headers.find(name);
	if (itor == m_package.headers.end())
		return -1;
	m_package.headers.erase(itor);
	return 0;
}

int HttpResponse::set_body(const char* body, size_t bodylen)
{
	if (body == nullptr || bodylen == 0x00 || bodylen > BODY_MAXSIZE)
		return -1;

	char* buff = new char[bodylen];

	memcpy(buff, body, bodylen);

	if (m_package.body != nullptr)
		delete[] m_package.body;
	m_package.body = buff;
	m_package.bodylen = bodylen;
	return 0;
}

size_t HttpResponse::size()
{
	return m_package.totalsize;
}

const char* HttpResponse::serialize()
{
	rstring* builder = new rstring;
	//头部第一行
	(*builder).append(m_package.version).append(" ")
		.append(m_package.code).append(" ")
		.append(m_package.phrase).append("\r\n");

	//添加各字段
	for (HttpHead_t::iterator itor = m_package.headers.begin(); 
		itor != m_package.headers.end(); 
		itor++) 
	{
		const rstring& name = itor->first;
		const rstring& attr = itor->second;

		(*builder).append(name).append(": ")
			.append(attr).append("\r\n");
	}
	//body分割行
	(*builder).append("\r\n");
	(*builder).append(m_package.body, m_package.bodylen);
	
	//拷贝内存
	size_t len = (*builder).size();
	m_package.data = new char[len +1];
	memcpy(m_package.data, &(*builder)[0], len);
	m_package.data[len] = 0x00;
	m_package.totalsize = len + 1;
	delete builder;
	return m_package.data;
}

void HttpResponse::set_common()
{
	this->add_head("Access-Control-Allow-Origin", "*");
	this->add_head("Access-Control-Allow-Methods", "POST,GET,OPTIONS,DELETE");
	this->add_head("Access-Control-Allow-Credentials", "true");
	this->add_head("Access-Control-Allow-Headers", "Origin,X-Custom-Header,Authorization,Content-Type");
	this->add_head("Access-Control-Max-Age", "3600");
	this->set_version(HTTP_VERSION);
	this->add_head(HTTP_HEAD_CONNECTION, "close");
}

void HttpResponse::build_body(const rstring& body)
{
	this->add_head(HTTP_HEAD_CONTENT_LEN, std::to_string(body.size()));
	this->set_body(&body[0], body.size());
}

void HttpResponse::build_ok()
{
	this->set_common();
	this->set_status("200", "OK");

}

void HttpResponse::build_err()
{
	this->set_common();
	this->set_status("500", "INTERNAL SERVER ERROR");
}

void HttpResponse::build_not_found()
{
	this->set_common();
	this->set_status("404", "NOT FOUND");
}
