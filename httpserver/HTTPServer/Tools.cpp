#include "Tools.h"

void Tools::report(rstring info)
{
	std::cout << info<< std::endl;
}

void Tools::report(rstring info, int error_code)
{
	std::cout << info + std::to_string(error_code) << std::endl;
}
