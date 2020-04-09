#pragma once
#include <iostream>
#include <string>
#include "config.h"
#include "http_const.h"
#include <map>

typedef std::string rstring;
typedef std::map<rstring, rstring> HttpHead_t;
typedef std::pair<rstring, rstring> HttpHeadPair_t;


class Tools
{
public:
	static void report(rstring info);
	static void report(rstring info, int error_code);
};