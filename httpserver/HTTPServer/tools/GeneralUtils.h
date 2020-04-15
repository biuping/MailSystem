#ifndef _GENERAL_UTILS_H_
#define _GENERAL_UTILS_H_

#include <string>

typedef std::string rstring;


namespace GeneralUtil {
	size_t strReplaceFirst(rstring& whole, const rstring& target, const rstring& newstr);
};


#endif // !_GENERAL_UTILS_H_

