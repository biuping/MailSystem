#include "HttpServerHandler.h"


HttpServerHandler::HttpServerHandler()
{
	m_client = nullptr;
	m_readbuff = nullptr;
}

HttpServerHandler::HttpServerHandler(HttpClient* client) :
	m_client(client)
{
	m_readbuff = nullptr;
}

HttpServerHandler::~HttpServerHandler()
{

	if (m_readbuff != nullptr)
	{
		delete[] m_readbuff;
		m_readbuff = nullptr;
	}
	//if (m_object != nullptr)
	//{
	//	delete m_object;
	//	m_object = nullptr;
	//}
	//if (m_err != nullptr)
	//{
	//	delete m_err;
	//	m_err = nullptr;
	//}

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
			Tools::report("Connection Closed");
			break;
		}
		else if (len == SOCKET_ERROR)
		{	//wouldblock说明资源暂时不可用，已读取完毕
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

	delete[] temp_buff;
	//设置末尾0
	m_readbuff = new char[size + 1];
	memcpy(m_readbuff, &temp_str[0], size);
	m_readbuff[size] = 0x00;

	//解析请求
	HttpRequest request;
	if (request.load_packet(m_readbuff, size) < 0)
	{
		Tools::report("Parse package error");
		return;
	}

	//生成响应
	HttpResponse* response = handle_request(request);
	if (nullptr != response)
	{
		const char* buff = response->serialize();
		size_t len = strlen(buff);
		m_client->send(buff, len, 0);
		delete response;

	}
}

void HttpServerHandler::set_client(HttpClient* client)
{
	m_client = client;
}

HttpResponse* HttpServerHandler::handle_request(HttpRequest& request)
{

	const rstring& method = request.method();
	const rstring& url = request.url();
	const rstring& content_type = request.head_content(HTTP_HEAD_CONTENT_TYPE);

	HttpResponse* response = new HttpResponse();

	if (!content_type.empty() && method.compare("POST") == 0)
	{

		//处理post消息体
		if (content_type.find(HTTP_HEAD_JSON_TYPE) != content_type.npos)
		{

			Tools::json_read(request.body(), request.body_len(), m_object, m_err);

		}
		else if (content_type.find(HTTP_HEAD_FORM_TYPE) != content_type.npos)
		{
			//TODO： 处理form-data
			Tools::report(HTTP_HEAD_FORM_TYPE);
		}
		else
		{
			response->set_common();
			response->set_status("400", "Unknown content type");
		}
	}

	//处理url
	if (url.compare(HTTP_URL_LOGIN) == 0)
	{
		Login(response);
		
	}
	else if (url.compare(HTTP_URL_SEND_NO_ATTACH) == 0)
	{
		SendNoAttach(response);
	}
	else if (url.compare(HTTP_URL_RECV_NO_ATTACH) == 0)
	{
		RecvNoAttach(response);
	}
	else if (url.compare(HTTP_URL_DELETE_MAIL) == 0)
	{
		DeleteMail(response);
	}
	else if (url.compare(HTTP_URL_DOWNLOAD_ATTACH) == 0)
	{
		DownloadAttach(response);
	}
	else
	{
		response->build_not_found();
	}


	return response;
}

void HttpServerHandler::Login(HttpResponse* response)
{
	response->build_ok();
	response->add_head(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_JSON_TYPE);
}

void HttpServerHandler::SendWithAttach(HttpResponse* response)
{
}

void HttpServerHandler::SendNoAttach(HttpResponse* response)
{
}

void HttpServerHandler::RecvWitnAttach(HttpResponse* response)
{
}

void HttpServerHandler::RecvNoAttach(HttpResponse* response)
{
}

void HttpServerHandler::DownloadAttach(HttpResponse* response)
{
	rstring down = "dGhpcyBpcyB0ZXN0Mg0KDQoNCi0tLS0tLS0tLS0tLQ0KLS0NCi0tLS0tLS0tLS0tLQ==";
	response->add_head(HTTP_HEAD_CONTENT_TYPE, "text / plain; name = \"test2.txt\"");
	response->build_ok();
	response->add_head("Content-Disposition", "attachment; filename=\"test2.txt\"");
	response->add_head("Content-Transfer-Encoding", "base64");
	response->build_body(down);
}

void HttpServerHandler::DeleteMail(HttpResponse* response)
{
}

