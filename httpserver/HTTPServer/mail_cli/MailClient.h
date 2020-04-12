#ifndef _MAIL_CLIENT_H_
#define _MAIL_CLIENT_H_

#include "MailReceiver.h"
#include "MailSender.h"


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

	/* mail interfaces */
	std::list<Mail*>& recvAll();
	std::list<mail_header_t&>& recvHeaders();
	bool sendMail(Mail* mail);
private:
	MailReceiver* mReceiver;
	MailSender* mSender;
};


#endif // !_MAIL_CLIENT_H_