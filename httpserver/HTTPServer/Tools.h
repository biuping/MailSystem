#pragma once
#include <iostream>
#include <string>
#include "config.h"
#include "http_const.h"

typedef std::string rstring;

class Tools
{
public:
	static void report(rstring info);
};