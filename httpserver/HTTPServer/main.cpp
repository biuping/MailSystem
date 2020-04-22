#include ".\tests\test.h"
#include "http\server\HttpServer.h"


int main(int argc, char* argv[])
{
#ifdef _DEBUG
#ifdef TEST_START
	// ���Ի��������п�������
	TEST_START();
#else
	// ���Ի���������ʹ�ò���
	HttpSocket socketInit;
	HttpServer server;

	server.run();
	server.close();
#endif // TEST_START
#else
	// ��������
	HttpSocket socketInit;
	HttpServer server;

	server.run();
	server.close();
#endif // _DEBUG

}