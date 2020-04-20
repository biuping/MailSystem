#include "HttpServerHandler.h"


HttpServerHandler::HttpServerHandler()
{
	m_client = nullptr;
	m_readbuff = nullptr;
	UserInfo* info=new UserInfo(TEST_MAIL_ADDR, TEST_MAIL_PASSWD);
	userStore["1"] = info;
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
	ClearUsers();
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
	rstring* temp_str = new rstring;
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
			temp_str->append(temp_buff, len);
		}
	} while (len > 0);

	delete[] temp_buff;
	//设置末尾0
	m_readbuff = new char[size + 1];
	memcpy(m_readbuff, &(*temp_str)[0], size);
	delete temp_str;
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
	delete[] m_readbuff;
	m_readbuff = nullptr;
}

void HttpServerHandler::set_client(HttpClient* client)
{
	m_client = client;
}

HttpResponse* HttpServerHandler::handle_request(HttpRequest& request)
{

	const rstring& method = request.method();
	const rstring& url = request.url();

	MultipartReader reader;
	MultipartRecord record(reader);

	HttpResponse* response = new HttpResponse();

	if (request.has_head(HTTP_HEAD_CONTENT_TYPE) && method.compare("POST") == 0)
	{
		const rstring& content_type = request.head_content(HTTP_HEAD_CONTENT_TYPE);
		//处理post消息体
		if (content_type.find(HTTP_HEAD_JSON_TYPE) != content_type.npos)
		{

			Tools::json_read(request.body(), request.body_len(), m_object, m_err);

		}
		else if (content_type.find(HTTP_HEAD_FORM_TYPE) != content_type.npos)
		{
			size_t start = content_type.find(HTTP_PREFIX_BOUNDARY);
			rstring boundary = content_type.substr(start + strlen(HTTP_PREFIX_BOUNDARY)); //找到boundary

			reader.setBoundary(boundary);
			reader.feed(request.body(), request.body_len());

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
	else if (url.compare(HTTP_URL_SEND_WITH_ATTACH) == 0)
	{
		SendWithAttach(response, record);
	}
	else if (url.compare(HTTP_URL_RECV_WITH_ATTACH) == 0)
	{
		RecvWithAttach(response);
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

	record.ClearList();

	return response;
}

UserInfo* HttpServerHandler::FindUserByUUID(rstring& uuid)
{
	UserStore::iterator itor = userStore.find(uuid);
	if (itor == userStore.end())
		return nullptr;
	return itor->second;
}

rstring HttpServerHandler::AddUser(rstring& email_addr, rstring& pass)
{
	rstring uuid = Tools::getUUID();
	UserInfo* user = new UserInfo;
	user->email_address = email_addr;
	user->pass = pass;
	userStore[uuid] = user;
	return uuid;
}

void HttpServerHandler::ClearUsers()
{
	UserStore::const_iterator itor;
	for (itor = userStore.begin(); itor != userStore.end(); itor++)
	{
		delete itor->second;
	}
	userStore.clear();
}

void HttpServerHandler::ClearUserByUUID(rstring& uuid)
{
	UserStore::const_iterator itor;
	for (itor = userStore.begin(); itor != userStore.end(); itor++)
	{
		if (itor->first == uuid) 
		{
			delete itor->second;
			userStore.erase(itor);
		}
			
	}
}

void HttpServerHandler::Login(HttpResponse* response)
{
	rstring email = m_object["email_address"].asString();
	rstring pass = m_object["password"].asString();
	rstring description;
	rstring uuid = "";
	bool success = false;

	MailReceiver* receiver = new POP3Client();
	MailClient client;
	client.setReceiver(receiver);
	success = client.Login(email, pass, description);//登录验证

	//成功返回uuid
	if (success)
	{
		uuid = AddUser(email, pass);
	}

	//build返回json
	Json::Value root;
	root["id"] = uuid;
	root["success"] = success;
	root["description"] = description;

	rstring res;
	Tools::json_write(root, res);

	response->build_ok();
	response->build_body(res);
	response->add_head(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_JSON_TYPE);
}

void HttpServerHandler::SendWithAttach(HttpResponse* response, MultipartRecord& record)
{
	Attachment attach_ptr;
	std::vector<Attachment> attachs;
	rstring id_str;
	rstring email_address;
	rstring password;
	rstring recver_str;
	rstring theme_str;
	rstring content_str;
	MailClient client;
	MailSender* sender = new SMTPClient();


	//检查id
	HttpHead_t* id = record.FindHeaderByName("id");
	if (id != nullptr)
	{
		id_str = record.FindHeadContent(*id, HTTP_FORM_CONTENT);
		if (!AuthLogin(response, client, sender, nullptr, id_str))
		{
			return;
		}
	}

	//检查附件
	HttpHead_t* attach = record.FindHeaderByName("attachment");
	if (attach != nullptr)
	{
		attach_ptr.file_name = record.FindHeadContent(*attach, HTTP_FORM_FILENAME);
		attach_ptr.content_type = record.FindHeadContent(*attach, HTTP_FORM_CONTENT_TYPE);
		attach_ptr.content = record.FindHeadContent(*attach, HTTP_FORM_CONTENT);
		attachs.push_back(attach_ptr);
	}
	HttpHead_t* theme = record.FindHeaderByName("theme");
	if (theme != nullptr)
	{
		theme_str = record.FindHeadContent(*theme, HTTP_FORM_CONTENT);
	}
	HttpHead_t* recver = record.FindHeaderByName("recver");
	if (recver != nullptr)
	{
		recver_str = record.FindHeadContent(*recver, HTTP_FORM_CONTENT);
	}
	HttpHead_t* content = record.FindHeaderByName("content");
	if (content != nullptr)
	{
		content_str = record.FindHeadContent(*content, HTTP_FORM_CONTENT);
	}
	const rstring& res = client.SendMail(recver_str, theme_str, content_str, attachs);
	response->build_ok();
	response->build_body(res);
	response->add_head(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_JSON_TYPE);
}

void HttpServerHandler::SendNoAttach(HttpResponse* response)
{
}

void HttpServerHandler::RecvWithAttach(HttpResponse* response)
{
	MailClient client;
	MailReceiver* receiver = new POP3Client();
	rstring id = m_object["id"].asString();
	if (!AuthLogin(response, client, nullptr, receiver, id))
	{
		return;
	}
	const rstring& res = client.RecvMail();
	response->build_ok();
	response->build_body(res);
	response->add_head(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_JSON_TYPE);
}

void HttpServerHandler::RecvNoAttach(HttpResponse* response)
{
}

void HttpServerHandler::DownloadAttach(HttpResponse* response)
{
	MailClient client;
	MailReceiver* receiver = new POP3Client();
	rstring id = m_object["id"].asString();
	if (!AuthLogin(response, client, nullptr, receiver, id))
	{
		return;
	}
	const rstring& mailId = m_object["mailId"].asString();
	const int attachIndex = m_object["attach_index"].asInt();
	
	const Json::Value& root = client.DownloadAttach(mailId, attachIndex);
	const rstring& filename = root["filename"].asString();
	const rstring& content_type = root["content-type"].asString();
	const rstring& content = root["content"].asString();

	response->add_head(HTTP_HEAD_CONTENT_TYPE, content_type+"; name="+filename);
	response->build_ok();
	response->add_head("Content-Disposition", "attachment; filename="+filename);
	response->build_body(content);
}

void HttpServerHandler::DeleteMail(HttpResponse* response)
{
	MailClient client;
	MailReceiver* receiver = new POP3Client();
	rstring id = m_object["id"].asString();
	if (!AuthLogin(response, client, nullptr, receiver, id))
	{
		return;
	}
	Json::Value mail_ids= m_object["mail_id"];
	std::list<rstring> id_list;
	for (int i = 0; i < mail_ids.size(); i++)
	{
		id_list.push_back(mail_ids[i].asString());
	}
	const rstring& res = client.DeleteMail(id_list);
	response->build_ok();
	response->build_body(res);
	response->add_head(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_JSON_TYPE);
}

UserInfo* HttpServerHandler::AuthUserById(HttpResponse* response, rstring& uuid)
{
	UserInfo* info = FindUserByUUID(uuid);
	if (info == nullptr)
	{
		Json::Value root;
		root["success"] = false;
		root["description"] = "Please Login";

		rstring res;
		Tools::json_write(root, res);

		response->set_status("403", "NO AUTHENTICATION");
		response->set_common();
		response->build_body(res);
		response->add_head(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_JSON_TYPE);
	}
	return info;

}

bool HttpServerHandler::AuthLogin(HttpResponse* response, MailClient& client, MailSender* sender, MailReceiver* receiver, rstring& uuid)
{
	UserInfo* info;
	rstring description;

	//查看是否登录
	info = AuthUserById(response, uuid);
	if (info == nullptr)
		return false;


	//登录验证
	client.setSender(sender);
	client.setReceiver(receiver);
	bool success = false;
	success = client.Login(info->email_address, info->pass, description);

	//验证不成功
	if (!success)
	{
		Json::Value root;
		root["id"] = uuid;
		root["success"] = success;
		root["description"] = description;

		rstring res;
		Tools::json_write(root, res);

		response->build_ok();
		response->build_body(res);
		response->add_head(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_JSON_TYPE);
		return false;
	}
	return true;
}


