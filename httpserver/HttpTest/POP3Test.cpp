#include "pch.h"
#include "CppUnitTest.h"
#include "../HTTPServer/mail_cli/pop3/POP3Client.h"
#include <iostream>
#include <string>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace POP3Test
{
	TEST_CLASS(POP3Test)
	{
	public:

		TEST_METHOD(TEST_METHOD1)
		{
			std::string name = "Bill";
			Assert::AreEqual(name.c_str(), "name");
		}
	};
}
