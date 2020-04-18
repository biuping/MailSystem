#ifndef _MIME_DECODER_H_
#define _MIME_DECODER_H_

#include <string>
#include <regex>
#include <map>
#include <list>
#include <utility>
#include "../Mail.h"

typedef std::string rstring;
typedef std::regex r_regex;
typedef std::smatch r_smatch;
typedef r_smatch::const_iterator sm_iter;
typedef std::list<std::pair<rstring, rstring>> str_kvlist;


class MIMEDecoder
{
public:
	static void decodeWord(const rstring& encoded, rstring& decoded);
	static void decodeMailBody(const rstring& encoded, const rstring& charset,
		ContentTransferEncoding encoding, rstring& decoded);
	static const rstring decode(const rstring& encoded, const rstring& charset, const rstring& encoding);
	static void rfc2231Decode(const rstring& raw, str_kvlist& kvs);
	static void base64Decode(const rstring& encoded, rstring& decoded);
	static void quotedPrintableDecode(const rstring& encoded, rstring& decoded, bool variant);
private:
	static const rstring encodedWordPattern;
	static const rstring replacePattern;
	static void rfc2231DecodePairs(str_kvlist& kvs);
	static void rfc2231DecodeSingle(rstring& encoded, rstring & encoding);
};



#endif // !_MIME_DECODER_H_
