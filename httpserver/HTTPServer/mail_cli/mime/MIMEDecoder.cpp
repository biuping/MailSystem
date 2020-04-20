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
		// 7bit, 8bit, binary 不需要进行额外处理
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
	// nomarlize, 检查分号遗失
// 如 text/plain; charset=\"iso-8859-1\" name=\"somefile.txt\"
// 以及 attachment filename=\"foo\"
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
			// 直接添加
			kvs.emplace_back("", s);
		}
		else {
			// = 前为 key，= 后为value
			rstring key = s.substr(0, eqpos);
			rstring val = s.substr(eqpos + 1);
			GeneralUtil::strTrim(key);
			GeneralUtil::strTrim(val);
			kvs.emplace_back(key, val);
		}
	}


	MIMEDecoder::rfc2231DecodePairs(kvs);
}

// base64 解码
void MIMEDecoder::base64Decode(const rstring& encoded, rstring& decoded)
{
	// 去除非法的空白字符
	decoded = encoded;
	GeneralUtil::strReplaceAll(decoded, "\r\n", "");
	GeneralUtil::strStripCharsIn(decoded, "\t ");

	// 使用工具解码
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
				// 解码，取出encoding，不符合规范则encoding设为""
				MIMEDecoder::rfc2231DecodeSingle(val, encoding);
				// 去除开始的表示continuation的*0，以及后面表示encoded的*
				GeneralUtil::strReplaceAll(key, "*0*", "");
			}
			else {
				// not encoded
				GeneralUtil::strReplaceAll(key, "*0", "");
			}

			// 查看之后的为continuation部分的key
			int count = 1;
			// list 迭代器无法 + 1（只能顺序访问，使用++）
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
					// 第一部分not encoded
					if (encoding.size() != 0) {
						// 检查实际是否编码
						MIMEDecoder::rfc2231DecodeSingle(nxtVal, encoding);
					}
					val.append(nxtVal);
				}
				else {
					// continuation 结束
					break;
				}

				// 删除
				kvs.erase(iter++);
				++count;
			}
		}
		else if (GeneralUtil::strEndsWith(key, "*")) {
			// not part of continuation
			// 去除 key* 中的 *
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
		// 无描述encoding部分
		encoding = "";
	}
	else {
		encoding = encoded.substr(0, start);
		size_t end = encoded.rfind('\'');
		encoded = encoded.substr(end + 1);

		// 编码使用 QuotedPrintable
		// 将 % 改为 =
		GeneralUtil::strReplaceAll(encoded, "%", "=");
		const rstring encodedWord = "=?" + encoding + "?Q?" + encoded + "?=";
		// 解码，输出encoded
		MIMEDecoder::decodeWord(encodedWord, encoded);
	}
}
