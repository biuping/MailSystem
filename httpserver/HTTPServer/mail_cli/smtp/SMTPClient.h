#ifndef __SMTP_H__ //避免重复包含
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
struct FILEINFO /*用来记录文件的一些信息*/
{
    char fileName[128]; /*文件名称*/
    char filePath[512]; /*文件绝对路径*/
};
class SMTPClient:public MailSender
{
public:
    SMTPClient(void);
    SMTPClient(
        int port,
        std::string srvDomain,  //SMTPClient服务器域名
        std::string userName,   //用户名
        std::string password,   //密码
        std::string targetEmail, //目的邮件地址
        std::string emailTitle,  //主题
        std::string content       //内容
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
    SOCKET sockClient;  //客户端的套接字
public:
    bool CreateConn(); /*创建连接*/
    bool Send(std::string& message);
    bool Recv();
    void FormatEmailHead(std::string& email);//格式化要发送的邮件头部
    int Login();
    bool Login(string userCount, string userPass, string& description);
    bool SendEmailHead();       //发送邮件头部信息
    bool SendTextBody();        //发送文本信息

    int SendAttachment_Ex();
    bool SendEnd();
public:
    void AddAttachment(Attachment attachs); //添加附件
    void DeleteAttachment(int index); //删除附件
    void DeleteAllAttachment(); //删除所有的附件
     /*错误码的说明:1.网络错误导致的错误2.用户名错误3.密码错误4.文件不存在0.成功*/
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
