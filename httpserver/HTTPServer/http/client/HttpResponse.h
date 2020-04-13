#pragma once
#include "..\static\Tools.h"

class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse();
	//���ð汾��
	void set_version(const rstring& version);
	//����״̬��Ͷ���
	void set_status(const rstring& code, const rstring& phrase);
	//���ͷ�ֶ�
	int add_head(const rstring& name, const rstring& attr);
	//ɾ��ͷ�ֶ�
	int del_head(const rstring& name);
	//������Ӧ��
	int set_body(const char* body, size_t bodylen);
	size_t size();
	//���л�����
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