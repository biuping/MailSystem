#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include <string>

#include "../HTTPServer/mail_cli/pop3/POP3Client.h"
#include "../HTTPServer/http/socket/HttpSocket.h"
#include "../HTTPServer/tests/my.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace POP3Testing
{
	TEST_CLASS(POP3Test)
	{
	public:
		HttpSocket* socketInit;
		POP3Client* popcli;
		rstring test_addr;
		rstring test_pass;
		rstring test_usr;
		rstring test_host;
		std::vector<Mail*> mails;
		
		POP3Test()
		{
			socketInit = new HttpSocket();
			popcli = new POP3Client();
			test_addr = TEST_MAIL_ADDR;
			test_pass = TEST_MAIL_PASSWD;
			size_t at_pos = test_addr.find('@');
			test_usr = test_addr.substr(0, at_pos);
			test_host = test_addr.substr(at_pos + 1);

			popcli->open(test_host);
			popcli->authenticate(test_usr, test_pass);

			Logger::WriteMessage("In POP3Test: Initialize WinSock");
		}

		~POP3Test()
		{
			delete socketInit;
			delete popcli;
			for (Mail*& m : mails) {
				delete m;
				m = nullptr;
			}
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
			sscanf_s("sdsdds", "%s", &teststr);
			Logger::WriteMessage(teststr.c_str());

			Assert::AreNotEqual(teststr.size(), (size_t)0);
		}

		/*TEST_METHOD(TEST_CONN_OPEN)
		{
			Assert::IsTrue(popcli->open(test_host));
		}*/

		/*TEST_METHOD(TEST_AUTHENTICATE)
		{
			Assert::IsTrue(popcli->authenticate(test_usr, test_pass));
		}*/

		TEST_METHOD(TEST_CAPA)
		{
			slist& capas = popcli->getCapabilities();
			for (rstring& s : capas) {
				LogUtil::report(s);
				Logger::WriteMessage(s.c_str());
			}

			Assert::IsTrue(capas.size() > 0);
		}

		TEST_METHOD(TEST_ALIVE)
		{
			Assert::IsTrue(popcli->alive());
		}

		TEST_METHOD(TEST_STAT)
		{
			size_t mailnum = 0;
			unsigned long long totsize = 0;
			Assert::IsTrue(popcli->getStatus(mailnum, totsize));
			Assert::IsTrue(mailnum == 4);
			Assert::IsTrue(totsize > 0);

			rstring info = "mails: ";
			info += std::to_string(mailnum) + " total size: " + std::to_string(totsize);

			LogUtil::report(info);
			Logger::WriteMessage(info.c_str());
		}

		TEST_METHOD(TEST_LIST)
		{
			Assert::IsTrue(popcli->getMailListWithSize(mails));
			for (size_t i = 0; i < mails.size(); ++i) {
				rstring info = "mail ";
				info += std::to_string(i + 1) + ": size " + std::to_string(mails[i]->getSize());

				LogUtil::report(info);
				Logger::WriteMessage(info.c_str());
			}
		}

		TEST_METHOD(TEST_UIDLIST)
		{
			Assert::IsTrue(popcli->getMailListWithUID(mails));
			for (size_t i = 0; i < mails.size(); ++i) {
				rstring info = "mail ";
				info += std::to_string(i + 1) + ": uid " + mails[i]->getUID();

				LogUtil::report(info);
				Logger::WriteMessage(info.c_str());
			}
		}

		TEST_METHOD(TEST_RETR)
		{
			for (size_t mailno = 0; mailno < mails.size(); ++mailno) {
				Assert::IsTrue(popcli->retrMail(mailno, mails[mailno]));
			}
		}

		TEST_METHOD(MAIL_PROPERTY_PARSING)
		{
			for (size_t mailno = 0; mailno < mails.size(); ++mailno) {
				LogUtil::report("Mail subject: ");
				rstring sub = mails[mailno]->getHeader().subject;
				LogUtil::report(sub);
				Logger::WriteMessage(sub.c_str());
				Assert::IsTrue(sub.size() > 0);

				std::list<MessagePart*> attachmentParts;
				mails[mailno]->getAllAttachmentParts(attachmentParts);
				for (MessagePart* p : attachmentParts) {
					rstring filename = p->getFileName();
					Assert::IsTrue(filename.size() > 0);
					LogUtil::report("Mail attachment filename: " + filename);
					Logger::WriteMessage(filename.c_str());
					LogUtil::report("Mail attachment filesize: " + p->getContentDisposition().rawsize);
					Logger::WriteMessage(p->getContentDisposition().rawsize.c_str());
				}

				std::list<MessagePart*> textParts;
				mails[mailno]->getAllTextParts(textParts);
				for (MessagePart* p : textParts) {
					rstring text = p->getMessage();
					Assert::IsTrue(text.size() > 0);
					LogUtil::report("Mail text: " + text);
					Logger::WriteMessage(text.c_str());
				}

				LogUtil::report(mails[mailno]->getFirstPlainTextMessage());
				Logger::WriteMessage(mails[mailno]->getFirstPlainTextMessage().c_str());
				LogUtil::report(mails[mailno]->getFirstHtmlMessage());
				Logger::WriteMessage(mails[mailno]->getFirstHtmlMessage().c_str());
			}
		}
	};
}
