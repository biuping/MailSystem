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
	//ȡ�õ�һ��
	string first_line = request_str.substr(0, request_str.find("\r\n"));
	//ȡ��URL
	first_line = first_line.substr(first_line.find(" ") + 1);//substr�����ƺ���������Ϊ��ʼλ�ã�Ĭ��0�������Ƶ��ַ���Ŀ
	string url = first_line.substr(0, first_line.find(" "));//find�����ҵ��ĵ�һ��ƥ���ַ�����λ�ã�����������Ƿ�����ƥ����ַ�����
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
		cout << url << " �յ���Ϣ" << endl;
		hint = "haha, this is home page!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
	else if (url == "/hello")
	{
		cout << url << " �յ���Ϣ" << endl;
		hint = "���!";
		send(client_sock, hint.c_str(), hint.length(), 0);
	}
	else
	{
		cout << url << " �յ���Ϣ" << endl;
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
