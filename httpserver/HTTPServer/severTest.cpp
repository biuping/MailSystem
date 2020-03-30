#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#pragma comment(lib,"WS2_32")
#include "severTest.h"
#include "config.h"
using namespace std;

class WinSockInit
{
	WSADATA _wsa;
public:
	WinSockInit()
	{  //分配套接字版本信息2.0，WSADATA变量地址
		WSAStartup(MAKEWORD(2, 0), &_wsa);

	}
	~WinSockInit()
	{
		WSACleanup();//功能是终止Winsock 2 DLL (Ws2_32.dll) 的使用
	}
};

//处理URL
void UrlRouter(int clientSock, string const& url)
{
	string hint;
	if (url == "/")
	{
		cout << url << " 收到信息" << endl;
		hint = "haha, this is home page!";
		send(clientSock, hint.c_str(), hint.length(), 0);
	}
	else if (url == "/hello")
	{
		cout << url << " 收到信息" << endl;
		hint = "你好!";
		send(clientSock, hint.c_str(), hint.length(), 0);
	}
	else
	{
		cout << url << " 收到信息" << endl;
		hint = "未定义URL!";
		send(clientSock, hint.c_str(), hint.length(), 0);
	}

}

int main2()
{
	WinSockInit socklibInit;//如果为Windows系统，进行WSAStartup

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//建立套接字，失败返回-1
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET; //指定地址族
	addr.sin_addr.s_addr = INADDR_ANY;//IP初始化
	addr.sin_port = htons(PORT_NUM);//端口号初始化

	int rc;
	rc = bind(sock, (sockaddr*)&addr, sizeof(addr));//分配IP和端口

	rc = listen(sock, 0);//设置监听

	//设置客户端
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	int clientSock;
	//接受客户端请求
	while (1)
	{
		clientSock = accept(sock, (sockaddr*)&clientAddr, (socklen_t*)&clientAddrSize);
		if (INVALID_SOCKET == clientSock)
			continue;
		// 收请求
		string requestStr;
		int bufSize = 4096;
		requestStr.resize(bufSize);
		//接受数据
		recv(clientSock, &requestStr[0], bufSize, 0);

		//取得第一行
		string firstLine = requestStr.substr(0, requestStr.find("\r\n"));
		//取得URL
		firstLine = firstLine.substr(firstLine.find(" ") + 1);//substr，复制函数，参数为起始位置（默认0），复制的字符数目
		string url = firstLine.substr(0, firstLine.find(" "));//find返回找到的第一个匹配字符串的位置，而不管其后是否还有相匹配的字符串。

		//发送响应头
		string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html; charset=gbk\r\n"
			"Connection: close\r\n"
			"\r\n";
		send(clientSock, response.c_str(), response.length(), 0);
		//处理URL
		UrlRouter(clientSock, url);

		closesocket(clientSock);//关闭客户端套接字
	}

	closesocket(sock);//关闭服务器套接字

	return 0;
}
