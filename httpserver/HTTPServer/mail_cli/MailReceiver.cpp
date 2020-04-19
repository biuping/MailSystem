#include "MailReceiver.h"
#include "../tools/LogUtils.h"


MailReceiver::MailReceiver()
{
}

MailReceiver::~MailReceiver()
{
}

bool MailReceiver::getStatus(int& mailnum, int& totsize)
{
	report("getStatus not implemented");
	return false;
}

bool MailReceiver::getMailListWithSize(std::vector<Mail*>& mails)
{
	report("getMailListWithSize not implemented");
	return false;
}

bool MailReceiver::getMailListWithUID(std::vector<Mail*>& mails)
{
	report("getMailListWithUID not implemented");
	return false;
}

bool MailReceiver::retrMail(size_t i, Mail* mail)
{
	report("retrMail i not implemented");
	return false;
}

bool MailReceiver::retrMail(const rstring& uid, Mail* mail)
{
	report("retrMail uid not implemented");
	return false;
}

bool MailReceiver::deleteMail(size_t i)
{
	report("deleteMail i not implemented");
	return false;
}

bool MailReceiver::deleteMail(const rstring& uid)
{
	report("deleteMail uid not implemented");
	return false;
}

void MailReceiver::deleteMails(const slist& uids, slist& completed)
{
	report("deleteMails not implemented");

}

void MailReceiver::report(const rstring& msg)
{
	LogUtil::report("MailReceiver " + msg);
}
