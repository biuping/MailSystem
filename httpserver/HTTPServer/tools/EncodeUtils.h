#ifndef _ENCODE_UTILS_H_
#define _ENCODE_UTILS_H_

#include <string>
#include <regex>

typedef std::string rstring;
typedef std::regex r_regex;


namespace EncodeUtil {
	const char* base64_encode(const unsigned char* text, int size, char* buf, int* out_len);
	int base64_decode(const char* text, int size, char* buf);

	void quoted_printable_decode(const rstring& encoded, rstring& decoded, bool variant);

	const rstring convert2UTF8(const rstring& bytes, const rstring& charset);

	static void removeIllegalCtlChars(rstring& raw);
	static void removeCarriageNewLineNotPaired(rstring& raw);
	static void decodeMissingBehindEqual(rstring& decoded);
	static void decodeEqual(rstring& qp);
};


#endif // !_ENCODE_UTILS_H_