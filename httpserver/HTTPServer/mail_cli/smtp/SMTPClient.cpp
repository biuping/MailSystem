#include "SMTPClient.h"
#include <iostream>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

//base64编码
char* SMTP::base64Encode(char const* origSigned, unsigned origLength)
{
    unsigned char const* orig = (unsigned char const*)origSigned;
    if (orig == NULL) return NULL;
    unsigned const numOrig24BitValues = origLength / 3;
    bool havePadding = origLength > numOrig24BitValues * 3;
    bool havePadding2 = origLength == numOrig24BitValues * 3 + 2;
    unsigned const numResultBytes = 4 * (numOrig24BitValues + havePadding);
    char* result = new char[numResultBytes + 3]; 

    unsigned i;
    for (i = 0; i < numOrig24BitValues; ++i)
    {
        result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
        result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
        result[4 * i + 2] = base64Char[((orig[3 * i + 1] << 2) | (orig[3 * i + 2] >> 6)) & 0x3F];
        result[4 * i + 3] = base64Char[orig[3 * i + 2] & 0x3F];
    }

    if (havePadding)
    {
        result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F];
        if (havePadding2)
        {
            result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
            result[4 * i + 2] = base64Char[(orig[3 * i + 1] << 2) & 0x3F];
        }
        else
        {
            result[4 * i + 1] = base64Char[((orig[3 * i] & 0x3) << 4) & 0x3F];
            result[4 * i + 2] = '=';
        }
        result[4 * i + 3] = '=';
    }
    result[numResultBytes] = '\0';
    return result;
}
//SMTP默认构造函数
SMTP::SMTP(void)
{
    this->content = "";
    this->port = 25;
    this->user = "";
    this->pass = "";
    this->targetAddr = "";
    this->title = "";
    this->domain = "";
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    this->sockClient = 0;
}
//SMTP析构函数
SMTP::~SMTP(void)
{
    DeleteAllAttachment();
    closesocket(sockClient);
    WSACleanup();
}
//SMTP重载构造函数，需要传入...数据
SMTP::SMTP(
    int port,
    std::string srvDomain,
    std::string userName,
    std::string password,
    std::string targetEmail,
    std::string emailTitle,
    std::string content
)
{
    this->content = content;
    this->port = port;
    this->user = userName;
    this->pass = password;
    this->targetAddr = targetEmail;
    this->title = emailTitle;
    this->domain = srvDomain;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    this->sockClient = 0;
}

//建立连接
bool SMTP::CreateConn()
{
    //为建立socket对象做准备，初始化环境
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0); //建立socket对象
    SOCKADDR_IN addrSrv;
    HOSTENT* pHostent;
    pHostent = gethostbyname(domain.c_str());  //得到有关于域名的信息
    addrSrv.sin_addr.S_un.S_addr = *((DWORD*)pHostent->h_addr_list[0]);    //得到smtp服务器的网络字节序的ip地址   
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    int err = connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));   //向服务器发送请求 
    if (err != 0)
    {
        return false;
    }
    this->sockClient = sockClient;
    if (false == Recv())
    {
        return false;
    }
    return true;
}
//发送信息函数
bool SMTP::Send(std::string& message)
{
    int err = send(sockClient, message.c_str(), message.length(), 0);
    if (err == SOCKET_ERROR)
    {
        return false;
    }
    std::string message01;
    return true;
}
//接收信息函数
bool SMTP::Recv()
{
    memset(buff, 0, sizeof(char) * (MAXLEN + 1));
    int err = recv(sockClient, buff, MAXLEN, 0); //接收数据
    if (err == SOCKET_ERROR)
    {
        return false;
    }
    buff[err] = '\0';
    return true;
}
//登录函数
int SMTP::Login()
{
    std::string sendBuff;
    sendBuff = "EHLO ";
    sendBuff += user; // 这一部分需要通过telnet验证一下
    sendBuff += "\r\n";
    if (false == Send(sendBuff) || false == Recv()) //既接收也发送
    {
        return 1; /*1表示发送失败由于网络错误*/
    }
    sendBuff.empty();
    sendBuff = "AUTH LOGIN\r\n";
    if (false == Send(sendBuff) || false == Recv()) //请求登陆
    {
        return 1; /*1表示发送失败由于网络错误*/
    }
    sendBuff.empty();
    int pos = user.find('@', 0);
    sendBuff = user.substr(0, pos); //得到用户名
    char* ecode;
    ecode = base64Encode(sendBuff.c_str(), strlen(sendBuff.c_str()));
    sendBuff.empty();
    sendBuff = ecode;
    sendBuff += "\r\n";
    delete[]ecode;
    if (false == Send(sendBuff) || false == Recv()) //发送用户名，并接收服务器的返回
    {
        return 1; /*错误码1表示发送失败由于网络错误*/
    }
    sendBuff.empty();
    ecode = base64Encode(pass.c_str(), strlen(pass.c_str()));
    sendBuff = ecode;
    sendBuff += "\r\n";
    delete[]ecode;
    if (false == Send(sendBuff) || false == Recv()) //发送用户密码，并接收服务器的返回
    {
        return 1; /*错误码1表示发送失败由于网络错误*/
    }
    if (NULL != strstr(buff, "550"))
    {
        return 2;/*错误码2表示用户名错误*/
    }
    if (NULL != strstr(buff, "535")) /*535是认证失败的返回*/
    {
        return 3; /*错误码3表示密码错误*/
    }
    return 0;
}
//发送邮件头部信息
bool SMTP::SendEmailHead()     
{
    std::string sendBuff;
    sendBuff = "MAIL FROM: <" + user + ">\r\n";
    if (false == Send(sendBuff) || false == Recv())
    {
        return false; /*表示发送失败由于网络错误*/
    }
    std::istringstream is(targetAddr);
    std::string tmpadd;
    while (is >> tmpadd)
    {
        sendBuff.empty();
        sendBuff = "RCPT TO: <" + tmpadd + ">\r\n";
        if (false == Send(sendBuff) || false == Recv())
        {
            return false; /*表示发送失败由于网络错误*/
        }
    }
    sendBuff.empty();
    sendBuff = "DATA\r\n";
    if (false == Send(sendBuff) || false == Recv())
    {
        return false; //表示发送失败由于网络错误
    }
    sendBuff.empty();
    FormatEmailHead(sendBuff);
    if (false == Send(sendBuff))
    {
        return false; /*表示发送失败由于网络错误*/
    }
    return true;
}
//格式化要发送的内容
void SMTP::FormatEmailHead(std::string& email)
{
    email = "From: ";
    email += user;
    email += "\r\n";
    email += "To: ";
    email += targetAddr;
    email += "\r\n";
    email += "Subject: ";
    email += title;
    email += "\r\n";
    email += "MIME-Version: 1.0";
    email += "\r\n";
    email += "Content-Type: multipart/mixed;boundary=qwertyuiop";
    email += "\r\n";
    email += "\r\n";
}
//发送邮件文本
bool SMTP::SendTextBody() 
{
    std::string sendBuff;
    sendBuff = "--qwertyuiop\r\n";
    sendBuff += "Content-Type: text/plain;";
    sendBuff += "charset=\"gb2312\"\r\n\r\n";
    sendBuff += content;
    sendBuff += "\r\n\r\n";
    return Send(sendBuff);
}
//发送附件
int SMTP::SendAttachment_Ex() 
{
    for (std::list<FILEINFO*>::iterator pIter = listFile.begin(); pIter != listFile.end(); pIter++)
    {
        std::string sendBuff;
        sendBuff = "--qwertyuiop\r\n";
        sendBuff += "Content-Type: application/octet-stream;\r\n";
        sendBuff += " name=\"";
        sendBuff += (*pIter)->fileName;
        sendBuff += "\"";
        sendBuff += "\r\n";
        sendBuff += "Content-Transfer-Encoding: base64\r\n";
        sendBuff += "Content-Disposition: attachment;\r\n";
        sendBuff += " filename=\"";
        sendBuff += (*pIter)->fileName;
        sendBuff += "\"";
        sendBuff += "\r\n";
        sendBuff += "\r\n";
        Send(sendBuff);
        std::ifstream ifs((*pIter)->filePath, std::ios::in | std::ios::binary);
        if (false == ifs.is_open())
        {
            return 4; /*错误码4表示文件打开错误*/
        }
        char fileBuff[MAX_FILE_LEN];
        char* chSendBuff;
        memset(fileBuff, 0, sizeof(fileBuff));
        /*文件使用base64加密传送*/
        while (ifs.read(fileBuff, MAX_FILE_LEN))
        {
            //cout << ifs.gcount() << endl;
            chSendBuff = base64Encode(fileBuff, MAX_FILE_LEN);
            chSendBuff[strlen(chSendBuff)] = '\r';
            chSendBuff[strlen(chSendBuff)] = '\n';
            send(sockClient, chSendBuff, strlen(chSendBuff), 0);
            delete[]chSendBuff;
        }
     
        chSendBuff = base64Encode(fileBuff, ifs.gcount());
        chSendBuff[strlen(chSendBuff)] = '\r';
        chSendBuff[strlen(chSendBuff)] = '\n';
        int err = send(sockClient, chSendBuff, strlen(chSendBuff), 0);
        if (err != strlen(chSendBuff))
        {
            //cout << "文件传送出错!" << endl;
            return 1;
        }
        delete[]chSendBuff;
    }
    return 0;
}
//发送结尾信息
bool SMTP::SendEnd() 
{
    std::string sendBuff;
    sendBuff = "--qwertyuiop--";
    sendBuff += "\r\n.\r\n";
    if (false == Send(sendBuff) || false == Recv())
    {
        return false;
    }
 
    sendBuff.empty();
    sendBuff = "QUIT\r\n";
    return (Send(sendBuff) && Recv());
}

//发送邮件函数
int SMTP::SendEmail_Ex()
{
    if (false == CreateConn())
    {
        return 1;
    }
   
    int err = Login(); //先登录
    if (err != 0)
    {
        return err; //错误代码必须要返回
    }
    if (false == SendEmailHead()) //发送EMAIL头部信息
    {
        return 1; /*错误码1是由于网络的错误*/
    }
    if (false == SendTextBody())
    {
        return 1; /*错误码1是由于网络的错误*/
    }
    err = SendAttachment_Ex();
    if (err != 0)
    {
        return err;
    }
    if (false == SendEnd())
    {
        return 1; /*错误码1是由于网络的错误*/
    }
    return 0; /*0表示没有出错*/
}

//添加附件函数
void SMTP::AddAttachment(std::string& filePath)
{
    FILEINFO* pFile = new FILEINFO;
    strcpy_s(pFile->filePath, filePath.c_str());
    const char* p = filePath.c_str();
    strcpy_s(pFile->fileName, p + filePath.find_last_of("\\") + 1);
    listFile.push_back(pFile);
}

//删除附件
void SMTP::DeleteAttachment(std::string& filePath) 
{
    std::list<FILEINFO*>::iterator pIter;
    for (pIter = listFile.begin(); pIter != listFile.end(); pIter++)
    {
        if (strcmp((*pIter)->filePath, filePath.c_str()) == 0)
        {
            FILEINFO* p = *pIter;
            listFile.remove(*pIter);
            delete p;
            break;
        }
    }
}
//删除所有的文件
void SMTP::DeleteAllAttachment() 
{
    for (std::list<FILEINFO*>::iterator pIter = listFile.begin(); pIter != listFile.end();)
    {
        FILEINFO* p = *pIter;
        pIter = listFile.erase(pIter);
        delete p;
    }
}
//set get函数
void SMTP::SetSrvDomain(std::string& domain)
{
    this->domain = domain;
}
void SMTP::SetUserName(std::string& user)
{
    this->user = user;
}
void SMTP::SetPass(std::string& pass)
{
    this->pass = pass;
}
void SMTP::SetTargetEmail(std::string& targetAddr)
{
    this->targetAddr = targetAddr;
}
void SMTP::SetEmailTitle(std::string& title)
{
    this->title = title;
}
void SMTP::SetContent(std::string& content)
{
    this->content = content;
}
void SMTP::SetPort(int port)
{
    this->port = port;
}
