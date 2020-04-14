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
	str_kvmap xfields;		// other extended fields
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
