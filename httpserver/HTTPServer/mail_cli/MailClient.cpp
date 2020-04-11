#include "MailClient.h"

MailClient::MailClient()
{
}

MailClient::~MailClient()
{
	delete mReceiver;
	delete mSender;

	mReceiver = nullptr;
	mSender = nullptr;
}