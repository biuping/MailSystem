#ifndef _MIME_DECODER_H_
#define _MIME_DECODER_H_

#include <string>
#include <regex>

typedef std::string rstring;
typedef std::regex r_regex;
typedef std::smatch r_smatch;
typedef r_smatch::const_iterator sm_iter;

class MIMEDecoder
{
public:
	static void decodeWord(const rstring& encoded, rstring& decoded);
	static const rstring decode(const rstring& encoded, const rstring& charset, const rstring& encoding);

private:
	static const rstring encodedWordPattern;
	static const rstring replacePattern;
};



#endif // !_MIME_DECODER_H_
