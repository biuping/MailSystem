#include "../test.h"
#include "../../http/server/HttpServer.h"
#include "huang.h"
#include <iostream>
#include <string>
using namespace std;

void start_huang()
{
	HttpSocket socketInit;
	SMTPClient smtp(
		25,                             /*smtp�˿�*/
		"smtp.whu.edu.cn",                 /*smtp��������ַ*/
		"2017302580306@whu.edu.cn",         /*��������ַ*/
		"wdrs14569",                   /*��������*/
		"1092949763@qq.com",  /*Ŀ�������ַ,��һ�����ÿո�ָ����Ӷ��������*/
		"����",                           /*����*/
		"SMTP��������Ŀ�귢�Ͳ���"      /*�ʼ�����*/
	);

	string filePath("C:\\Users\\hjx\\Desktop\\test.txt");
	smtp.AddAttachment(filePath);
	int err;
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
	system("pause");

}