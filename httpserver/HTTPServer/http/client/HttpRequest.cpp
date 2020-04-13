#include "HttpRequest.h"

HttpRequest::HttpRequest() :m_body(nullptr), m_bodylen(0)
{

}

HttpRequest::~HttpRequest()
{
	if (m_body != nullptr)
		delete m_body;
}

int HttpRequest::load_packet(const char* msg, size_t msglen)
{
	//����ʣ��δ������Ϣ
	const char* remain_msg = msg;
	//������Ϣ��ĩβ'\0'
	const char* end_msg = msg + msglen;
	//�ҵ���ʼ�н���λ��
	const char* start_line_end = Tools::find_line(remain_msg, end_msg);
	if (start_line_end == nullptr) {
		Tools::report("ERROR: Can't find request header line.");
	}

	//������ʼ������
	if (parse_startline(remain_msg, start_line_end) < 0) {
		Tools::report("ERROR: Invalid startline");
		return -1;
	}
	//����һ��
	//remain_msg = start_line_end;

	//�ҵ�ͷ���н���λ��
	const char* head_line_end = Tools::find_head(remain_msg, end_msg);
	if (head_line_end == nullptr) {
		Tools::report("ERROR: Head line not found");
		return -1;
	}
	//����ͷ��
	if (parse_headers(start_line_end, head_line_end) < 0) {
		Tools::report("ERROR: Parse headers error");
		return -1;
	}

	//����
	//remain_msg = head_line_end;
	//����body
	if (parse_body(head_line_end, end_msg) < 0) {
		Tools::report("ERROR: Parse body error");
		return -1;
	}
	return 0;
}

int HttpRequest::parse_startline(const char* start, const char* end)
{
	size_t content_len = 0, sum_len = 0;
	const char* remain = start, * content_start = nullptr;
	content_start = Tools::find_content(remain, end, '\r', content_len, sum_len);
	if (content_start == nullptr)
		return -1;
	m_startline = rstring(remain, content_len); //ͷ����

	content_start = Tools::find_content(remain, end, ' ', content_len, sum_len);
	if (content_start == nullptr)
		return -1;
	m_method = rstring(content_start, content_len); //���󷽷�
	remain += sum_len;

	content_start = Tools::find_content(remain, end, ' ', content_len, sum_len);
	if (content_start == nullptr)
		return -1;
	m_url = rstring(content_start, content_len); //����url
	remain += sum_len;

	content_start = Tools::find_content(remain, end, '\r', content_len, sum_len);
	if (content_start == nullptr)
		return -1;
	m_version = rstring(content_start, content_len); //����url
	return 0;
}

int HttpRequest::parse_headers(const char* start, const char* end)
{
	size_t content_len = 0, sum_len = 0;
	const char* line_start = start;
	rstring head, attr;
	m_headers.clear();

	while (true)
	{
		const char* line_end = Tools::find_line(line_start, end);
		if (line_end == nullptr)
			return -1;
		else if (line_end == end)//����
			break;

		//�ҵ�':'ǰ��head
		const char* head_start = Tools::find_content(line_start, line_end, ':', content_len, sum_len);
		if (head_start == NULL)
			return -1;
		head = rstring(head_start, content_len);
		Tools::to_upper(head);//ͳһתΪ��д

		//�ҵ�':'���attrbute, +0x01����Ϊȥ��':'���һ���ո�
		const char* attr_start = line_start + sum_len + 0x01;
		attr_start = Tools::find_content(attr_start, line_end, '\r', content_len, sum_len);
		if (attr_start == NULL)
			return -1;
		attr = rstring(attr_start, content_len);

		line_start = line_end;
		//�Ѽ�ֵ�Լ���map
		m_headers[head] = attr;
	}
	return 0;
}

int HttpRequest::parse_body(const char* start, const char* end)
{
	size_t body_len = Tools::cal_len(start, end);
	if (body_len == 0x00)
		return 0;

	char* buff = new char[body_len + 1];
	if (buff == nullptr)
		return -1;
	//��body����������
	memcpy(buff, start, body_len);
	//������ֹλ��
	buff[body_len] = 0x00;
	//��Ϊ�գ����
	if (m_body != nullptr)
		delete m_body;

	m_body = buff;
	m_bodylen = body_len;
	return 0;
}


const rstring& HttpRequest::start_line()
{
	return m_startline;
}

const rstring& HttpRequest::method()
{
	return m_method;
}

const rstring& HttpRequest::url()
{
	return m_url;
}

const rstring& HttpRequest::version()
{
	return m_version;
}

const HttpHead_t& HttpRequest::headers()
{
	return m_headers;
}

bool HttpRequest::has_head(const rstring& key)
{
	HttpHead_t::iterator itor = m_headers.find(key);

	return itor != m_headers.end();
}

const rstring& HttpRequest::head_content(const rstring& key)
{
	HttpHead_t::iterator itor = m_headers.find(key);
	if (itor != m_headers.end())
		return itor->second;
	return nullptr;
}

const size_t HttpRequest::body_len()
{
	return m_bodylen;
}

const char* HttpRequest::body()
{
	return m_body;
}
