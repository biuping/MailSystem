#ifndef _MAIL_H_
#define _MAIL_H_

#include <list>
#include <utility>
#include "ContentTypes.h"
#include "MessagePart.h"


typedef std::list<rstring> slist;


/* 邮件地址类型 */
typedef struct {
	rstring name;
	rstring addr;
} mail_addr_t;

typedef std::list<mail_addr_t> maddr_list;

/* 邮件 Received 字段值类型 */
typedef struct {
	rstring date;
	str_kvmap names;
} received_t;

/* 邮件头 */
typedef struct mail_header_t {
	mail_addr_t from;
	rstring subject;
	rstring date;
	mail_addr_t	reply_to;
	mail_addr_t sender;
	rstring mime_version;
	ContentTransferEncoding content_transfer_encoding;
	mail_content_type_t content_type;
	mail_content_disposition content_disposition;
	maddr_list to;
	maddr_list cc;
	maddr_list bcc;
	received_t received;

	// other fields
	str_kvmap xfields;

	mail_header_t() : content_transfer_encoding(ContentTransferEncoding::SevenBit) {}
} mail_header_t;

/* 邮件体 */
typedef struct {
	rstring message;
	std::list<MessagePart*> parts;
} mail_body_t;


/* 邮件类 */
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

private:
	mail_header_t mHeader;
	mail_body_t mBody;
	rstring mUID;
	size_t mSize;

	void clearParts();
};


#endif // !_MAIL_H_
