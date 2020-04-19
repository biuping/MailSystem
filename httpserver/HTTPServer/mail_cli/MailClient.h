#ifndef _MAIL_CLIENT_H_
#define _MAIL_CLIENT_H_

#include <vector>
#include "MailReceiver.h"
#include "MailSender.h"
#include "..\http\static\Tools.h"

class MailClient
{
public:
	MailClient();
	MailClient(const MailClient& cli);
	MailClient(MailReceiver* receiver, MailSender* sender);
	~MailClient();

	/* receiver, sender setters and getters */
	void setSender(MailSender* sender);
	void setReceiver(MailReceiver* receiver);
	MailReceiver* getReceiver();
	MailSender* getSender();

	/*
	登录
	mailAddr: 邮箱地址
	passwd: 邮箱密码、独立密码或授权码
	description: 结果说明
	return: 是否成功登录
	*/
	bool Login(const rstring& mailAddr, const rstring& passwd, rstring& description);

	/*
	发送邮件
	targetAddr: 目标地址，多个地址空格隔开
	theme：邮件主题
	content：邮件内容
	attachments: 附件
	*/
	rstring SendMail(
		const rstring& targetAddr,
		const rstring& theme,
		const rstring& content,
		std::list<Attachment>& attachments
	);
	/*
	接收邮件
	description: 结果说明
	return: 失败返回空字符串，否则则为邮件的json结构
	*/
	const rstring RecvMail(rstring& description);

	/*
	删除邮件
	mailId：邮件id
	*/
	rstring DeleteMail(const std::list<rstring>& mailId);
	
	/*
	下载附件
	mailId: 邮件id
	attachIndex：邮件序号
	*/
	rstring DownloadAttach(
		const rstring& mailId,
		const rstring& attachIndex
	);

	/* mail interfaces */
	// std::vector<Mail*>& recvAll();
	// std::vector<mail_header_t&>& recvHeaders();
	// bool sendMail(Mail* mail);

private:
	MailReceiver* mReceiver;
	MailSender* mSender;

	/* tools */
	const rstring MailToJson(Mail* mail);
};


#endif // !_MAIL_CLIENT_H_