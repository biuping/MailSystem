#include "HttpServerHandler.h"

HttpServerHandler::HttpServerHandler(HttpClient* client):
	m_client(client)
{
	m_readbuff = new char[DEFAULT_BUFF_SIZE+1];
}

HttpServerHandler::~HttpServerHandler()
{
	delete m_client;
	m_client = nullptr;

	delete m_readbuff;
	m_readbuff = nullptr;
}

void HttpServerHandler::handle_client()
{
	int nread = m_client->recv(m_readbuff, DEFAULT_BUFF_SIZE, 0);
	m_readbuff[nread] = 0x00;

	HttpRequest request;
	if (request.load_packet(m_readbuff, nread) < 0)
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

	HttpResponse* respose = new HttpResponse();

	rstring write_res;
	Json::Value root;
	root["null"] = NULL;			//注意此处在输出是显示为0，而非null
	root["message"] = "OK";
	root["age"] = 52;
	root["array"].append("arr");	// 新建一个key为array，类型为数组，对第一个元素赋值为字符串“arr”
	root["array"].append(123);		// 为数组 key_array 赋值，对第二个元素赋值为：123
	Tools::json_write(root, write_res);

	respose->set_version(HTTP_VERSION);
	respose->set_status("200", "OK");
	respose->add_head(HTTP_HEAD_CONTENT_LEN, std::to_string(write_res.size()));
	respose->add_head(HTTP_HEAD_CONNECTION, "close");
	respose->set_body(&write_res[0], write_res.size());
	
	return respose;
}
