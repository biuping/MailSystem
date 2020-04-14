#include "LogUtils.h"


#ifdef _DEBUG
bool LogUtil::_trace(const rstring& msg)
{
	rstring output = "\n Log ===>" + msg + '\n';
	OutputDebugStringA(output.c_str());

	return true;
}
#endif // _DEBUG


void LogUtil::report(const rstring& msg)
{
	std::cout << msg << std::endl;
	TRACE(msg);
}