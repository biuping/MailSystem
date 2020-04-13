#include "../test.h"
#include "mao.h"
void start_mao() {

	//json_test();
	server_start();
}
void server_start() {
	HttpSocket socketInit;
	HttpServer server;

	server.run();
	server.close();
	delete& socketInit;
}

void json_test()
{
	const char* json_str = "{\n\t\"name\":\"13\",\n\t\"type\":\"pop\"\n}";

	Json::Value object;
	Json::String err;
	Tools::json_read(json_str, strlen(json_str), object, err);
	std::cout << object["type"].asString() << std::endl;

	rstring write_res;
	Json::Value root;
	root["null"] = NULL;			//ע��˴����������ʾΪ0������null
	root["message"] = "OK";
	root["age"] = 52;
	root["array"].append("arr");	// �½�һ��keyΪarray������Ϊ���飬�Ե�һ��Ԫ�ظ�ֵΪ�ַ�����arr��
	root["array"].append(123);		// Ϊ���� key_array ��ֵ���Եڶ���Ԫ�ظ�ֵΪ��123

	Tools::json_write(root, write_res);
	std::cout << write_res << std::endl;
}