#pragma once
#include <string>
#include "config.h"
#include "HttpClient.h"
using namespace std;

class HttpRequest 
{
private:
	HttpClient* m_client;
public:

	std::string request_str;
	std::string response;
	HttpRequest();
	HttpRequest(HttpClient* client);
	int recv_data();
	int send_data(std::string res);
	std::string get_url();
	void url_router(string const& url);

	void handle_request();
};