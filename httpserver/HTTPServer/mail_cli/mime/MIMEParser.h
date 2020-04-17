#ifndef _MIME_PARSER_H_
#define _MIME_PARSER_H_

#include "../Mail.h"
#include <utility>

typedef std::pair<rstring, rstring> str_kv_t;
typedef rstring::const_iterator str_citer;


class MIMEParser
{
public:
	static MIMEParser* instance();

	void parseMail(const rstring& raw, Mail* mail);
	
	void parseHeader(const str_citer& begin, const str_citer& end, mail_header_t& header);
	void parseBody(const str_citer& begin, const str_citer& end,
		const mail_header_t& header, mail_body_t& body);

private:
	MIMEParser();
	~MIMEParser();

	static MIMEParser* mInstance;
	/* constants */
	const char* illegalChars = "\"*?<>:\\/|";
	const char* whitespaces = " \t\v\f\r\n";
	const char* tspecials = "()<>@,;:\\/\"[]?=";

	size_t skipWhiteSpaces(const str_citer& begin, const str_citer& end);

	/* header parsing functions */
	size_t extractField(const str_citer& begin, const str_citer& end, str_kv_t& field);
	void setHeaderField(mail_header_t& header, const str_kv_t& field);
	/* header fields setters(responsible for specific parsing) */
	/* set specific fields */
	void setSubject(mail_header_t& header, const rstring& subject);
	void setDate(mail_header_t& header, const rstring& date);
	void setFrom(mail_header_t& header, const rstring& from);
	void setReplyTo(mail_header_t& header, const rstring& replyto);
	void setSender(mail_header_t& header, const rstring& sender);
	void setReceived(mail_header_t& header, const rstring& received);
	void setMimeVersion(mail_header_t& header, const rstring& version);
	void setTo(mail_header_t& header, const rstring& to);
	void setCc(mail_header_t& header, const rstring& cc);
	void setBcc(mail_header_t& header, const rstring& bcc);
	void setContentType(mail_header_t& header, const rstring& contentType);
	void setContentTransferEncoding(mail_header_t& header, const rstring& encoding);
	/* set non-standard(extended) fields */
	void setOthers(mail_header_t& header, const rstring & key, const rstring & val);

	void parseRfcMailAddrs(const rstring& raw, maddr_list& addrlist);
	void parseSingleMailAddr(const rstring& raw, mail_addr_t& addr);

	void cleanMediaType(rstring& mediatype);
	void stripRfc2045TSpecials(rstring& raw);
	void stripRfc822Ctls(rstring& raw);

	/* body parsing functions */
	void extractParts(const str_citer& begin, const str_citer& end,
		const rstring& boundary, slist& part);
	void parseMessage(const rstring& raw, rstring& message);
};

#endif !_MIME_PARSER_H_