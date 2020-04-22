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
		// 邮箱格式错误
		description = "Invalid Email address";
		return false;
	}

	rstring usr = mailAddr.substr(0, at_pos);
	rstring at = mailAddr.substr(at_pos + 1);
	
	if (mReceiver == nullptr && mSender == nullptr) {
		// 未绑定操作对象
		description = "No receiver or sender binded";
		return false;
	}

	if (mReceiver != nullptr) {
		if (!mReceiver->open(at)) {
			// 打开连接错误
			description = "Cannot open the connection to" + at;
			return false;
		}
		else {
			// 打开连接成功
			if (!mReceiver->authenticate(usr, passwd)) {
				// 验证失败
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
	
	// Json对象
	Json::Value mailsjson;
	bool success = false;
	rstring description;

	if (mSender == nullptr)
	{
		description = "未绑定sender!";
		// 未绑定sender
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
		/*错误码的说明:1.网络错误导致的错误2.用户名错误3.密码错误4.文件不存在0.成功*/
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
	// Json对象
	Json::Value mailsjson;
	bool success = true;
	rstring description;

	mailsjson["mails"] = Json::Value(Json::ValueType::arrayValue);

	if (mReceiver == nullptr) {
		// 未绑定Receiver
		description = "No receiver binded";
		success = false;
	} else if (!mReceiver->alive()) {
		// 未登录或丢失连接
		description = "Not logged in or the connection has benn lost";
		success = false;
	}
	else {
		// 连接正常

		std::vector<Mail*> mails;
		// 创建带UID的邮件数组
		if (!mReceiver->getMailListWithUID(mails)) {
			description = "Cannot get mailbox status and mail list correctly";
			success = false;
		}
		else {
			description = "";

			// 检查邮件数量是否过多
			size_t i = 0;
			size_t total = mails.size();
			if (mails.size() > MAX_RECVMAIL_NUMBER) {
				// 邮件数量超过上限，从最新的开始取
				i = mails.size() - MAX_RECVMAIL_NUMBER;
				total = MAX_RECVMAIL_NUMBER;
				description += std::to_string(mails.size()) +
					" mails (too many) in the mailbox, only latest " +
					std::to_string(MAX_RECVMAIL_NUMBER) + " are collected\n";
			}

			size_t failedCount = 0;
			for (; i < mails.size(); ++i) {
				// 取第 i 封邮件
				if (mReceiver->retrMail(i, mails[i])) {
					// 获取成功
					mailsjson["mails"].append(MailToJson(mails[i], i));
				}
				else {
					// 获取失败
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

			// 释放资源
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
	// Json对象
	Json::Value deleteObj;
	bool success = true;
	rstring description;

	deleteObj["deleted"] = Json::Value(Json::ValueType::arrayValue);

	if (mReceiver == nullptr) {
		// 未绑定Receiver
		description = "No receiver binded";
		success = false;
	}
	else if (!mReceiver->alive()) {
		// 未登录或丢失连接
		description = "Not logged in or the connection has benn lost";
		success = false;
	}
	else {
		// 成功连接
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
	// Json对象
	Json::Value attachObj;
	bool success = true;
	rstring description;

	// keys
	attachObj["filename"] = "";
	attachObj["content-type"] = "";
	attachObj["content"] = "";

	if (mReceiver == nullptr) {
		// 未绑定Receiver
		description = "No receiver binded";
		success = false;
	}
	else if (!mReceiver->alive()) {
		// 未登录或丢失连接
		description = "Not logged in or the connection has benn lost";
		success = false;
	}
	else {
		Mail* mail = new Mail();
		if (mReceiver->retrMail(mailId, mail)) {
			// 取到该邮件
			std::list<MessagePart*> parts;
			mail->getAllAttachmentParts(parts);
			// 检查索引
			if (parts.size() != 0 && attachIndex >= 0 && (size_t)attachIndex < parts.size()) {
				// 寻找索引对应附件
				size_t idx = 0;
				for (std::list<MessagePart*>::iterator ite = parts.begin();
					ite != parts.end();
					++ite) {
					if (idx == (size_t)attachIndex) {
						// 找到该附件，设置属性
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
		// 只有一个可用的文本部分
		mailObj["content"] = (*(textparts.begin()))->getMessage();
	}
	else {
		// 否则只使用普通文本
		mailObj["content"] = mail->getFirstPlainTextMessage();
	}

	// 附件部分
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
