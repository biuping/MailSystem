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
	const mail_content_disposition_t& getContentDisposition();
	const rstring& getMessage();
	const std::list<MessagePart*> getParts();
	const int getPartsSize();
	const str_kvmap& getParams();
	bool containParam(const rstring& name);
	const rstring& getParam(const rstring& name);

	/* setters */
	void setEncoding(ContentTransferEncoding encoding);
	void setContentType(const mail_content_type_t& contenttype);
	void setContentDisposition(const mail_content_disposition_t& contentDisposition);
	void setMessage(const rstring& message);
	void setParts(const std::list<MessagePart*>& parts);
	void setParams(const str_kvmap& params);
	bool addParam(const rstring& name, const rstring& val);
	bool setParam(const rstring& name, const rstring& val);
	void setOrAddParam(const rstring& name, const rstring& val);
	void addPartBack(MessagePart*& part);
	void addPartFront(MessagePart*& part);
	void clearParts();

	const rstring& getFileName();
	
	/* convenient type check */
	bool isMultipart();
	bool isText();
	bool isAttachment();

private:
	ContentTransferEncoding mEncoding;
	mail_content_type_t mContentType;
	mail_content_disposition_t mContentDisposition;
	rstring mMessage;
	std::list<MessagePart*> mParts;
	str_kvmap mParams;
};



#endif // !_MESSAGE_PART_H_
