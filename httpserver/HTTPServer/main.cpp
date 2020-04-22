#include ".\tests\test.h"
#include "http\server\HttpServer.h"


int main(int argc, char* argv[])
{
#ifdef _DEBUG
#ifdef TEST_START
	// 调试环境下运行开发测试
	TEST_START();
#else
	// 调试环境下运行使用测试
	HttpSocket socketInit;
	HttpServer server;

	server.run();
	server.close();
#endif // TEST_START
#else
	// 正常运行
	HttpSocket socketInit;
	HttpServer server;

	server.run();
	server.close();
#endif // _DEBUG

}