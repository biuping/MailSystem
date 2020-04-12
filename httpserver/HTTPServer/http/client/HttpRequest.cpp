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
	//保存剩余未处理消息
	const char* remain_msg = msg;
	//保存消息的末尾'\0'
	const char* end_msg = msg + msglen;
	//找到起始行结束位置
	const char* start_line_end = Tools::find_line(remain_msg, end_msg);
	if (start_line_end == nullptr) {
		Tools::report("ERROR: Can't find request header line.");
	}

	//解析起始请求行
	if (parse_startline(remain_msg, start_line_end) < 0) {
		Tools::report("ERROR: Invalid startline");
		return -1;
	}
	//后移一行
	//remain_msg = start_line_end;

	//找到头部行结束位置
	const char* head_line_end = Tools::find_head(remain_msg, end_msg);
	if (head_line_end == nullptr) {
		Tools::report("ERROR: Head line not found");
		return -1;
	}
	//解析头部
	if (parse_headers(start_line_end, head_line_end) < 0) {
		Tools::report("ERROR: Parse headers error");
		return -1;
	}

	//后移
	//remain_msg = head_line_end;
	//解析body
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
	m_startline = rstring(remain, content_len); //头部行

	content_start = Tools::find_content(remain, end, ' ', content_len, sum_len);
	if (content_start == nullptr)
		return -1;
	m_method = rstring(content_start, content_len); //请求方法
	remain += sum_len;

	content_start = Tools::find_content(remain, end, ' ', content_len, sum_len);
	if (content_start == nullptr)
		return -1;
	m_url = rstring(content_start, content_len); //请求url
	remain += sum_len;

	content_start = Tools::find_content(remain, end, '\r', content_len, sum_len);
	if (content_start == nullptr)
		return -1;
	m_version = rstring(content_start, content_len); //请求url
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
		else if (line_end == end)//结束
			break;

		//找到':'前的head
		const char* head_start = Tools::find_content(line_start, line_end, ':', content_len, sum_len);
		if (head_start == NULL)
			return -1;
		head = rstring(head_start, content_len);
		Tools::to_upper(head);//统一转为大写

		//找到':'后的attrbute, +0x01是因为去掉':'后的一个空格
		const char* attr_start = line_start + sum_len + 0x01;
		attr_start = Tools::find_content(attr_start, line_end, '\r', content_len, sum_len);
		if (attr_start == NULL)
			return -1;
		attr = rstring(attr_start, content_len);

		line_start = line_end;
		//把键值对加入map
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
	//将body拷贝进缓冲
	memcpy(buff, start, body_len);
	//设置终止位置
	buff[body_len] = 0x00;
	//不为空，清除
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
