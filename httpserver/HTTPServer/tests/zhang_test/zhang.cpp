#include "../test.h"
#ifdef ZHANG_TEST

#include "zhang.h"
#include "../../tools/EncodeUtils.h"
#include "../../mail_cli/mime/MIMEDecoder.h"
#include "../../mail_cli/mime/MIMEDecoder.h"
#include "../../tools/CharsetUtils.h"


void start_zhang()
{
	// pop3_test();
	// utility_test();
	//string_test();
	decode_test();
}


// ���߲���
void utility_test()
{
	const char* text = "zhangjiaji";
	char buf[BUFFER_SIZE];
	int len;
	rstring msg = "Base64 decode ";
	msg += text;
	msg += ": ";
	msg += EncodeUtil::base64_encode((const unsigned char*)text, strlen(text), buf, &len);;
	LogUtil::report(msg);
	LogUtil::report(buf);
	const char* str = "aaa 55";
	_snscanf(str, strlen(str), "%s %d", buf, &len);
	LogUtil::report(buf);
}


// �ַ�������
void string_test()
{
	std::string text = "{{ name }} for brown fox", replace_result;
	std::regex double_brace("(\\{\\{) (.*) (\\}\\})");

	//regex_replace����ֵ��Ϊ�滻����ַ��� 
	replace_result = regex_replace(text, double_brace, "*");
	LogUtil::report(replace_result);
}


// ���빤�߲���
void decode_test()
{
	rstring test;
	MIMEDecoder::decodeWord("=?UTF-8?Q?a?= =?UTF-8?Q?a?= =?UTF-8?Q?a?=", test);
}


// �ַ���ת�����߲���
void charset_test()
{
	using namespace CharsetUtil;

	rwstring wstr(L"��abc��");
	rstring str("��abc��");

	rstring ansi;
	rstring utf8;
	rstring gbk;
	rwstring unicode;

	showHex("unicode", wstr);
	showHex("ansi", str);

	ansi = UnicodeToAnsi(wstr); showHex("ansi", ansi);

	unicode = AnsiToUnicode(ansi); showHex("unicode", unicode);

	utf8 = AnsiToUtf8(str); showHex("utf8", utf8);
	ansi = Utf8ToAnsi(utf8); showHex("ansi", ansi);

	utf8 = UnicodeToUtf8(wstr); showHex("utf8", utf8);
	unicode = Utf8ToUnicode(utf8); showHex("unicode", unicode);

	gbk = Utf8ToGBK(utf8); showHex("gbk", gbk);
	utf8 = GBKToUtf8(gbk); showHex("utf8", utf8);

	getchar();
}

// pop3 ���ܲ���
void pop3_test()
{
	HttpSocket socketInit;

	POP3Client* popcli = new POP3Client();
	rstring mailaddr = TEST_MAIL_ADDR;
	rstring passwd = TEST_MAIL_PASSWD;
	size_t at_pos = mailaddr.find('@');
	rstring usr = mailaddr.substr(0, at_pos);
	rstring at = mailaddr.substr(at_pos + 1);

	/* open and authenticate */
	if (popcli->open(at) && popcli->authenticate(usr, passwd)) {
		LogUtil::report("Done");

		/* capa */
		LogUtil::report("Capabilities: ");
		slist& capas = popcli->getCapabilities();
		for (rstring& s : capas) {
			LogUtil::report(s);
		}

		/* stat */
		size_t mailnum, totsize;
		if (popcli->getStatus(mailnum, totsize)) {
			rstring info = "mails: ";
			info += std::to_string(mailnum) + " total size: " + std::to_string(totsize);

			LogUtil::report(info);
		}

		/* list */
		std::vector<Mail*> mails;
		if (popcli->getMailListWithSize(mails)) {
			for (size_t i = 0; i < mails.size(); ++i) {
				rstring info = "mail ";
				info += std::to_string(i + 1) + ": size " + std::to_string(mails[i]->getSize());

				LogUtil::report(info);
			}
		}

		/* uidl */
		if (popcli->getMailListWithUID(mails)) {
			for (size_t i = 0; i < mails.size(); ++i) {
				rstring info = "mail ";
				info += std::to_string(i + 1) + ": uid " + mails[i]->getUID();

				LogUtil::report(info);
			}
		}

		/* retr */
		if (popcli->retrMail(0, mails[0])) {
			LogUtil::report("Mail 1 subject: ");
			LogUtil::report(mails[0]->getHeader().subject);
			LogUtil::report(mails[0]->getBody().message);
		}
	}
	else {
		std::cout << "Failed";
	}

	delete popcli;
}


#endif // ZHANG_TEST