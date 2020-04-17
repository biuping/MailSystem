#include "../test.h"
#include "huang.h"
#include <iostream>
using namespace std;

void start_huang()
{
	SMTPClient * smtp=new  SMTPClient();
	smtp->sentEmail();
	cout << "np";
}