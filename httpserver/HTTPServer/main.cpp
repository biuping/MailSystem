#include ".\tests\test.h"


int main(int argc, char* argv[])
{
#ifdef _DEBUG
#ifdef TEST_START
	// ���Ի��������п�������
	TEST_START();
#else
	// ���Ի���������ʹ�ò���

#endif // TEST_START
#else
	// ��������

#endif // _DEBUG

}