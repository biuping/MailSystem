#include "HttpClient.h"
#include "HttpServer.h"
#include "HttpRequest.h"
#include <iostream>
#include "severTest.h"

void start1() {
	
	HttpServer server;
	if (server.start() < 0) {
		std::cout << "start failed" << endl;
	}
	int server_sock = server.get_socket();
	HttpClient* client;
	int client_sock;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	while (INVALID_SOCKET != (client_sock = ::accept(server_sock, (sockaddr*)&client_addr, (socklen_t*)&client_addr_size)))
	{
		client = new HttpClient(client_sock);
		HttpRequest request(client);
		request.handle_request();
		client->close();


	}
	server.close();
}

void start2() {
	HttpServer server;
	if (server.start() < 0) {
		std::cout << "start failed" << endl;
	}
	else {
		std::cout << "start success" << endl;
	}

	HttpClient* client;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	while (NULL != (client = server.accept()))
	{
		HttpRequest request(client);
		request.handle_request();
		client->close();


	}
	server.close();
}
int main()
{
	HttpSocket socketInit;
	//main2();
	start2();
	delete &socketInit;
	system("pause");
	return 0;
}