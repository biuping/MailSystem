#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include <string>

#include "../HTTPServer/mail_cli/pop3/POP3Client.h"
#include "../HTTPServer/http/socket/HttpSocket.h"
#include "../HTTPServer/tests/my.h"
#include "../HTTPServer/mail_cli/MailClient.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MailClientTest
{
	TEST_CLASS(MailClientTest)
	{
	public:
		HttpSocket* socketInit;
		MailClient* mailcli;
		rstring test_addr;
		rstring test_pass;

		MailClientTest()
		{
			socketInit = new HttpSocket();
			mailcli = new MailClient();
			mailcli->setReceiver(new POP3Client());

			test_addr = TEST_MAIL_ADDR;
			test_pass = TEST_MAIL_PASSWD;

			rstring desc;
			Assert::IsTrue(mailcli->Login(test_addr, test_pass, desc));
			Logger::WriteMessage(desc.c_str());
			LogUtil::report(desc);

			Logger::WriteMessage("In MailClientTest: Initialize WinSock");
		}

		~MailClientTest()
		{
			delete socketInit;
			delete mailcli;
			Logger::WriteMessage("In ~MailClientTest: Cleanup WinSock");
		}

		TEST_CLASS_INITIALIZE(POP3Initialize)
		{
			Logger::WriteMessage("MailClientTest Intialize");
		}

		TEST_CLASS_CLEANUP(POP3Cleanup)
		{
			Logger::WriteMessage("MailClientTest Cleanup");
		}

		/*TEST_METHOD(TEST_LOGIN)
		{
			rstring desc;
			Assert::IsTrue(mailcli->Login(test_addr, test_pass, desc));
			Logger::WriteMessage(desc.c_str());
			LogUtil::report(desc);
		}*/

		TEST_METHOD(TEST_RECV)
		{
			rstring resp = mailcli->RecvMail();
			Assert::IsTrue(resp.size() > 0);
			Logger::WriteMessage(resp.c_str());
			LogUtil::report(resp);
		}

		TEST_METHOD(TEST_DOWNLOAD_ATTACH)
		{
			rstring att;
			Json::Value attObj = mailcli->DownloadAttach("1tbiAQcHBFmj3-i-OgAPsS", 0);
			Tools::json_write(attObj, att, true);
			Assert::IsTrue(att.size() > 0);
			LogUtil::report(att);
			Logger::WriteMessage(att.c_str());
		}
	};
}