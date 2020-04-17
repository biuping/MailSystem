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

	static const char* whitespaces = " \f\v\t\r\n";
};


#endif // !_GENERAL_UTILS_H_

