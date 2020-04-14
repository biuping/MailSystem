#include "Mail.h"


Mail::Mail():mSize(0),mUID("")
{
}

Mail::~Mail()
{
}

// mailsize
void Mail::setSize(const size_t mailsize)
{
	this->mSize = mailsize;
}

void Mail::setUID(const rstring& mailUID)
{
	this->mUID = mailUID;
}

const size_t Mail::getSize()
{
	return this->mSize;
}

const rstring& Mail::getUID()
{
	return this->mUID;
}

void Mail::setHeader(const mail_header_t& header)
{
	this->mHeader = header;
}

void Mail::setBody(const mail_body_t& body)
{
	this->mBody = body;
}
