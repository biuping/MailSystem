#include "MailClient.h"

MailClient::MailClient():mReceiver(nullptr), mSender(nullptr)
{
}

MailClient::MailClient(const MailClient& cli) : mReceiver(cli.mReceiver), mSender(cli.mSender)
{
}

MailClient::~MailClient()
{
	delete mReceiver;
	delete mSender;

	mReceiver = nullptr;
	mSender = nullptr;
}