#include "HttpClient.h"
#include "HttpServer.h"
#include "HttpRequest.h"

void start1() {
	
	HttpServer server;
	if (server.start() < 0) {
		std::cout << "start failed" << std::endl;
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
		std::cout << "start failed" << std::endl;
	}
	else {
		std::cout << "start success" << std::endl;
	}

	HttpClient* client;
	while (NULL != (client = server.accept()))
	{
		HttpRequest request(client);
		request.handle_request();
		client->close();
	}
	server.close();
}
int main(int argc, char* argv[])
{
	std::cout << "sdf" << std::endl;
	HttpSocket socketInit;
	//main2();
	start2();
	delete &socketInit;
	return 0;
}