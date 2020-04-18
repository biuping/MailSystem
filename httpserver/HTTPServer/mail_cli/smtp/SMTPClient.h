#ifndef __SMTP_H__ //�����ظ�����
#define __SMTP_H__
#include <list>
#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../MailSender.h"

const int MAXLEN = 1024;
const int MAX_FILE_LEN = 6000;
static const char base64Char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
struct FILEINFO /*������¼�ļ���һЩ��Ϣ*/
{
    char fileName[128]; /*�ļ�����*/
    char filePath[512]; /*�ļ�����·��*/
};
class SMTP:MailSender
{
public:
    SMTP(void);
    SMTP(
        int port,
        std::string srvDomain,  //smtp����������
        std::string userName,   //�û���
        std::string password,   //����
        std::string targetEmail, //Ŀ���ʼ���ַ
        std::string emailTitle,  //����
        std::string content       //����
    );
public:
    ~SMTP(void);
public:
    int port;
public:
    std::string domain;
    std::string user;
    std::string pass;
    std::string targetAddr;
    std::string title;
    std::string content;
    /*Ϊ�˷�������ļ���ɾ���ļ�����ģ�ʹ��list������Ϊ���㣬���Ŵ�������ݽṹ���涼ѧ��*/
    std::list <FILEINFO*> listFile;
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
    bool SendEmailHead();       //�����ʼ�ͷ����Ϣ
    bool SendTextBody();        //�����ı���Ϣ
    //bool SendAttachment();        //���͸���
    int SendAttachment_Ex();
    bool SendEnd();
public:
    void AddAttachment(std::string& filePath); //��Ӹ���
    void DeleteAttachment(std::string& filePath); //ɾ������
    void DeleteAllAttachment(); //ɾ�����еĸ���
    void SetSrvDomain(std::string& domain);
    void SetUserName(std::string& user);
    void SetPass(std::string& pass);
    void SetTargetEmail(std::string& targetAddr);
    void SetEmailTitle(std::string& title);
    void SetContent(std::string& content);
    void SetPort(int port);
    int SendEmail_Ex();
    /*���ڴ������˵��:1.��������µĴ���2.�û�������3.�������4.�ļ�������0.�ɹ�*/
    char* base64Encode(char const* origSigned, unsigned origLength);
};
#endif // !__SMTP_H__
