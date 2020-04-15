#include "MIMEDecoder.h"
#include "../../tools/LogUtils.h"
#include "../../tools/EncodeUtils.h"
#include "../../tools/CharsetUtils.h"
#include "../../tools/GeneralUtils.h"


const rstring MIMEDecoder::encodedWordPattern = R"(\=\?(\S+?)\?(\w)\?(.+?)\?\=)";
const rstring MIMEDecoder::replacePattern = R"(()" + MIMEDecoder::encodedWordPattern +
											R"()\s+()" + MIMEDecoder::encodedWordPattern + R"())";

void MIMEDecoder::decodeWord(const rstring& encoded, rstring& decoded)
{
	// 正则表达式
	r_regex encodedWordRegex(MIMEDecoder::encodedWordPattern);		// encoded-word 规则
	r_regex replaceRegex(MIMEDecoder::replacePattern);		// 两个空格相间的 encoded-word 规则
	
	// 去掉多个encoded-word之间的空白符
	rstring newEncoded = encoded;
	while (std::regex_search(newEncoded, replaceRegex)) {
		newEncoded = std::regex_replace(newEncoded, replaceRegex, "$1$5");
	}

	// 匹配提取内容
	decoded = encoded;
	r_smatch matches;
	size_t replaceStart = 0;
	while (std::regex_search(newEncoded, matches, encodedWordRegex)) {
		// 搜索到第一个匹配的 encoded-word
		const rstring& fullval = matches.str(0);
		const rstring& charset = matches.str(1);
		const rstring& encoding = matches.str(2);
		const rstring& text = matches.str(3);

		// 替换编码的部分
		rstring decodedText = MIMEDecoder::decode(text, charset, encoding);
		size_t start = GeneralUtil::strReplaceFirst(decoded, fullval, decodedText);

		replaceStart = start + decodedText.size();

		newEncoded = matches.suffix();
	}

	LogUtil::report(decoded);
}


const rstring MIMEDecoder::decode(const rstring& encoded,
	const rstring& charset, const rstring& encoding)
{
	const rstring decoded = encoded;
	
	// decode first
	if (_strnicmp(encoding.c_str(), "B", 1) == 0) {
		// base64 encoding
		char* buf = new char[encoded.size()+1];
		EncodeUtil::base64_decode(encoded.c_str(), encoded.size(), buf);
		
		const rstring decoded = buf;

		delete[] buf;
	}
	else if (_strnicmp(encoding.c_str(), "Q", 1) == 0) {
		// quoted-printable encoding
		
	}
	else {
		// unknown encoding
		return encoded;
	}

	return EncodeUtil::encodeAsciiWithCharset(decoded, charset);
}