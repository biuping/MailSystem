#ifndef _MAIL_H_
#define _MAIL_H_

#include <list>
#include <utility>
#include "ContentTypes.h"
#include "MessagePart.h"


typedef std::list<rstring> slist;


/* �ʼ���ַ���� */
typedef struct {
	rstring name;
	rstring addr;
} mail_addr_t;

typedef std::list<mail_addr_t> maddr_list;

/* �ʼ� Received �ֶ�ֵ���� */
typedef struct {
	rstring date;
	str_kvmap names;
} received_t;

/* �ʼ�ͷ */
typedef struct mail_header_t {
	mail_addr_t from;
	rstring subject;
	rstring date;
	mail_addr_t	reply_to;
	mail_addr_t sender;
	rstring mime_version;
	ContentTransferEncoding content_transfer_encoding;
	mail_content_type_t content_type;
	mail_content_disposition_t content_disposition;
	maddr_list to;
	maddr_list cc;
	maddr_list bcc;
	received_t received;

	// other fields
	str_kvmap xfields;

	mail_header_t() : content_transfer_encoding(ContentTransferEncoding::SevenBit) {}
} mail_header_t;

/* �ʼ��� */
typedef struct {
	rstring message;
	std::list<MessagePart*> parts;
} mail_body_t;


/* body clear tool */
void clearParts(mail_body_t& body);


/* �ʼ��� */
class Mail
{
public:
	Mail();
	virtual ~Mail();

	/* get methods */
	const mail_header_t& getHeader();
	const mail_body_t& getBody();
	const size_t getSize();
	const rstring& getUID();

	/* set methods */
	void setHeader(const mail_header_t& header);
	void setBody(const mail_body_t& body);
	void setSize(const size_t mailsize);
	void setUID(const rstring& mailUID);

	/* convenient type checking */
	bool hasMultipart();
	bool isBodyText();
	
	/* convenient body content getter */
	MessagePart* getFirstPartOfMediaType(const rstring& media);
	void getAllPartsOfMediaType(const rstring& media, std::list<MessagePart*>& parts);
	void getAllAttachmentParts(std::list<MessagePart*>& parts);
	const rstring getFirstPlainTextMessage();
	const rstring getFirstHtmlMessage();
	const rstring getFirstXmlMessage();
	void getAllTextParts(std::list<MessagePart*>& messages);

private:
	mail_header_t mHeader;
	mail_body_t mBody;
	rstring mUID;
	size_t mSize;

};


#endif // !_MAIL_H_
