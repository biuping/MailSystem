#include "SMTPClient.h"
#include <iostream>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

//base64����
char* SMTPClient::base64Encode(char const* origSigned, unsigned origLength)
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
//SMTPClientĬ�Ϲ��캯��
SMTPClient::SMTPClient(void)
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
    wVersionRequested = MAKEWORD(2, 1);
    int err=0;
    this->sockClient = 0;
}
//SMTPClient��������
SMTPClient::~SMTPClient(void)
{
    DeleteAllAttachment();
    closesocket(sockClient);
}
//SMTPClient���ع��캯������Ҫ����...����
SMTPClient::SMTPClient(
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
    int err=0;
    wVersionRequested = MAKEWORD(2, 1);
    this->sockClient = 0;
}

//��������
bool SMTPClient::CreateConn()
{
    //Ϊ����socket������׼������ʼ������
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0); //����socket����
    SOCKADDR_IN addrSrv;
    HOSTENT* pHostent;
    pHostent = gethostbyname(domain.c_str());  //�õ��й�����������Ϣ
    addrSrv.sin_addr.S_un.S_addr = *((DWORD*)pHostent->h_addr_list[0]);    //�õ�SMTPClient�������������ֽ����ip��ַ   
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    int err = connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));   //��������������� 
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
//������Ϣ����
bool SMTPClient::Send(std::string& message)
{
    int err = send(sockClient, message.c_str(), message.length(), 0);
    if (err == SOCKET_ERROR)
    {
        return false;
    }
    std::string message01;
    return true;
}
//������Ϣ����
bool SMTPClient::Recv()
{
    memset(buff, 0, sizeof(char) * (MAXLEN + 1));
    int err = recv(sockClient, buff, MAXLEN, 0); //��������
    if (err == SOCKET_ERROR)
    {
        return false;
    }
    buff[err] = '\0';
    return true;
}
//��¼����
int SMTPClient::Login()
{
    std::string sendBuff;
    sendBuff = "EHLO ";
    sendBuff += user; // ��һ������Ҫͨ��telnet��֤һ��
    sendBuff += "\r\n";
    if (false == Send(sendBuff) || false == Recv()) //�Ƚ���Ҳ����
    {
        return 1; /*1��ʾ����ʧ�������������*/
    }
    sendBuff.empty();
    sendBuff = "AUTH LOGIN\r\n";
    if (false == Send(sendBuff) || false == Recv()) //�����½
    {
        return 1; /*1��ʾ����ʧ�������������*/
    }
    sendBuff.empty();
    int pos = user.find('@', 0);
    sendBuff = user.substr(0, pos); //�õ��û���
    char* ecode;
    ecode = base64Encode(sendBuff.c_str(), strlen(sendBuff.c_str()));
    sendBuff.empty();
    sendBuff = ecode;
    sendBuff += "\r\n";
    delete[]ecode;
    if (false == Send(sendBuff) || false == Recv()) //�����û����������շ������ķ���
    {
        return 1; /*������1��ʾ����ʧ�������������*/
    }
    sendBuff.empty();
    ecode = base64Encode(pass.c_str(), strlen(pass.c_str()));
    sendBuff = ecode;
    sendBuff += "\r\n";
    delete[]ecode;
    if (false == Send(sendBuff) || false == Recv()) //�����û����룬�����շ������ķ���
    {
        return 1; /*������1��ʾ����ʧ�������������*/
    }
    if (NULL != strstr(buff, "550"))
    {
        return 2;/*������2��ʾ�û�������*/
    }
    if (NULL != strstr(buff, "535")) /*535����֤ʧ�ܵķ���*/
    {
        return 3; /*������3��ʾ�������*/
    }
    return 0;
}
//��¼���� mailsender!
bool SMTPClient::Login(string userCount, string userPass, string& description)
{
    string smtpServer="smtp.";
    int Stmppos = userCount.find('@', 0);
    smtpServer += userCount.substr(Stmppos+1,userCount.size()); //�õ��û���
    this->SetPass(userPass);
    this->SetUserName(userCount);
    this->SetSrvDomain(smtpServer);
    if (false == CreateConn())
    {
        return false;
    }
    string sendBuff;
    sendBuff = "EHLO ";
    sendBuff += userCount; // ��һ������Ҫͨ��telnet��֤һ��
    sendBuff += "\r\n";
    if (false == Send(sendBuff) || false == Recv()) //�Ƚ���Ҳ����
    {
        description = "Login failed, Internet error!";
        return false;
    }
    sendBuff.empty();
    sendBuff = "AUTH LOGIN\r\n";
    if (false == Send(sendBuff) || false == Recv()) //�����½
    {
        description = "Login failed, Internet error!";
        return false;
    }
    sendBuff.empty();
    int pos = userCount.find('@', 0);
    sendBuff = userCount.substr(0, pos); //�õ��û���
    char* ecode;
    ecode = base64Encode(sendBuff.c_str(), strlen(sendBuff.c_str()));
    sendBuff.empty();
    sendBuff = ecode;
    sendBuff += "\r\n";
    delete[]ecode;
    if (false == Send(sendBuff) || false == Recv()) //�����û����������շ������ķ���
    {
        description = "Login failed, Internet error!";
        return false;
    }
    sendBuff.empty();
    ecode = base64Encode(userPass.c_str(), strlen(userPass.c_str()));
    sendBuff = ecode;
    sendBuff += "\r\n";
    delete[]ecode;
    if (false == Send(sendBuff) || false == Recv()) //�����û����룬�����շ������ķ���
    {
        description = "Login failed, Internet error!";
        return false;
    }
    if (NULL != strstr(buff, "550"))
    {
        description = "Login failed, user name error!";
        return false;
    }
    if (NULL != strstr(buff, "535")) /*535����֤ʧ�ܵķ���*/
    {
        description = "Login failed,password error!";
        return false;
    }
   
}
//�����ʼ�ͷ����Ϣ
bool SMTPClient::SendEmailHead()     
{
    std::string sendBuff;
    sendBuff = "MAIL FROM: <" + user + ">\r\n";
    if (false == Send(sendBuff) || false == Recv())
    {
        return false; /*��ʾ����ʧ�������������*/
    }
    std::istringstream is(targetAddr);
    std::string tmpadd;
    while (is >> tmpadd)
    {
        sendBuff.empty();
        sendBuff = "RCPT TO: <" + tmpadd + ">\r\n";
        if (false == Send(sendBuff) || false == Recv())
        {
            return false; /*��ʾ����ʧ�������������*/
        }
    }
    sendBuff.empty();
    sendBuff = "DATA\r\n";
    if (false == Send(sendBuff) || false == Recv())
    {
        return false; //��ʾ����ʧ�������������
    }
    sendBuff.empty();
    FormatEmailHead(sendBuff);
    if (false == Send(sendBuff))
    {
        return false; /*��ʾ����ʧ�������������*/
    }
    return true;
}
//��ʽ��Ҫ���͵�����
void SMTPClient::FormatEmailHead(std::string& email)
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
//�����ʼ��ı�
bool SMTPClient::SendTextBody() 
{
    std::string sendBuff;
    sendBuff = "--qwertyuiop\r\n";
    sendBuff += "Content-Type: text/plain;";
    sendBuff += "charset=\"gb2312\"\r\n\r\n";
    sendBuff += content;
    sendBuff += "\r\n\r\n";
    return Send(sendBuff);
}
//���͸���
int SMTPClient::SendAttachment_Ex() 
{
    for (int i = 0; i < Attachments.size(); i++)
    {
        std::string sendBuff;
        sendBuff = "--qwertyuiop\r\n";
        sendBuff += "Content-Type: ";
        sendBuff += Attachments[i].content_type;
        sendBuff += ";\r\n";
        sendBuff += " name=\"";

        sendBuff += Attachments[i].file_name;
        sendBuff += "\"";
        sendBuff += "\r\n";
        sendBuff += "Content-Transfer-Encoding: base64\r\n";
        sendBuff += "Content-Disposition: attachment;\r\n";
        sendBuff += " filename=\"";

        sendBuff += Attachments[i].file_name;
        sendBuff += "\"";
        sendBuff += "\r\n";
        sendBuff += "\r\n";
        Send(sendBuff);

        char fileBuff[MAX_FILE_LEN];
        char* chSendBuff;
        memset(fileBuff, 0, sizeof(fileBuff));
        int allLength = Attachments[i].content.size()/ MAX_FILE_LEN;
        string contents = Attachments[i].content;
        int index = 0;
        while (index<allLength)
        {
            string subContent = contents.substr(MAX_FILE_LEN * index, MAX_FILE_LEN * (index + 1));
            /*�ļ�ʹ��base64���ܴ���*/
            strncpy(fileBuff, subContent.c_str(), MAX_FILE_LEN);
            fileBuff[MAX_FILE_LEN] = '\0';
            chSendBuff = base64Encode(fileBuff, MAX_FILE_LEN);
            chSendBuff[strlen(chSendBuff)] = '\r';
            chSendBuff[strlen(chSendBuff)] = '\n';
            send(sockClient, chSendBuff, strlen(chSendBuff), 0);
            delete[]chSendBuff;
            index++;
        }
        
        string subContent = contents.substr(MAX_FILE_LEN * index, contents.size());
        /*�ļ�ʹ��base64���ܴ���*/
        strncpy(fileBuff, subContent.c_str(), subContent.size());
        fileBuff[subContent.size()] = '\0';
        chSendBuff = base64Encode(fileBuff, Attachments[i].content.size());
        chSendBuff[strlen(chSendBuff)] = '\r';
        chSendBuff[strlen(chSendBuff)] = '\n';
        int err = send(sockClient, chSendBuff, strlen(chSendBuff), 0);
        if (err != strlen(chSendBuff))
        {
            return 1;
        }
        delete[]chSendBuff;
    }
    return 0;
}
//���ͽ�β��Ϣ
bool SMTPClient::SendEnd() 
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

//�����ʼ�����
int SMTPClient::SendEmail_Ex(const rstring& targetAddr, const rstring& theme, const rstring& content)
{
    this->SetTargetEmail(targetAddr);
    this->SetEmailTitle(theme);
    this->SetContent(content);
    if (false == CreateConn())
    {
        return 1;
    }
   
    int err = Login(); //�ȵ�¼
    if (err != 0)
    {
        return err; //����������Ҫ����
    }
    if (false == SendEmailHead()) //����EMAILͷ����Ϣ
    {
        return 1; /*������1����������Ĵ���*/
    }
    if (false == SendTextBody())
    {
        return 1; /*������1����������Ĵ���*/
    }
    err = SendAttachment_Ex();
    if (err != 0)
    {
        return err;
    }
    if (false == SendEnd())
    {
        return 1; /*������1����������Ĵ���*/
    }
    return 0; /*0��ʾû�г���*/
}

//��Ӹ�������
void SMTPClient::AddAttachment(Attachment attachs) //��Ӹ���
{
    Attachments.push_back(attachs);
}

//ɾ������
void SMTPClient::DeleteAttachment(int index)
{
    vector <Attachment> ::iterator itr = Attachments.begin() + index - 1;
    Attachments.erase(itr);
}
//ɾ�����е��ļ�
void SMTPClient::DeleteAllAttachment() 
{
    Attachments.clear();
}
//set get����
void SMTPClient::SetSrvDomain(std::string& domain)
{
    this->domain = domain;
}
void SMTPClient::SetUserName(std::string& user)
{
    this->user = user;
}
void SMTPClient::SetPass(std::string& pass)
{
    this->pass = pass;
}
void SMTPClient::SetTargetEmail(const std::string& targetAddr)
{
    this->targetAddr = targetAddr;
}
void SMTPClient::SetEmailTitle(const std::string& title)
{
    this->title = title;
}
void SMTPClient::SetContent(const std::string& content)
{
    this->content = content;
}
void SMTPClient::SetPort(int port)
{
    this->port = port;
}
