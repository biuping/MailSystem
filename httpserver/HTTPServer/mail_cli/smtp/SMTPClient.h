#ifndef __SMTP_H__ //�����ظ�����
#define __SMTP_H__
#include <list>
#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "../MailSender.h"
using namespace std;
const int MAXLEN = 1024;
const int MAX_FILE_LEN = 6000;
static const char base64Char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
struct FILEINFO /*������¼�ļ���һЩ��Ϣ*/
{
    char fileName[128]; /*�ļ�����*/
    char filePath[512]; /*�ļ�����·��*/
};
class SMTPClient:public MailSender
{
public:
    SMTPClient(void);
    SMTPClient(
        int port,
        std::string srvDomain,  //SMTPClient����������
        std::string userName,   //�û���
        std::string password,   //����
        std::string targetEmail, //Ŀ���ʼ���ַ
        std::string emailTitle,  //����
        std::string content       //����
    );
public:
    ~SMTPClient(void);
public:
    int port;
public:
    std::string domain;
    std::string user;
    std::string pass;
    std::string targetAddr;
    std::string title;
    std::string content;

    std::vector <Attachment> Attachments;
public:
    char buff[MAXLEN + 1];
    int buffLen;
    SOCKET sockClient;  //�ͻ��˵��׽���
public:
    bool CreateConn(); /*��������*/
    bool Send(std::string& message);
    bool Recv();
    void FormatEmailHead(std::string& email);//��ʽ��Ҫ���͵��ʼ�ͷ��
    int Login();
    bool Login(string userCount, string userPass, string& description);
    bool SendEmailHead();       //�����ʼ�ͷ����Ϣ
    bool SendTextBody();        //�����ı���Ϣ

    int SendAttachment_Ex();
    bool SendEnd();
public:
    void AddAttachment(Attachment attachs); //��Ӹ���
    void DeleteAttachment(int index); //ɾ������
    void DeleteAllAttachment(); //ɾ�����еĸ���
     /*�������˵��:1.��������µĴ���2.�û�������3.�������4.�ļ�������0.�ɹ�*/
    int SendEmail_Ex();
    void SetSrvDomain(std::string& domain);
    void SetUserName(std::string& user);
    void SetPass(std::string& pass);
    void SetTargetEmail(const std::string& targetAddr);
    void SetEmailTitle(const std::string& title);
    void SetContent(const std::string& content);
    void SetPort(int port);
 
   
    char* base64Encode(char const* origSigned, unsigned origLength);
};
#endif // !__SMTP_H__
