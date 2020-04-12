#include "mao.h"

void start() {
	HttpSocket socketInit;
	HttpServer server;

	server.run();
	server.close();
	delete& socketInit;
}