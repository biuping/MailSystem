#include "MailClient.h"

MailClient::MailClient():mReceiver(nullptr), mSender(nullptr)
{
}

MailClient::MailClient(const MailClient& cli) : mReceiver(cli.mReceiver), mSender(cli.mSender)
{
}

MailClient::MailClient(MailReceiver* receiver, MailSender* sender) : mReceiver(receiver), mSender(sender)
{
}

MailClient::~MailClient()
{
	delete mReceiver;
	delete mSender;

	mReceiver = nullptr;
	mSender = nullptr;
}

void MailClient::setSender(MailSender* sender)
{
	mSender = sender;
}

void MailClient::setReceiver(MailReceiver* receiver)
{
	mReceiver = receiver;
}

MailReceiver* MailClient::getReceiver()
{
	return mReceiver;
}

MailSender* MailClient::getSender()
{
	return mSender;
}

bool MailClient::Login(const rstring& mailAddr, const rstring& passwd, rstring& description)
{
	
	std::cout << mailAddr;
	size_t at_pos = mailAddr.find('@');
	if (at_pos == rstring::npos) {
		// �����ʽ����
		description = "Invalid Email address";
		return false;
	}

	rstring usr = mailAddr.substr(0, at_pos);
	rstring at = mailAddr.substr(at_pos + 1);
	
	if (mReceiver == nullptr && mSender == nullptr) {
		// δ�󶨲�������
		description = "No receiver or sender binded";
		return false;
	}

	if (mReceiver != nullptr) {
		if (!mReceiver->open(at)) {
			// �����Ӵ���
			description = "Cannot open the connection to" + at;
			return false;
		}
		else {
			// �����ӳɹ�
			if (!mReceiver->authenticate(usr, passwd)) {
				// ��֤ʧ��
				description = "Authentication failed";
				return false;
			}
		}
	} // receiver is OK

	if (mSender != nullptr) {
		// sender
		if (!mSender->Login(mailAddr, passwd, description))
		{
			return false;
		}

	}
	description = "OK";
	return true;
}
rstring MailClient::SendMail(const rstring& targetAddr,const rstring& theme,const rstring& content,std::vector<Attachment>& attachments) 
{
	
	// Json����
	Json::Value mailsjson;
	bool success = false;
	rstring description;

	if (mSender == nullptr)
	{
		description = "δ��sender!";
		// δ��sender
		return "";
	}
	for (int i = 0; i < attachments.size(); i++)
	{
		if (attachments[i].content_type != "")
		{
			mSender->AddAttachment(attachments[i]);
		}
	}
		
	int num = mSender->SendEmail_Ex(targetAddr,theme,content);
	switch (num)
	{
		/*�������˵��:1.��������µĴ���2.�û�������3.�������4.�ļ�������0.�ɹ�*/
	case 0:
	{
		success = true;
		description= "Send success!";
		break;
	}
	case 1:
	{
		success = false;
		description=  "Send failed, Internet error!";
		break;
	}
	case 2:
	{
		success = false;
		description=  "Send failed, user name error!";
		break;
	}
	case 3:
	{
		success = false;
		description = "Send failed, password error!";
		break;
	}
	case 4:
	{
		success = false;
		description= "Send failed, password error!";
		break;
	}
	}
	mailsjson["success"] = success;
	mailsjson["description"] = description;

	rstring mailsjsonStr;
	Tools::json_write(mailsjson, mailsjsonStr);

	return mailsjsonStr;

}

const rstring MailClient::RecvMail()
{
	// Json����
	Json::Value mailsjson;
	bool success = true;
	rstring description;

	mailsjson["mails"] = Json::Value(Json::ValueType::arrayValue);

	if (mReceiver == nullptr) {
		// δ��Receiver
		description = "No receiver binded";
		success = false;
	} else if (!mReceiver->alive()) {
		// δ��¼��ʧ����
		description = "Not logged in or the connection has benn lost";
		success = false;
	}
	else {
		// ��������

		std::vector<Mail*> mails;
		// ������UID���ʼ�����
		if (!mReceiver->getMailListWithUID(mails)) {
			description = "Cannot get mailbox status and mail list correctly";
			success = false;
		}
		else {
			description = "";

			// ����ʼ������Ƿ����
			size_t i = 0;
			size_t total = mails.size();
			if (mails.size() > MAX_RECVMAIL_NUMBER) {
				// �ʼ������������ޣ������µĿ�ʼȡ
				i = mails.size() - MAX_RECVMAIL_NUMBER;
				total = MAX_RECVMAIL_NUMBER;
				description += std::to_string(mails.size()) +
					" mails (too many) in the mailbox, only latest " +
					std::to_string(MAX_RECVMAIL_NUMBER) + " are collected\n";
			}

			size_t failedCount = 0;
			for (; i < mails.size(); ++i) {
				// ȡ�� i ���ʼ�
				if (mReceiver->retrMail(i, mails[i])) {
					// ��ȡ�ɹ�
					mailsjson["mails"].append(MailToJson(mails[i], i));
				}
				else {
					// ��ȡʧ��
					mailsjson["mails"].append(Json::Value::nullSingleton());
					++failedCount;
				}
			}

			description += "Total :" + std::to_string(total) +
				", success: " + std::to_string(total - failedCount) +
				", failed: " + std::to_string(failedCount);
			if (failedCount >= total) {
				success = false;
			}

			// �ͷ���Դ
			for (size_t i = 0; i < mails.size(); ++i) {
				delete mails[i];
				mails[i] = nullptr;
			}
		}
	}
	mailsjson["success"] = success;
	mailsjson["description"] = description;

	rstring mailsjsonStr;
	Tools::json_write(mailsjson, mailsjsonStr);
	
	return mailsjsonStr;
}

const rstring MailClient::DeleteMail(const std::list<rstring>& mailId)
{
	// Json����
	Json::Value deleteObj;
	bool success = true;
	rstring description;

	deleteObj["deleted"] = Json::Value(Json::ValueType::arrayValue);

	if (mReceiver == nullptr) {
		// δ��Receiver
		description = "No receiver binded";
		success = false;
	}
	else if (!mReceiver->alive()) {
		// δ��¼��ʧ����
		description = "Not logged in or the connection has benn lost";
		success = false;
	}
	else {
		// �ɹ�����
		slist completed;
		mReceiver->deleteMails(mailId, completed);
		description = "Total: " + std::to_string(mailId.size()) +
			", success: " + std::to_string(completed.size()) +
			", failed: " + std::to_string(mailId.size() - completed.size());

		if (completed.size() != mailId.size()) {
			success = false;
		}
		
		for (rstring s : completed) {
			deleteObj["deleted"].append(s);
		}
	}

	deleteObj["success"] = success;
	deleteObj["description"] = description;

	rstring deleteJsonStr;
	Tools::json_write(deleteObj, deleteJsonStr);
	return deleteJsonStr;
}

const Json::Value MailClient::DownloadAttach(const rstring& mailId, const int attachIndex)
{
	// Json����
	Json::Value attachObj;
	bool success = true;
	rstring description;

	// keys
	attachObj["filename"] = "";
	attachObj["content-type"] = "";
	attachObj["content"] = "";

	if (mReceiver == nullptr) {
		// δ��Receiver
		description = "No receiver binded";
		success = false;
	}
	else if (!mReceiver->alive()) {
		// δ��¼��ʧ����
		description = "Not logged in or the connection has benn lost";
		success = false;
	}
	else {
		Mail* mail = new Mail();
		if (mReceiver->retrMail(mailId, mail)) {
			// ȡ�����ʼ�
			std::list<MessagePart*> parts;
			mail->getAllAttachmentParts(parts);
			// �������
			if (parts.size() != 0 && attachIndex >= 0 && (size_t)attachIndex < parts.size()) {
				// Ѱ��������Ӧ����
				size_t idx = 0;
				for (std::list<MessagePart*>::iterator ite = parts.begin();
					ite != parts.end();
					++ite) {
					if (idx == (size_t)attachIndex) {
						// �ҵ��ø�������������
						attachObj["filename"] = (*ite)->getFileName();
						attachObj["content-type"] = (*ite)->getContentType().raw;
						attachObj["content"] = (*ite)->getMessage();
						description = "OK";
						break;
					}
					++idx;
				}
			}
			else {
				success = false;
				description = "Cannot find attachment " +
					std::to_string(attachIndex) + " on mail '" + mailId + "'";
			}
		}
		else {
			success = false;
			description = "Cannot retreive the mail '" + mailId + "'";
		}

		delete mail;
	}

	attachObj["success"] = success;
	attachObj["description"] = description;
	return attachObj;
}

const Json::Value MailClient::MailToJson(Mail* mail, size_t index)
{
	if (mail == nullptr) {
		return Json::Value::nullSingleton();
	}

	Json::Value mailObj;
	mail_header_t header = mail->getHeader();
	mailObj["mail_id"] = mail->getUID();
	mailObj["sender"] = header.from.name.size() == 0 ?
		header.from.addr :
		header.from.name + " <" + header.from.addr + ">";
	mailObj["theme"] = header.subject;
	mailObj["order_id"] = index;

	// find a available text version
	std::list<MessagePart*> textparts;
	mail->getAllTextParts(textparts);
	if (textparts.size() == 1) {
		// ֻ��һ�����õ��ı�����
		mailObj["content"] = (*(textparts.begin()))->getMessage();
	}
	else {
		// ����ֻʹ����ͨ�ı�
		mailObj["content"] = mail->getFirstPlainTextMessage();
	}

	// ��������
	mailObj["attachments"] = Json::Value(Json::ValueType::arrayValue);
	std::list<MessagePart*> attachs;
	mail->getAllAttachmentParts(attachs);
	for (MessagePart* att : attachs) {
		Json::Value attachObj;
		attachObj["name"] = att->getFileName();
		attachObj["size"] = att->getContentDisposition().rawsize.size() == 0 ?
			std::to_string(att->getMessage().size()) + "B" :
			att->getContentDisposition().rawsize;
		mailObj["attachments"].append(attachObj);
	}

	return mailObj;
}
