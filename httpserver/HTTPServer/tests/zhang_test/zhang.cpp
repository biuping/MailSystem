#include "../test.h"
#include "zhang.h"


void start_zhang()
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
	}
	else {
		std::cout << "Failed";
	}

	delete popcli;
}