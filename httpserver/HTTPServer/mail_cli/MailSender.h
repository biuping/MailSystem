#ifndef _MAIL_SENDER_H_
#define _MAIL_SENDER_H_

#include <string>

class MailSender
{
public:
	MailSender();
	virtual ~MailSender();

    virtual void AddAttachment(std::string& filePath)=0; //添加附件
    virtual void DeleteAttachment(std::string& filePath)=0; //删除附件
    virtual void DeleteAllAttachment()=0; //删除所有的附件
     /*错误码的说明:1.网络错误导致的错误2.用户名错误3.密码错误4.文件不存在0.成功*/
    virtual int SendEmail_Ex()=0;
};


#endif // !_MAIL_SENDER_H_
