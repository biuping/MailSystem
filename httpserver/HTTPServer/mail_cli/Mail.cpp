#include "Mail.h"


Mail::Mail():mSize(0),mUID("")
{
}

Mail::~Mail()
{
}

void Mail::setSize(const size_t mailsize)
{
	this->mSize = mailsize;
}

void Mail::setUID(const rstring& mailUID)
{
	this->mUID = mailUID;
}
