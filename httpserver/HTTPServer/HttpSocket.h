#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#pragma comment(lib,"WS2_32")

class HttpSocket 
{
private:
	WSADATA _wsa;
public:
	HttpSocket();

	~HttpSocket();
};