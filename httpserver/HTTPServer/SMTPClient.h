#include "socket/TCPClientSocket.h"
#include"mail_cli/MailSender.h"
#include <string>
#define MAX_CONNECTION_NUM 5
#define SMTP_SERVER_PORT 25
#define BUFFER_SIZE 4096




class SMTPClient :MailSender
{
public:
	std::string sentEmail();
	int AnalysisData();
private:
	bool FormatEmail(char* pFrom, char* pTo, char* pSubject, char* pMessage, char* Email);
	bool SendAndRecvMsg(TCPClientSocket socketClient, char* pMessage ,int Messagelen, int Dowhat, char* recvBuf, int recvBuflen);
	void StringToBase64(const char* src, char* dst);
	int GetStrLength(char* pString);
};
