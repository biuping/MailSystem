#ifndef _GENERAL_UTILS_H_
#define _GENERAL_UTILS_H_

#include <string>
#include <list>

typedef std::string rstring;
typedef std::list<rstring> slist;
typedef rstring::const_iterator str_citer;


namespace GeneralUtil {
	size_t strReplaceFirst(rstring& whole, const rstring& target, const rstring& newstr);
	void strReplaceAll(rstring& whole, const rstring& target, const rstring& newstr);
	void strSplitIgnoreQuoted(const rstring& raw, char split, slist& parts);
	void strTrim(rstring& str);
	void strRemoveQuotes(rstring& str);
	bool strEndsWith(const rstring& source, const rstring& suffix, bool caseIgnore=false);
	bool strEndsWith(const str_citer& begin, const str_citer& end, 
		const rstring& suffix, bool caseIgnore = false);
	bool strStartWith(const rstring& source, const rstring& prefix, bool caseIgnore = false);
	bool strStartWith(const str_citer& begin, const str_citer& end,
		const rstring& prefix, bool caseIgnore = false);
	size_t strFindLineEnd(const str_citer& begin, const str_citer& end);
	size_t strSkipWhiteSpaces(const str_citer& begin, const str_citer& end);
	size_t strStripCharsIn(rstring& str, const rstring& chars);

	static const char* whitespaces = " \f\v\t\r\n";
};


#endif // !_GENERAL_UTILS_H_

