#ifndef _MAIL_RECEIVER_H_
#define _MAIL_RECEIVER_H_

#include "Mail.h"
#include <vector>

typedef unsigned short USHORT;


class MailReceiver
{
public:
	MailReceiver();
	virtual ~MailReceiver();

	/* neccessary interfaces declaration */
	virtual bool open(const rstring& at) = 0;
	virtual bool open(const rstring& at, USHORT port) = 0;
	virtual void close() = 0;
	virtual bool alive() = 0;
	virtual bool authenticate(const rstring& usr, const rstring& passwd) = 0;

	/* specialized interfaces declaration */
	virtual bool getStatus(int& mailnum, int& totsize);
	virtual bool getMailListWithSize(std::vector<Mail*>& mails);
	virtual bool getMailListWithUID(std::vector<Mail*>& mails);
	virtual bool retrMail(size_t i, Mail* mail);
	virtual bool deleteMail(size_t i);
	virtual bool deleteMail(rstring uid);

private:
	virtual void report(const rstring& msg);
};



#endif // !_MAIL_RECEIVER_H_

