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
	} // mReceiver is OK

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
	if (mSender == nullptr)
	{
		// 未绑定sender
		return "";
	}
	for (int i = 0; i < attachments.size(); i++)
		mSender->AddAttachment(attachments[i]);
	int num = mSender->SendEmail_Ex();
	switch (num)
	{
		/*错误码的说明:1.网络错误导致的错误2.用户名错误3.密码错误4.文件不存在0.成功*/
	case 0:
	{
		return "Send success!";
		break;
	}
	case 1:
	{
		return  "Send failed, Internet error!";
		break;
	}
	case 2:
	{
		return  "Send failed, user name error!";
		break;
	}
	case 3:
	{
		return  "Send failed, password error!";
		break;
	}
	case 4:
	{
		return "Send failed, password error!";
		break;
	}
	}

}
const rstring MailClient::RecvMail(rstring& description)
{
	if (mReceiver == nullptr) {
		// 未绑定Receiver
		description = "No receiver binded";
		return "";
	}

	if (mReceiver->alive()) {
		// 未登录或丢失连接
		description = "Not logged in or the connection has benn lost";
		return "";
	}

	// 连接正常
	std::vector<Mail*> mails;
	// 创建带UID的邮件数组
	mReceiver->getMailListWithUID(mails);
	for (size_t i = 0; i < mails.size(); ++i) {
		// 取第 i 封邮件
		if (mReceiver->retrMail(i, mails[i])) {
			// 获取成功
		}
	}

	return rstring();
}

rstring MailClient::DeleteMail(const std::list<rstring>& mailId)
{
	return rstring();
}

rstring MailClient::DownloadAttach(const rstring& mailId, const rstring& attachIndex)
{
	return rstring();
}
