#ifndef _MIME_PARSER_H_
#define _MIME_PARSER_H_

#include "../Mail.h"
#include <utility>

typedef std::pair<rstring, rstring> kv_t;
typedef rstring::const_iterator str_citer;


class MIMEParser
{
public:
	static MIMEParser* instance();

	void parseMail(const rstring& raw, Mail* mail);
	
	void parseHeader(const str_citer& begin, const str_citer& end, mail_header_t& header);
	void parseBody(const str_citer& begin, const str_citer& end, mail_body_t& body);

private:
	MIMEParser();
	~MIMEParser();

	static MIMEParser* mInstance;
	const char* illegalChars = "\"*?<>:\\/|";
	const char* whitespaces = " \t";

	size_t skipWhiteSpaces(const str_citer& begin, const str_citer& end);

	size_t extractField(const str_citer& begin, const str_citer& end, kv_t& field);
	void setHeaderField(mail_header_t& header, const kv_t& field);

	/* header fields setters */
	/* set specific fields */
	void setSubject(mail_header_t& header, const rstring& subject);
	void setDate(mail_header_t& header, const rstring& date);
	void setFrom(mail_header_t& header, const rstring& from);
	void setTo(mail_header_t& header, const rstring& to);
	/* set non-standard(extended) fields */
	void setOthers(mail_header_t& header, const rstring & key, const rstring & val);
};

#endif !_MIME_PARSER_H_