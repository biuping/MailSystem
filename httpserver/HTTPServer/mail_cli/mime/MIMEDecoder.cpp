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
	// ������ʽ
	r_regex encodedWordRegex(MIMEDecoder::encodedWordPattern);		// encoded-word ����
	r_regex replaceRegex(MIMEDecoder::replacePattern);		// �����ո����� encoded-word ����
	
	// ȥ�����encoded-word֮��Ŀհ׷�
	rstring newEncoded = encoded;
	while (std::regex_search(newEncoded, replaceRegex)) {
		newEncoded = std::regex_replace(newEncoded, replaceRegex, "$1$5");
	}

	// ƥ����ȡ����
	decoded = encoded;
	r_smatch matches;
	size_t replaceStart = 0;
	while (std::regex_search(newEncoded, matches, encodedWordRegex)) {
		// ��������һ��ƥ��� encoded-word
		const rstring& fullval = matches.str(0);
		const rstring& charset = matches.str(1);
		const rstring& encoding = matches.str(2);
		const rstring& text = matches.str(3);

		// �滻����Ĳ���
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