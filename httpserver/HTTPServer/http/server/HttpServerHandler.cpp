#include "HttpServerHandler.h"

HttpServerHandler::HttpServerHandler(HttpClient* client) :
	m_client(client)
{
	m_readbuff = nullptr;
}

HttpServerHandler::~HttpServerHandler()
{
	delete m_client;
	m_client = nullptr;

	if (m_readbuff != nullptr)
	{
		delete[] m_readbuff;
		m_readbuff = nullptr;
	}
	
}

void HttpServerHandler::handle_client()
{
	size_t size = 0;
	size_t len = 0;
	char* temp_buff = new char[DEFAULT_BUFF_SIZE];
	rstring temp_str;
	do
	{
		len = m_client->recv(temp_buff, DEFAULT_BUFF_SIZE, 0);
		if (len == 0)
		{
			Tools::report("connection closed");
			break;
		}
		else if (len == SOCKET_ERROR)
		{	//wouldblock˵����Դ��ʱ�����ã��Ѷ�ȡ���
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				Tools::report("ERROR: receive error, ", WSAGetLastError());
				return;
			}
			break;
		}
		else
		{
			size += len;
			temp_str.append(temp_buff, len);
		}
	} while (len > 0);

	//����ĩβ0
	m_readbuff = new char[size + 1];
	m_readbuff = &temp_str[0];
	m_readbuff[size] = 0x00;

	HttpRequest request;
	if (request.load_packet(m_readbuff, size) < 0)
	{
		Tools::report("Parse package error");
		return;
	}

	HttpResponse* response = handle_request(request);
	if (nullptr != response)
	{
		const char* buff = response->serialize();
		size_t len = strlen(buff);
		m_client->send(buff, len, 0);
		delete response;
	}
}

HttpResponse* HttpServerHandler::handle_request(HttpRequest& request)
{
	const rstring& method = request.method();
	const rstring& url = request.url();

	HttpResponse* response = new HttpResponse();

	rstring write_res;
	Json::Value root;
	root["null"] = NULL;			//ע��˴����������ʾΪ0������null
	root["message"] = "OK";
	root["age"] = 52;
	root["array"].append("arr");	// �½�һ��keyΪarray������Ϊ���飬�Ե�һ��Ԫ�ظ�ֵΪ�ַ�����arr��
	root["array"].append(123);		// Ϊ���� key_array ��ֵ���Եڶ���Ԫ�ظ�ֵΪ��123
	Tools::json_write(root, write_res);

	response->build_ok();
	response->build_body(write_res);

	return response;
}





