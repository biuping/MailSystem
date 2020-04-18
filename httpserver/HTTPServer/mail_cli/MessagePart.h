#ifndef _MESSAGE_PART_H_
#define _MESSAGE_PART_H_


#include "ContentTypes.h"
#include <list>


class MessagePart
{
public:
	MessagePart();
	virtual ~MessagePart();

	/* getters */
	const ContentTransferEncoding getEncoding();
	const mail_content_type_t& getContentType();
	const mail_content_disposition& getContentDisposition();
	const rstring& getMessage();
	const std::list<MessagePart*> getParts();
	const int getPartsSize();

	/* setters */
	void setEncoding(ContentTransferEncoding encoding);
	void setContentType(const mail_content_type_t& contenttype);
	void setContentDisposition(const mail_content_disposition& contentDisposition);
	void setMessage(const rstring& message);
	void setParts(const std::list<MessagePart*>& parts);
	void addPartBack(MessagePart*& part);
	void addPartFront(MessagePart*& part);
	void clearParts();

	const rstring& getFileName();
	
	/* convenient type check */
	bool multipart();
	bool isText();
	bool isAttachment();

private:
	ContentTransferEncoding mEncoding;
	mail_content_type_t mContentType;
	mail_content_disposition mContentDisposition;
	rstring mMessage;
	std::list<MessagePart*> mParts;
};



#endif // !_MESSAGE_PART_H_
