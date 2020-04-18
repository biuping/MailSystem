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

	/* complete mail parsing */
	void parseMail(const rstring& raw, Mail* mail);
	
	/* header and body parsing included in mail parsing */
	void parseHeader(const str_citer& begin, const str_citer& end, mail_header_t& header);
	void parseBody(const str_citer& begin, const str_citer& end,
		const mail_header_t& header, mail_body_t& body);

	/* general content related fields parsing */
	void parseContentType(const str_citer& begin, const str_citer& end,
		mail_content_type_t& contentType);
	void parseContentDispostion(const str_citer& begin, const str_citer& end,
		mail_content_disposition& contentDisposition);
	void parseContentTransferEncoding(const str_citer& begin, const str_citer& end,
		ContentTransferEncoding& encoding);
	void parseSize(const str_citer& begin, const str_citer& end, long long& size);

	/* multipart of message in body parsing */
	void parseMessagePart(const rstring& raw, MessagePart* part);
	void parseMessagePartHeader(const str_citer& begin, const str_citer& end, MessagePart* part);
	void parseMessagePartBody(const str_citer& begin, const str_citer& end, MessagePart* part);

private:
	MIMEParser();
	~MIMEParser();

	static MIMEParser* mInstance;
	/* constants */
	const char* illegalChars = "\"*?<>:\\/|";
	const char* tspecials = "()<>@,;:\\/\"[]?=";
	const std::map<rstring, long long> sizeMap{
		std::make_pair(rstring(""), 1LL),
		std::make_pair(rstring("B"), 1LL),
		std::make_pair(rstring("KB"), 1024LL),
		std::make_pair(rstring("MB"), 1024LL * 1024LL),
		std::make_pair(rstring("GB"), 1024LL * 1024LL * 1024LL),
		std::make_pair(rstring("TB"), 1024LL * 1024LL * 1024LL * 1024LL)
	};

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
	void setHeaderContentType(mail_header_t& header, const rstring& contentType);
	void setHeaderContentTransferEncoding(mail_header_t& header, const rstring& encoding);
	void setHeaderContentDisposition(mail_header_t& header, const rstring& disposition);
	/* set non-standard(extended) fields */
	void setOthers(mail_header_t& header, const rstring & key, const rstring & val);

	/* auxiliary functions */
	void parseRfcMailAddrs(const rstring& raw, maddr_list& addrlist);
	void parseSingleMailAddr(const rstring& raw, mail_addr_t& addr);

	void cleanMediaType(rstring& mediatype);
	void stripRfc2045TSpecials(rstring& raw);
	void stripRfc822Ctls(rstring& raw);
	void cleanBody(rstring& rawbody);
	/* body parsing functions */
	void extractParts(const str_citer& begin, const str_citer& end,
		const rstring& boundary, slist& part);
	void parseMessage(const rstring& raw, rstring& message);
	size_t findBoundary(const str_citer& begin, const str_citer& end,
		const rstring& boundary);
	void setPartHeaderField(MessagePart* part, const str_kv_t& field);
};

#endif !_MIME_PARSER_H_