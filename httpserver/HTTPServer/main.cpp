#include ".\tests\test.h"


int main(int argc, char* argv[])
{
#ifdef _DEBUG
#ifdef TEST_START
	// 调试环境下运行开发测试
	TEST_START();
#else
	// 调试环境下运行使用测试

#endif // TEST_START
#else
	// 正常运行

#endif // _DEBUG

}