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
}

void MIMEDecoder::decodeMailBody(const rstring& encoded, const rstring& charset,
	ContentTransferEncoding encoding, rstring& decoded)
{
	switch (encoding)
	{
	case ContentTransferEncoding::QuotedPrintable:
		quotedPrintableDecode(encoded, decoded, false);
		break;
	case ContentTransferEncoding::Base64:
		base64Decode(encoded, decoded);
		break;
	case ContentTransferEncoding::SevenBit:
	case ContentTransferEncoding::EightBit:
	case ContentTransferEncoding::Binary:
	default:
		// 7bit, 8bit, binary ����Ҫ���ж��⴦��
		decoded = encoded;
		break;
	}

	decoded = EncodeUtil::encodeAsciiWithCharset(decoded, charset);
}


const rstring MIMEDecoder::decode(const rstring& encoded,
	const rstring& charset, const rstring& encoding)
{
	rstring decoded = encoded;
	
	// decode first
	if (GeneralUtil::strEquals(encoding.c_str(), "B", true)) {
		// base64 encoding
		base64Decode(encoded, decoded);
	}
	else if (GeneralUtil::strEquals(encoding.c_str(), "Q", true)) {
		// quoted-printable encoding
		quotedPrintableDecode(encoded, decoded, true);
	}
	// else unknow encoding

	return decoded;
	//return EncodeUtil::encodeAsciiWithCharset(decoded, charset);
}


void MIMEDecoder::rfc2231Decode(const rstring& raw, str_kvlist & kvs)
{
	// nomarlize, ���ֺ���ʧ
// �� text/plain; charset=\"iso-8859-1\" name=\"somefile.txt\"
// �Լ� attachment filename=\"foo\"
	rstring encoded = std::regex_replace(raw, r_regex("=\\s*\"([^\"]*)\"\\s"), "=\"$1\"; ");
	encoded = std::regex_replace(encoded, r_regex(R"(^([^;\s]+)\s([^;\s]+))"), "$1; $2");
	// split by ;
	GeneralUtil::strTrim(encoded);
	slist splitted;
	GeneralUtil::strSplitIgnoreQuoted(encoded, ';', splitted);

	kvs.clear();
	for (rstring s : splitted) {
		GeneralUtil::strTrim(s);
		if (s.size() == 0) {
			continue;
		}

		size_t eqpos = s.find('=');
		if (eqpos == rstring::npos) {
			// ֱ�����
			kvs.emplace_back("", s);
		}
		else {
			// = ǰΪ key��= ��Ϊvalue
			rstring key = s.substr(0, eqpos);
			rstring val = s.substr(eqpos + 1);
			GeneralUtil::strTrim(key);
			GeneralUtil::strTrim(val);
			kvs.emplace_back(key, val);
		}
	}


	MIMEDecoder::rfc2231DecodePairs(kvs);
}

// base64 ����
void MIMEDecoder::base64Decode(const rstring& encoded, rstring& decoded)
{
	// ȥ���Ƿ��Ŀհ��ַ�
	decoded = encoded;
	GeneralUtil::strReplaceAll(decoded, "\r\n", "");
	GeneralUtil::strStripCharsIn(decoded, "\t ");

	// ʹ�ù��߽���
	char* buf = new char[decoded.size() + 1];
	ZeroMemory(buf, sizeof(char) * (decoded.size() + 1));
	EncodeUtil::base64_decode(decoded.c_str(), decoded.size(), buf);

	decoded = buf;
	delete[] buf;
}

void MIMEDecoder::quotedPrintableDecode(const rstring& encoded, rstring& decoded, bool variant)
{
	EncodeUtil::quoted_printable_decode(encoded, decoded, variant);
}

void MIMEDecoder::rfc2231DecodePairs(str_kvlist& kvs)
{
	for (str_kvlist::iterator ite = kvs.begin(); ite != kvs.end(); ++ite) {
		rstring& key = ite->first;
		rstring& val = ite->second;
		GeneralUtil::strRemoveQuotes(val);

		if (GeneralUtil::strEndsWith(key, "*0") || GeneralUtil::strEndsWith(key, "*0*")) {
			rstring encoding = "notEncoded";
			if (GeneralUtil::strEndsWith(key, "*0*")) {
				// encoded
				// ���룬ȡ��encoding�������Ϲ淶��encoding��Ϊ""
				MIMEDecoder::rfc2231DecodeSingle(val, encoding);
				// ȥ����ʼ�ı�ʾcontinuation��*0���Լ������ʾencoded��*
				GeneralUtil::strReplaceAll(key, "*0*", "");
			}
			else {
				// not encoded
				GeneralUtil::strReplaceAll(key, "*0", "");
			}

			// �鿴֮���Ϊcontinuation���ֵ�key
			int count = 1;
			// list �������޷� + 1��ֻ��˳����ʣ�ʹ��++��
			str_kvlist::iterator iter = iter;
			++iter;
			while(iter != kvs.end()) {
				rstring& nxtKey = iter->first;
				rstring& nxtVal = iter->second;
				GeneralUtil::strRemoveQuotes(nxtVal);

				rstring tempKey = key + "*" + std::to_string(count);
				if (nxtKey.compare(tempKey) == 0) {
					// not encoded value
					val.append(nxtVal);
				}
				else if (nxtKey.compare(tempKey + "*") == 0) {
					// ��һ����not encoded
					if (encoding.size() != 0) {
						// ���ʵ���Ƿ����
						MIMEDecoder::rfc2231DecodeSingle(nxtVal, encoding);
					}
					val.append(nxtVal);
				}
				else {
					// continuation ����
					break;
				}

				// ɾ��
				kvs.erase(iter++);
				++count;
			}
		}
		else if (GeneralUtil::strEndsWith(key, "*")) {
			// not part of continuation
			// ȥ�� key* �е� *
			GeneralUtil::strReplaceAll(key, "*", "");
			// decode value
			rstring throwAway;
			MIMEDecoder::rfc2231DecodeSingle(val, throwAway);
		}
		// else normal key, no changes needed
	}
}

void MIMEDecoder::rfc2231DecodeSingle(rstring& encoded, rstring& encoding)
{
	size_t start = encoded.find('\'');
	if (start == rstring::npos) {
		// ������encoding����
		encoding = "";
	}
	else {
		encoding = encoded.substr(0, start);
		size_t end = encoded.rfind('\'');
		encoded = encoded.substr(end + 1);

		// ����ʹ�� QuotedPrintable
		// �� % ��Ϊ =
		GeneralUtil::strReplaceAll(encoded, "%", "=");
		const rstring encodedWord = "=?" + encoding + "?Q?" + encoded + "?=";
		// ���룬���encoded
		MIMEDecoder::decodeWord(encodedWord, encoded);
	}
}
