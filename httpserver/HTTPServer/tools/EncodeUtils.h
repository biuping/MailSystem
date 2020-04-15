#ifndef _ENCODE_UTILS_H_
#define _ENCODE_UTILS_H_

#include <string>

typedef std::string rstring;


namespace EncodeUtil {
	const char* base64_encode(const unsigned char* text, int size, char* buf, int* out_len);
	int base64_decode(const char* text, int size, char* buf);

	const rstring encodeAsciiWithCharset(const rstring& asciitext, const rstring& charset);
};


#endif // !_ENCODE_UTILS_H_