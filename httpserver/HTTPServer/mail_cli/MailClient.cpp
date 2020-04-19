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
	} // mReceiver is OK

	if (mSender != nullptr) {
		// sender

	}

	description = "OK";
	return true;
}

const rstring MailClient::RecvMail(rstring& description)
{
	if (mReceiver == nullptr) {
		// δ��Receiver
		description = "No receiver binded";
		return "";
	}

	if (mReceiver->alive()) {
		// δ��¼��ʧ����
		description = "Not logged in or the connection has benn lost";
		return "";
	}

	// ��������
	std::vector<Mail*> mails;
	// ������UID���ʼ�����
	mReceiver->getMailListWithUID(mails);
	for (size_t i = 0; i < mails.size(); ++i) {
		// ȡ�� i ���ʼ�
		if (mReceiver->retrMail(i, mails[i])) {
			// ��ȡ�ɹ�
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
