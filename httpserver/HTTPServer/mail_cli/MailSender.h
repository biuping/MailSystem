#ifndef _MAIL_SENDER_H_
#define _MAIL_SENDER_H_

#include <string>

class MailSender
{
public:
	MailSender();
	virtual ~MailSender();

    virtual void AddAttachment(std::string& filePath)=0; //��Ӹ���
    virtual void DeleteAttachment(std::string& filePath)=0; //ɾ������
    virtual void DeleteAllAttachment()=0; //ɾ�����еĸ���
     /*�������˵��:1.��������µĴ���2.�û�������3.�������4.�ļ�������0.�ɹ�*/
    virtual int SendEmail_Ex()=0;
};


#endif // !_MAIL_SENDER_H_
