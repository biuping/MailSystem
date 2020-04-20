#ifndef _MAIL_SENDER_H_
#define _MAIL_SENDER_H_

#include <string>
#include "../http/static/Tools.h"



class MailSender
{
public:
	MailSender();
	virtual ~MailSender();

    virtual void AddAttachment(Attachment attach)=0; //��Ӹ���
    virtual void DeleteAttachment(int index)=0; //ɾ������
    virtual void DeleteAllAttachment()=0; //ɾ�����еĸ���
     /*�������˵��:1.��������µĴ���2.�û�������3.�������4.�ļ�������0.�ɹ�*/
    virtual int SendEmail_Ex()=0;
    virtual bool Login(std::string userCount, std::string userPass, std::string& description)=0;
   
    virtual void SetTargetEmail(const std::string& targetAddr)=0;
    virtual void SetEmailTitle(const std::string& title)=0;
    virtual void SetContent(const std::string& content)=0;
    virtual void SetPort(int port)=0;
};


#endif // !_MAIL_SENDER_H_
