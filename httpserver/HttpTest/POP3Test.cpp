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

		POP3Test()
		{
			Logger::WriteMessage("In POP3Test");
		}

		~POP3Test()
		{
			Logger::WriteMessage("In ~POP3Test");
		}

		TEST_CLASS_INITIALIZE(POP3Initialize)
		{
			Logger::WriteMessage("POP3 Intialize");
			httpSocket = new HttpSocket();
		}

		TEST_CLASS_CLEANUP(POP3Cleanup)
		{
			Logger::WriteMessage("POP3 Cleanup");
			delete httpSocket;
		}
		TEST_METHOD(TEST_METHOD1)
		{
			POP3Client client;
			rstring msg = "Auth State: "+client.Authorization;
			Logger::WriteMessage(msg.c_str());
		}

	private:
		static HttpSocket* httpSocket;
	};
}
