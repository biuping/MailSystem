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
	rstring from;
	rstring subject;
	rstring date;
	rstring content_type;
	slist to;
	str_kvmap xfields;		// extended fields
} mail_header_t;

typedef struct {
	rstring message;
} mail_body_t;


class Mail
{
public:
	Mail();
	virtual ~Mail();

	/* static methods for parsing */
	static Mail* parse(rstring raw);
	static bool parseHeader(rstring raw, mail_header_t& header);
	static bool parseBody(rstring raw, mail_body_t& body);

	/* get methods */
	const mail_header_t& getHeader();
	const mail_body_t& getBody();

	void setHeader(mail_header_t header);
	void setBody(mail_body_t body);

	int getSize();


	mail_header_t mHeader;
	mail_body_t mBody;
	int mSize;
};


#endif // !_MAIL_H_
