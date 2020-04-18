#include "MessagePart.h"
#include "../tools/GeneralUtils.h"

MessagePart::MessagePart() : mEncoding(ContentTransferEncoding::SevenBit)
{
}

MessagePart::~MessagePart()
{
	this->clearParts();
}

const ContentTransferEncoding MessagePart::getEncoding()
{
	return mEncoding;
}

const mail_content_type_t& MessagePart::getContentType()
{
	return mContentType;
}

const mail_content_disposition& MessagePart::getContentDisposition()
{
	return mContentDisposition;
}

const rstring& MessagePart::getMessage()
{
	return mMessage;
}

const std::list<MessagePart*> MessagePart::getParts()
{
	return mParts;
}

const int MessagePart::getPartsSize()
{
	return mParts.size();
}

void MessagePart::setEncoding(ContentTransferEncoding encoding)
{
	this->mEncoding = encoding;
}

void MessagePart::setContentType(const mail_content_type_t& contenttype)
{
	this->mContentType = contenttype;
}

void MessagePart::setContentDisposition(const mail_content_disposition& contentDisposition)
{
	this->mContentDisposition = contentDisposition;
}

void MessagePart::setMessage(const rstring& message)
{
	this->mMessage = message;
}

void MessagePart::setParts(const std::list<MessagePart*>& parts)
{
	this->clearParts();
	this->mParts = parts;
}

void MessagePart::addPartBack(MessagePart*& part)
{
	this->mParts.emplace_back(part);
}

void MessagePart::addPartFront(MessagePart*& part)
{
	this->mParts.emplace_front(part);
}

void MessagePart::clearParts()
{
	// 释放原资源
	for (MessagePart* p : mParts) {
		delete p;
	}

	mParts.clear();
}

const rstring& MessagePart::getFileName()
{
	return mContentDisposition.filename.size() == 0 ?
		mContentType.name : mContentDisposition.filename;
}

// 是否为 multipart 类型
bool MessagePart::multipart()
{
	return GeneralUtil::strStartWith(mContentType.media, "multipart/", true);
}

bool MessagePart::isText()
{
	return GeneralUtil::strStartWith(mContentType.media, "text/", true) ||
		(_strnicmp(mContentType.media.c_str(), "message/rfc822", 14) == 0);
}

// 是否为附件类型
bool MessagePart::isAttachment()
{
	return false;
}
