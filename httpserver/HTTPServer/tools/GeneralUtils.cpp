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
