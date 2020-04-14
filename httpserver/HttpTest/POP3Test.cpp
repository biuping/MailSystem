#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include <string>

#include "../HTTPServer/mail_cli/pop3/POP3Client.h"
#include "../HTTPServer/http/socket/HttpSocket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace POP3Testing
{
	TEST_CLASS(POP3Test)
	{
	public:
		HttpSocket* socketInit;
		
		POP3Test()
		{
			socketInit = new HttpSocket();
			Logger::WriteMessage("In POP3Test: Initialize WinSock");
		}

		~POP3Test()
		{
			delete socketInit;
			Logger::WriteMessage("In ~POP3Test: Cleanup WinSock");
		}

		TEST_CLASS_INITIALIZE(POP3Initialize)
		{
			Logger::WriteMessage("POP3 Intialize");
		}

		TEST_CLASS_CLEANUP(POP3Cleanup)
		{
			Logger::WriteMessage("POP3 Cleanup");
		}

		TEST_METHOD(TEST_ALIVE_DIRECTLY_CALLED)
		{
			POP3Client client;
			rstring msg = "client alive: ";
			bool alive = client.alive();
			msg.append(alive ? "true" : "false");
			Logger::WriteMessage(msg.c_str());

			Assert::IsFalse(alive);
		}

		TEST_METHOD(TEST_STRING_SCANF)
		{
			rstring teststr;
			sscanf("sdsdds", "%s", &teststr);
			Logger::WriteMessage(teststr.c_str());

			Assert::AreNotEqual(teststr.size(), (size_t)0);
		}
	};
}
