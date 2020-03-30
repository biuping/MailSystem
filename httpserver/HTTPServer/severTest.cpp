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
	{  //�����׽��ְ汾��Ϣ2.0��WSADATA������ַ
		WSAStartup(MAKEWORD(2, 0), &_wsa);

	}
	~WinSockInit()
	{
		WSACleanup();//��������ֹWinsock 2 DLL (Ws2_32.dll) ��ʹ��
	}
};

//����URL
void UrlRouter(int clientSock, string const& url)
{
	string hint;
	if (url == "/")
	{
		cout << url << " �յ���Ϣ" << endl;
		hint = "haha, this is home page!";
		send(clientSock, hint.c_str(), hint.length(), 0);
	}
	else if (url == "/hello")
	{
		cout << url << " �յ���Ϣ" << endl;
		hint = "���!";
		send(clientSock, hint.c_str(), hint.length(), 0);
	}
	else
	{
		cout << url << " �յ���Ϣ" << endl;
		hint = "δ����URL!";
		send(clientSock, hint.c_str(), hint.length(), 0);
	}

}

int main2()
{
	WinSockInit socklibInit;//���ΪWindowsϵͳ������WSAStartup

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽��֣�ʧ�ܷ���-1
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET; //ָ����ַ��
	addr.sin_addr.s_addr = INADDR_ANY;//IP��ʼ��
	addr.sin_port = htons(PORT_NUM);//�˿ںų�ʼ��

	int rc;
	rc = bind(sock, (sockaddr*)&addr, sizeof(addr));//����IP�Ͷ˿�

	rc = listen(sock, 0);//���ü���

	//���ÿͻ���
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	int clientSock;
	//���ܿͻ�������
	while (1)
	{
		clientSock = accept(sock, (sockaddr*)&clientAddr, (socklen_t*)&clientAddrSize);
		if (INVALID_SOCKET == clientSock)
			continue;
		// ������
		string requestStr;
		int bufSize = 4096;
		requestStr.resize(bufSize);
		//��������
		recv(clientSock, &requestStr[0], bufSize, 0);

		//ȡ�õ�һ��
		string firstLine = requestStr.substr(0, requestStr.find("\r\n"));
		//ȡ��URL
		firstLine = firstLine.substr(firstLine.find(" ") + 1);//substr�����ƺ���������Ϊ��ʼλ�ã�Ĭ��0�������Ƶ��ַ���Ŀ
		string url = firstLine.substr(0, firstLine.find(" "));//find�����ҵ��ĵ�һ��ƥ���ַ�����λ�ã�����������Ƿ�����ƥ����ַ�����

		//������Ӧͷ
		string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html; charset=gbk\r\n"
			"Connection: close\r\n"
			"\r\n";
		send(clientSock, response.c_str(), response.length(), 0);
		//����URL
		UrlRouter(clientSock, url);

		closesocket(clientSock);//�رտͻ����׽���
	}

	closesocket(sock);//�رշ������׽���

	return 0;
}
