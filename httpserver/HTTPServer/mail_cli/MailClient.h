#ifndef _MAIL_CLIENT_H_
#define _MAIL_CLIENT_H_

#include "MailReceiver.h"
#include "MailSender.h"

class MailClient
{
public:
	MailClient();
	~MailClient();

private:
	MailReceiver* mReceiver;
	MailSender* mSender;
};


#endif // !_MAIL_CLIENT_H_