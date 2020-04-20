#ifndef _MAIL_SENDER_H_
#define _MAIL_SENDER_H_

#include <string>
#include "../http/static/Tools.h"



class MailSender
{
public:
	MailSender();
	virtual ~MailSender();

    virtual void AddAttachment(Attachment attach)=0; //添加附件
    virtual void DeleteAttachment(int index)=0; //删除附件
    virtual void DeleteAllAttachment()=0; //删除所有的附件
     /*错误码的说明:1.网络错误导致的错误2.用户名错误3.密码错误4.文件不存在0.成功*/
    virtual int SendEmail_Ex()=0;
    virtual bool Login(std::string userCount, std::string userPass, std::string& description)=0;
   
    virtual void SetTargetEmail(const std::string& targetAddr)=0;
    virtual void SetEmailTitle(const std::string& title)=0;
    virtual void SetContent(const std::string& content)=0;
    virtual void SetPort(int port)=0;
};


#endif // !_MAIL_SENDER_H_
