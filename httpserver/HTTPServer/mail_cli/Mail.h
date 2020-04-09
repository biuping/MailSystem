#ifndef _MAIL_H_
#define _MAIL_H_

#include <string>
#include <list>
#include <map>
#include <utility>

typedef std::string rstring;
typedef std::list<rstring> mlist;
typedef std::map<rstring, rstring> str_kvmap;


class Mail
{
public:
	Mail();
	virtual ~Mail();

	/* static methods for parsing */
	static Mail* parse(rstring raw);
	static bool parseHeader(rstring raw, mail_header_t& header);
	static bool parseBody(rstring raw, mail_body_t& body);

	/*  */
	const mail_header_t& getHeader();
	const mail_body_t& getBody();
private:
	mail_header_t mHeader;
	mail_body_t mBody;
};

typedef struct {
	rstring from;
	mlist to;
	rstring subject;
	rstring date;
	rstring content_type;
	str_kvmap xfields;
} mail_header_t;

typedef struct {

} mail_body_t;



#endif // !_MAIL_H_
