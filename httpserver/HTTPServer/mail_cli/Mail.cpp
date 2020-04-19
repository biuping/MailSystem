#include "Mail.h"
#include "../tools/GeneralUtils.h"


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

// 判断邮件是否包含多个子部分
bool Mail::hasMultipart()
{
	return GeneralUtil::strStartWith(mHeader.content_type.media, "multipart/", true);
}

// 判断邮件主体是否为文本
bool Mail::isBodyText()
{
	return mHeader.mime_version.size() == 0 ||
		mHeader.content_type.media.size() == 0 ||
		GeneralUtil::strStartWith(mHeader.content_type.media, "text/", true) ||
		(_strnicmp(mHeader.content_type.media.c_str(), "message/rfc822", mHeader.content_type.media.size()) == 0);
}

// 获取指定 media type 的第一个出现的 MessagePart
// media: 媒体类型
MessagePart* Mail::getFirstPartOfMediaType(const rstring& media)
{
	if (this->hasMultipart()) {
		for (MessagePart* p : mBody.parts) {
			MessagePart* found = p->getFirstPartOfMediaType(media);
			if (found != nullptr) {
				return found;
			}
		}
	}

	return nullptr;
}

// 获取所有指定 media type 的 MessagePart
// media: 媒体类型
// parts: MessagePart* list 引用
void Mail::getAllPartsOfMediaType(const rstring& media, std::list<MessagePart*>& parts)
{
	if (this->hasMultipart()) {
		for (MessagePart* p : mBody.parts) {
			p->getAllPartsOfMediaType(media, parts);
		}
	}
}

// 获取邮件所有类型为邮件的子部分
void Mail::getAllAttachmentParts(std::list<MessagePart*>& parts)
{
	if (this->hasMultipart()) {
		for (MessagePart* p : mBody.parts) {
			p->getAllAttachmentParts(parts);
		}
	}
}

// 获取最先出现的 text/plain 类型的信息
// return: plain text 字符串
const rstring Mail::getFirstPlainTextMessage()
{
	const rstring media = "text/plain";

	if (mHeader.content_type.media.compare(media) == 0) {
		return mBody.message;
	}

	MessagePart* p = getFirstPartOfMediaType(media);
	if (p != nullptr) {
		return p->getMessage();
	}

	return "";
}

// 获取最先出现的 html 信息
// return: html 字符串
const rstring Mail::getFirstHtmlMessage()
{
	const rstring media = "text/html";

	if (mHeader.content_type.media.compare(media) == 0) {
		return mBody.message;
	}

	MessagePart* p = getFirstPartOfMediaType(media);
	if (p != nullptr) {
		return p->getMessage();
	}

	return "";
}

// 获取最先出现的 xml 信息
// return: xml 字符串
const rstring Mail::getFirstXmlMessage()
{
	const rstring media = "text/xml";

	if (mHeader.content_type.media.compare(media) == 0) {
		return mBody.message;
	}

	MessagePart* p = getFirstPartOfMediaType(media);
	if (p != nullptr) {
		return p->getMessage();
	}

	return "";
}

// 获取所有的类型为文本的子部分
void Mail::getAllTextParts(std::list<MessagePart*>& parts)
{
	if (this->hasMultipart()) {
		for (MessagePart* p : mBody.parts) {
			p->getAllTextParts(parts);
		}
	}
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
