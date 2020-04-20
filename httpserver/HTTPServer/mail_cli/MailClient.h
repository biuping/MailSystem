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
	��¼
	mailAddr: �����ַ
	passwd: �������롢�����������Ȩ��
	description: ���˵��
	return: �Ƿ�ɹ���¼
	*/
	bool Login(const rstring& mailAddr, const rstring& passwd, rstring& description);

	/*
	�����ʼ�
	targetAddr: Ŀ���ַ�������ַ�ո����
	theme���ʼ�����
	content���ʼ�����
	attachments: ����
	*/
	rstring SendMail(
		const rstring& targetAddr,
		const rstring& theme,
		const rstring& content,
		std::list<Attachment*>& attachments
	);
	/*
	�����ʼ�
	description: ���˵��
	return: ʧ�ܷ��ؿ��ַ�����������Ϊ�ʼ���json�ṹ
	*/
	const rstring RecvMail();

	/*
	ɾ���ʼ�
	mailId���ʼ�id
	*/
	const rstring DeleteMail(const std::list<rstring>& mailId);
	
	/*
	���ظ���
	mailId: �ʼ�id
	attachIndex���ʼ����
	*/
	const Json::Value DownloadAttach(
		const rstring& mailId,
		const int attachIndex
	);

	/* mail interfaces */
	// std::vector<Mail*>& recvAll();
	// std::vector<mail_header_t&>& recvHeaders();
	// bool sendMail(Mail* mail);

private:
	MailReceiver* mReceiver;
	MailSender* mSender;

	/* tools */
	const Json::Value MailToJson(Mail* mail, size_t index);
};


#endif // !_MAIL_CLIENT_H_