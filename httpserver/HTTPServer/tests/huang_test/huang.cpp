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
	msd->SetEmailTitle("����");
	msd->SetTargetEmail("calvin_huang1@qq.com");*/

	vector<Attachment>aa;
	Attachment attahs = Attachment("application / octet - stream", "hello.txt", "123");
	aa.push_back(attahs);
	mcli->SendMail("1092949763@qq.com", "����", "��������Ŀ�귢�Ͳ���", aa);
	int aaaaaa = 0;
	//	25,                             /*smtp�˿�*/
	//	"smtp.whu.edu.cn",                 /*smtp��������ַ*/
	//	"2017302580306@whu.edu.cn",         /*��������ַ*/
	//	"wdrs14569",                   /*��������*/
	//	"1092949763@qq.com",  /*Ŀ�������ַ,��һ�����ÿո�ָ����Ӷ��������*/
	//	"����",                           /*����*/
	//	"SMTP��������Ŀ�귢�Ͳ���"      /*�ʼ�����*/
	//);
	

	
	

	
	

	
	

	/*int err;
	if ((err = smtp.SendEmail_Ex()) != 0)
	{
		if (err == 1)
			cout << "����1: �������粻��ͨ������ʧ��!" << endl;
		if (err == 2)
			cout << "����2: �û�������,��˶�!" << endl;
		if (err == 3)
			cout << "����3: �û����������˶�!" << endl;
		if (err == 4)
			cout << "����4: ���鸽��Ŀ¼�Ƿ���ȷ���Լ��ļ��Ƿ����!" << endl;
	}
	system("pause");*/

}