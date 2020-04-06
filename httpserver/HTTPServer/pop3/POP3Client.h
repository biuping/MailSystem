#ifndef _POP3CLIENT_H_
#define _POP3CLIENT_H_

#include "POP3ClientSocket.h"

class POP3Client
{
public:
	void login(rstring usr, rstring passwd);
	void collectmail();
	void logout();
private:
	POP3ClientSocket* mConn;
};



#endif // !_POP3CLIENT_H_