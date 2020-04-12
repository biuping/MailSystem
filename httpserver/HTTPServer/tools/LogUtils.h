#ifndef _LOG_UTILS_H_
#define _LOG_UTILS_H_

#include <string>
#include <iostream>
#include <Windows.h>

typedef std::string rstring;

namespace LogUtil {
#ifdef _DEBUG
	bool _trace(const rstring& msg);
#define TRACE _trace
#else
#define TRACE false && _trace
#endif

	void report(const rstring& msg);
}

#endif // !_LOG_UTILS_H_

