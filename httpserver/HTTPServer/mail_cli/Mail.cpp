#include "Mail.h"


void clearParts(mail_body_t& body)
{
	for (MessagePart* p : body.parts) {
		delete p;
	}
	body.parts.clear();
}


Mail::Mail():mSize(0),mUID("")
{
}

Mail::~Mail()
{
	clearParts(mBody);
}

// 设置邮件大小（bytes）
void Mail::setSize(const size_t mailsize)
{
	this->mSize = mailsize;
}

// 设置邮件UID
void Mail::setUID(const rstring& mailUID)
{
	this->mUID = mailUID;
}

const mail_header_t& Mail::getHeader()
{
	return mHeader;
}

const mail_body_t& Mail::getBody()
{
	return mBody;
}

// 获取邮件解码前字节数（bytes）
const size_t Mail::getSize()
{
	return this->mSize;
}

// 获取邮件UID
const rstring& Mail::getUID()
{
	return this->mUID;
}

// 设置邮件首部
void Mail::setHeader(const mail_header_t& header)
{
	this->mHeader = header;
}

// 设置邮件主体
void Mail::setBody(const mail_body_t& body)
{
	clearParts(mBody);
	this->mBody = body;
}
