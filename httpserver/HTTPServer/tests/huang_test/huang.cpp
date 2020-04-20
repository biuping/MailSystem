#include "../test.h"
#include "../../http/server/HttpServer.h"
#include "huang.h"
#include <iostream>
#include "../../mail_cli/MailClient.h"
#include "../../mail_cli/MailSender.h"
#include <string>
using namespace std;

void start_huang()
{
	HttpSocket socketInit;
	MailClient* mcli = new MailClient();
	
	MailSender* msd = new SMTPClient();

	mcli->setSender(msd);
	string a = "";
	bool aaa = mcli->Login("2017302580306@whu.edu.cn", "wdrs14569", a);

	/*msd->SetContent("aa");
	msd->SetEmailTitle("测试");
	msd->SetTargetEmail("calvin_huang1@qq.com");*/

	vector<Attachment>aa;
	Attachment attahs = Attachment("application / octet - stream", "hello.txt", "123");
	aa.push_back(attahs);
	mcli->SendMail("1092949763@qq.com", "测试", "附件及多目标发送测试", aa);
	int aaaaaa = 0;
	//	25,                             /*smtp端口*/
	//	"smtp.whu.edu.cn",                 /*smtp服务器地址*/
	//	"2017302580306@whu.edu.cn",         /*你的邮箱地址*/
	//	"wdrs14569",                   /*邮箱密码*/
	//	"1092949763@qq.com",  /*目的邮箱地址,这一部分用空格分割可添加多个收信人*/
	//	"测试",                           /*主题*/
	//	"SMTP附件及多目标发送测试"      /*邮件正文*/
	//);
	

	
	

	
	

	
	

	/*int err;
	if ((err = smtp.SendEmail_Ex()) != 0)
	{
		if (err == 1)
			cout << "错误1: 由于网络不畅通，发送失败!" << endl;
		if (err == 2)
			cout << "错误2: 用户名错误,请核对!" << endl;
		if (err == 3)
			cout << "错误3: 用户密码错误，请核对!" << endl;
		if (err == 4)
			cout << "错误4: 请检查附件目录是否正确，以及文件是否存在!" << endl;
	}
	system("pause");*/

}