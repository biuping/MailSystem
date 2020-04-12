#include "../test.h"
#include "mao.h"

void start_mao() {
	HttpSocket socketInit;
	HttpServer server;

	server.run();
	server.close();
	delete& socketInit;
}