#include "GeneralUtils.h"

// 字符串替换第一个找到的子串为新字符串
// return: 第一次找到子串的位置
size_t GeneralUtil::strReplaceFirst(rstring& whole, const rstring& target, const rstring& newstr)
{
	size_t pos = whole.find(target);
	whole.replace(pos, target.size(), newstr);

	return pos;
}

// 字符串替换所有找到的子串为新字符串
void GeneralUtil::strReplaceAll(rstring& whole, const rstring& target, const rstring& newstr)
{
	size_t start = 0;
	size_t pos = rstring::npos;
	while (start < whole.size() && (pos = whole.find(target, start)) != rstring::npos) {
		whole.replace(pos, target.size(), newstr);
		start = pos + newstr.size();
	}
}

// 字符串分割（引号内无视）
void GeneralUtil::strSplitIgnoreQuoted(const rstring& raw, char split, slist& parts)
{
	parts.clear();

	str_citer begin = raw.begin();
	str_citer end = raw.end();
	size_t start = 0;
	size_t len = 0;

	// 遍历字符并进行分割
	while (begin + start + len < end) {
		if (*(begin + start + len) == split) {
			// encounter char to split at
			parts.push_back(rstring(begin + start, begin + start + len));
			start = start + len + 1;
			len = 0;
			continue;
		} else if (*(begin + start + len) == '\"') {
			// inside quote
			while (begin + start + len < end && *(begin + start + len) != '\"') {
				++len;
			}

			if (begin + start + len == end) {
				break;
			}
		}

		++len;
	}

	// 剩余字符
	if (len > 0) {
		parts.push_back(rstring(begin + start, begin + start + len));
	}
}

// 
void GeneralUtil::strTrim(rstring& str)
{
	str.erase(0, str.find_first_not_of(GeneralUtil::whitespaces));
	str.erase(str.find_last_not_of(GeneralUtil::whitespaces)+1);
}

// 去除一对引号
void GeneralUtil::strRemoveQuotes(rstring& str)
{
	// 检查字符数以及首尾引号
	if (str.size() > 1 && *str.begin() == '"' && *(str.end() - 1) == '"') {
		str = str.substr(1, str.size() - 2);
	}

	// 否则不进行处理
}

// 检查字符串是否以某子串结尾
bool GeneralUtil::strEndsWith(const rstring& source, const rstring& suffix, bool caseIgnore)
{
	if (source.size() < suffix.size()) {
		return false;
	}

	for (int sr = source.size() - 1, su = suffix.size() - 1;
		sr >= 0 && su >= 0;
		sr--, su--) {
		char src = caseIgnore ? tolower(source[sr]) : source[sr];
		char suc = caseIgnore ? tolower(suffix[su]) : suffix[su];
		if (src != suc) {
			return false;
		}
	}

	return true;
}

bool GeneralUtil::strEndsWith(const str_citer& begin, const str_citer& end, const rstring& suffix, bool caseIgnore)
{
	if (begin + suffix.size() >= end) {
		return false;
	}

	for (int sr = 1, su = suffix.size() - 1;
		(size_t)sr + begin < end && su >= 0;
		++sr, --su) {
		char src = caseIgnore ? tolower(*(end - sr)) : *(end - sr);
		char suc = caseIgnore ? tolower(suffix[su]) : suffix[su];
		if (src != suc) {
			return false;
		}
	}

	return true;
}

bool GeneralUtil::strStartWith(const rstring& source, const rstring& prefix, bool caseIgnore)
{
	if (source.size() < prefix.size()) {
		return false;
	}

	int srcsize = source.size();
	int presize = prefix.size();
	for (int sr = 0, pr = 0;
		sr < srcsize && pr < presize;
		++sr, ++pr) {
		char src = caseIgnore ? tolower(source[sr]) : source[sr];
		char prc = caseIgnore ? tolower(prefix[pr]) : prefix[pr];
		if (src != prc) {
			return false;
		}
	}

	return true;
}

bool GeneralUtil::strStartWith(const str_citer& begin, const str_citer& end, const rstring& prefix, bool caseIgnore)
{
	if (begin + prefix.size() >= end) {
		return false;
	}

	for (int sr = 0, su = 0;
		(size_t)sr + begin < end && su >= 0;
		++sr, ++su) {
		char src = caseIgnore ? tolower(*(begin + sr)) : *(begin + sr);
		char suc = caseIgnore ? tolower(prefix[su]) : prefix[su];
		if (src != suc) {
			return false;
		}
	}

	return true;
}

size_t GeneralUtil::strFindLineEnd(const str_citer& begin, const str_citer& end)
{
	size_t len = 0;
	while (begin + len < end) {
		if (*(begin + len) == '\r' && begin + len + 1 < end && *(begin + len + 1) == '\n') {
			break;
		}
		++len;
	}

	return begin + len < end ? len : rstring::npos;
}

// 跳过空白字符
// begin: 字符串开始位置const_iterator
// end: 字符串结束位置const_iterator
// return: 跳过的字符数
size_t GeneralUtil::strSkipWhiteSpaces(const str_citer& begin, const str_citer& end)
{
	size_t tot = 0;
	// skip white spaces
	while (begin < end && strchr(whitespaces, *(begin + tot))) {
		++tot;
	}
	return tot;
}

// 去除字符串中的某些字符
// str: 被修改的字符串
// char: 要去除的字符集合
size_t GeneralUtil::strStripCharsIn(rstring& str, const rstring& chars)
{
	size_t k = 0;
	size_t len = str.size();
	for (size_t i = 0; i < len; ++i) {
		if (strchr(chars.c_str(), str[i])) {
			// 包含要去除的字符
			++k;
			continue;
		}

		str[i - k] = str[i];
	}
	str.erase(str.size() - k);

	return k;
}

// 去除字符串结尾的某些字符
// str: 被修改的字符串
// strips: 要去除的字符的集合
size_t GeneralUtil::strStripCharsInEnding(rstring& str, const rstring& strips)
{
	size_t pos = str.find_last_not_of(strips);
	if (pos != rstring::npos && pos < str.size()-1) {
		size_t stripLen = str.size() - 1 - pos;
		str.erase(pos + 1);
		return stripLen;
	}

	return 0;
}

bool GeneralUtil::strEquals(const rstring& str1, const rstring& str2, bool caseIgnore)
{
	size_t len1 = str1.size();
	size_t len2 = str2.size();
	size_t len = len1 > len2 ? len1 : len2;
	
	if (caseIgnore) {
		return _strnicmp(str1.c_str(), str2.c_str(), len) == 0;
	}
	else {
		return strncmp(str1.c_str(), str2.c_str(), len) == 0;
	}
}
