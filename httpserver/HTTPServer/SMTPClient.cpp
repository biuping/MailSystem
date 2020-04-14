#include "SMTPClient.h"
#include<iostream>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib")


//获得字符串的长度
int SMTPClient::GetStrLength(char* pString)
{
	int i = 0;
	while (pString[i++] != 0);
	return i - 1;
}
//格式化要发送的内容
bool SMTPClient::FormatEmail(char* pFrom, char* pTo, char* pSubject, char* pMessage, char* Email)
{
	lstrcat(Email, "From: ");
	lstrcat(Email, pFrom);
	lstrcat(Email, "\r\n");

	lstrcat(Email, "To: ");
	lstrcat(Email, pTo);
	lstrcat(Email, "\r\n");

	lstrcat(Email, "Subject: ");
	lstrcat(Email, pSubject);
	lstrcat(Email, "\r\n");

	lstrcat(Email, "MIME-Version:1.0");
	lstrcat(Email, "\r\n");
	lstrcat(Email, "\r\n");

	lstrcat(Email, pMessage);

	lstrcat(Email, "\r\n.\r\n");

	return true;

}
//发送并接收来自smtp服务器的信息
bool SMTPClient::SendAndRecvMsg(TCPClientSocket socketClient, char* pMessage, int Messagelen, int Dowhat, char* recvBuf, int recvBuflen)
{
	char lpMessage[256] = { 0 };
	memcpy(lpMessage, pMessage, Messagelen);
	printf("\n\n%s \n", lpMessage);
	if (Dowhat == 0)
	{
		socketClient.write(lpMessage, Messagelen, 0);
		memset(recvBuf, 0, recvBuflen);
		DWORD num = socketClient.readblock(recvBuf, recvBuflen, 0);
		if (num == -1)
		{
			cout << "读取失败";
			return false;
		}
		printf("%s \n", recvBuf);
		int i = 0;
		while (i!=num)
		{
			printf("%02X ", recvBuf[i++]);
			if ((i) % 16 == 0)
			{
				printf("\n");
			}
		}
		printf("\n");
	}
	//只发送
	else if (Dowhat ==1)
	{
		socketClient.write(lpMessage, Messagelen, 0);
	}
	//只接收
	else if (Dowhat == 2)
	{
		memset(recvBuf, 0, recvBuflen);
		DWORD num = socketClient.readblock(recvBuf, recvBuflen, 0);
		if (num == -1)
		{
			cout << "读取失败";
			return false;
		}
		printf("%s \n", recvBuf);
		int i = 0;
		while (i != num)
		{
			printf("%02X ", recvBuf[i++]);
			if ((i) % 16 == 0)
			{
				printf("\n");
			}
		}
		printf("\n");
	}
}

void SMTPClient::StringToBase64(const char* src, char* dst)
{/*将字符串变为base64编码*/
	int i = 0;
	char* p = dst;
	int d = strlen(src) - 3;
	static const char Base64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//for(i=0;i<strlen(src)-3;i+=3) ;if (strlen(src)-3)<0 there is a buf

	for (i = 0; i <= d; i += 3)
	{
		*p++ = Base64[((*(src + i)) >> 2) & 0x3f];
		*p++ = Base64[(((*(src + i)) & 0x3) << 4) + ((*(src + i + 1)) >> 4)];
		*p++ = Base64[((*(src + i + 1) & 0xf) << 2) + ((*(src + i + 2)) >> 6)];
		*p++ = Base64[(*(src + i + 2)) & 0x3f];
	}
	if ((strlen(src) - i) == 1)
	{
		*p++ = Base64[((*(src + i)) >> 2) & 0x3f];
		*p++ = Base64[((*(src + i)) & 0x3) << 4];
		*p++ = '=';
		*p++ = '=';
	}
	if ((strlen(src) - i) == 2)
	{
		*p++ = Base64[((*(src + i)) >> 2) & 0x3f];
		*p++ = Base64[(((*(src + i)) & 0x3) << 4) + ((*(src + i + 1)) >> 4)];
		*p++ = Base64[((*(src + i + 1) & 0xf) << 2)];
		*p++ = '=';
	}
	*p = '\0';
}

string SMTPClient::sentEmail()
{
	char srvDomain[256] = "smtp.whu.edu.cn";
	char userName[256] = "2017302580306@whu.edu.cn";
	char password[256] = "wdrs14569";
	char targetEmail[256] = "1092949763@qq.com";
	char emailTitle[256] = "hello";
	char content[256] = "这是一封测试邮件，很高兴我的第一个客户端制作成功！";

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	//武大邮箱
	TCPClientSocket socketClient = TCPClientSocket(srvDomain, SMTP_SERVER_PORT);
	char buff[BUFFER_SIZE];
	memset(buff, 0, sizeof(char) * BUFFER_SIZE);//缓存清零

	SendAndRecvMsg(socketClient, 0, 0, 2, buff, BUFFER_SIZE);

	char UserNameToSendEmail[256] = { 0 };
	printf(UserNameToSendEmail, "EHLO %s", "it is just a test");
	lstrcat(UserNameToSendEmail, "\r\n\0");

	SendAndRecvMsg(socketClient, UserNameToSendEmail,SMTPClient::GetStrLength(UserNameToSendEmail), 0, buff, BUFFER_SIZE);
	char pUerName[256] = { 0 };
	//调用strstr函数，搜索一个字符串在另外一个字符串中的第一次出现，并返回第一次出现位置的指针
	DWORD p = strstr(userName, "@") - userName;
	memcpy(pUerName, userName, p); //得到用户名,如从"13203200199@163.com"得到"13203200199"
	char base[256];
	StringToBase64(pUerName, base); //得到用户名的base64编码

	char str[BUFFER_SIZE];
	memset(str, 0, BUFFER_SIZE);
	sprintf(str, "%s\r\n", base/*"MTMyMDMyMDAxOTk="*/);
	SendAndRecvMsg(socketClient, str, lstrlen(str), 0, buff, BUFFER_SIZE); //发送用户名，并接收服务器的返回

	StringToBase64(password, base);
	memset(str, 0, 1024);
	sprintf(str, "%s\r\n", base);
	SendAndRecvMsg(socketClient, str, lstrlen(str), 0, buff, BUFFER_SIZE); //发送用户密码，并接收服务器的返回

	char MailFrom[256] = { 0 };
	sprintf(MailFrom, "MAIL FROM: <%s>\r\n", userName);

	SendAndRecvMsg(socketClient, MailFrom, lstrlen(MailFrom), 0, buff, BUFFER_SIZE);

	char RcptTo[256] = { 0 };
	sprintf(RcptTo, "RCPT TO: <%s>\r\n", targetEmail);
	SendAndRecvMsg(socketClient, RcptTo, lstrlen(RcptTo), 0, buff, BUFFER_SIZE);
	char Data[7] = "DATA\r\n";
	SendAndRecvMsg(socketClient, Data, lstrlen("DATA\r\n"), 0, buff, BUFFER_SIZE);

	char Email[1024] = { 0 };
	FormatEmail(userName, targetEmail, emailTitle, content, Email);

	SendAndRecvMsg(socketClient, Email, lstrlen(Email), 0, buff, BUFFER_SIZE);
	char Quit[7] = "QUIT\r\n";
	SendAndRecvMsg(socketClient, Quit, lstrlen("QUIT\r\n"), 0, buff, BUFFER_SIZE);

	socketClient.closeSocket();
	WSACleanup();

	
	return "";
}
