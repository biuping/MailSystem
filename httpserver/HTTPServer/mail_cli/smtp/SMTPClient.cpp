#include "SMTPClient.h"
#include <iostream>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

//base64����
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
//SMTPĬ�Ϲ��캯��
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
//SMTP��������
SMTP::~SMTP(void)
{
    DeleteAllAttachment();
    closesocket(sockClient);
    WSACleanup();
}
//SMTP���ع��캯������Ҫ����...����
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

//��������
bool SMTP::CreateConn()
{
    //Ϊ����socket������׼������ʼ������
    SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0); //����socket����
    SOCKADDR_IN addrSrv;
    HOSTENT* pHostent;
    pHostent = gethostbyname(domain.c_str());  //�õ��й�����������Ϣ
    addrSrv.sin_addr.S_un.S_addr = *((DWORD*)pHostent->h_addr_list[0]);    //�õ�smtp�������������ֽ����ip��ַ   
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
//������Ϣ����
bool SMTP::Recv()
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
int SMTP::Login()
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
//�����ʼ�ͷ����Ϣ
bool SMTP::SendEmailHead()     
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
//�����ʼ��ı�
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
//���͸���
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
            return 4; /*������4��ʾ�ļ��򿪴���*/
        }
        char fileBuff[MAX_FILE_LEN];
        char* chSendBuff;
        memset(fileBuff, 0, sizeof(fileBuff));
        /*�ļ�ʹ��base64���ܴ���*/
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
            //cout << "�ļ����ͳ���!" << endl;
            return 1;
        }
        delete[]chSendBuff;
    }
    return 0;
}
//���ͽ�β��Ϣ
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

//�����ʼ�����
int SMTP::SendEmail_Ex()
{
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
void SMTP::AddAttachment(std::string& filePath)
{
    FILEINFO* pFile = new FILEINFO;
    strcpy_s(pFile->filePath, filePath.c_str());
    const char* p = filePath.c_str();
    strcpy_s(pFile->fileName, p + filePath.find_last_of("\\") + 1);
    listFile.push_back(pFile);
}

//ɾ������
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
//ɾ�����е��ļ�
void SMTP::DeleteAllAttachment() 
{
    for (std::list<FILEINFO*>::iterator pIter = listFile.begin(); pIter != listFile.end();)
    {
        FILEINFO* p = *pIter;
        pIter = listFile.erase(pIter);
        delete p;
    }
}
//set get����
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
