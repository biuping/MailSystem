#ifndef _MAIL_H_
#define _MAIL_H_

#include <string>
#include <list>
#include <map>
#include <utility>

typedef std::string rstring;
typedef std::list<rstring> slist;
typedef std::map<rstring, rstring> str_kvmap;


typedef struct {
	rstring name;
	rstring addr;
} mail_addr_t;

typedef std::list<mail_addr_t> maddr_list;

typedef struct {
	rstring media;
	rstring boundary;
	rstring charset;
	rstring name;
	str_kvmap params;
} mail_content_type_t;

typedef struct {
	rstring date;
	str_kvmap names;
} received_t;

typedef struct {
	mail_addr_t from;
	rstring subject;
	rstring date;
	mail_addr_t	reply_to;
	mail_addr_t sender;
	rstring mime_version;
	rstring content_transfer_encoding;
	mail_content_type_t content_type;
	maddr_list to;
	maddr_list cc;
	maddr_list bcc;
	received_t received;

	// other fields
	str_kvmap xfields;
} mail_header_t;

typedef struct {
	rstring message;
} mail_body_t;


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
};


#endif // !_MAIL_H_
