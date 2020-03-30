#include "HttpRequest.h"

HttpRequest::HttpRequest():HttpRequest(NULL)
{
	
}

HttpRequest::HttpRequest(HttpClient* client):
	m_client(client)
{
	request_str.resize(DEFAULT_BUFF_SIZE);
	response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=gbk\r\n"
		"Connection: close\r\n"
		"\r\n";
}

int HttpRequest::recv_data()
{
	if (m_client == NULL)
		return -1;

	return m_client->recv(&request_str[0],DEFAULT_BUFF_SIZE,0);
}

int HttpRequest::send_data(string res)
{
	if (m_client == NULL)
		return -1;
	
	return m_client->send(res.c_str(),res.length(), 0);
}

string HttpRequest::get_url()
{
	if (request_str.empty())
		return "";
	//取得第一行
	string first_line = request_str.substr(0, request_str.find("\r\n"));
	//取得URL
	first_line = first_line.substr(first_line.find(" ") + 1);//substr，复制函数，参数为起始位置（默认0），复制的字符数目
	string url = first_line.substr(0, first_line.find(" "));//find返回找到的第一个匹配字符串的位置，而不管其后是否还有相匹配的字符串。
	return url;
}

void HttpRequest::url_router(string const& url)
{
	if (m_client == NULL)
		return;
	int client_sock = m_client->get_socket();
	string hint;
	if (url == "/")
	{
		cout << url << " 收到信息" << endl;
		hint = "haha, this is home page!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
	else if (url == "/hello")
	{
		cout << url << " 收到信息" << endl;
		hint = "你好!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
	else
	{
		cout << url << " 收到信息" << endl;
		hint = "未定义URL!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
}

void HttpRequest::handle_request()
{
	recv_data();
	send_data(response);
	url_router(get_url());
}
