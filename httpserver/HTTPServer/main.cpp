#include "HttpClient.h"
#include "HttpServer.h"
#include "HttpRequest.h"

int main(int argc, char* argv[])
{
	HttpSocket socketInit;
	HttpServer server;
	
	server.run();
	server.close();
	delete &socketInit;
	return 0;
}