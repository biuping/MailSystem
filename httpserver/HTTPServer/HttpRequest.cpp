#include "HttpRequest.h"

HttpRequest::HttpRequest() :HttpRequest(nullptr)
{

}

HttpRequest::HttpRequest(HttpClient* client) :
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
	if (m_client == nullptr)
		return -1;
	
	return m_client->recv(&request_str[0], DEFAULT_BUFF_SIZE, 0);
}

int HttpRequest::send_data(rstring res)
{
	if (m_client == nullptr)
		return -1;

	return m_client->send(res.c_str(), res.length(), 0);
}

rstring HttpRequest::get_url()
{
	if (request_str.empty())
		return "";
	//ȡ�õ�һ��
	rstring first_line = request_str.substr(0, request_str.find("\r\n"));
	//ȡ��URL
	first_line = first_line.substr(first_line.find(" ") + 1);//substr�����ƺ���������Ϊ��ʼλ�ã�Ĭ��0�������Ƶ��ַ���Ŀ
	rstring url = first_line.substr(0, first_line.find(" "));//find�����ҵ��ĵ�һ��ƥ���ַ�����λ�ã�����������Ƿ�����ƥ����ַ�����
	return url;
}

void HttpRequest::url_router(rstring const& url)
{
	if (m_client == nullptr)
		return;
	SOCKET client_sock = m_client->get_socket();
	rstring hint;
	if (url == "/")
	{
		Tools::report(url + " �յ���Ϣ");
		hint = "haha, this is home page!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
	else if (url == "/hello")
	{
		Tools::report(url + " �յ���Ϣ");
		hint = "���!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
	else
	{
		Tools::report(url + " �յ���Ϣ");
		hint = "δ����URL!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
}

void HttpRequest::handle_request()
{
	recv_data();
	send_data(response);
	url_router(get_url());
}
