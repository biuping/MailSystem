#ifndef _MAIL_H_
#define _MAIL_H_

#include <string>
#include <list>

typedef std::string rstring;
typedef std::list<rstring> mlist;

class Mail
{
public:
	Mail();
	virtual ~Mail();

	static Mail* parse(rstring raw);

private:
	mail_header_t mHeader;
	mail_body_t mBody;
};

typedef struct {
	rstring from;
	mlist to;
	rstring subject;
	rstring date;
} mail_header_t;

typedef struct {

} mail_body_t;



#endif // !_MAIL_H_
