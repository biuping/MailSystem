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

bool MailReceiver::retrMail(int i, Mail* mail)
{
	report("retrMail not implemented");
	return false;
}

void MailReceiver::report(const rstring& msg)
{
	LogUtil::report("MailReceiver " + msg);
}
