#ifndef  _POP3CLIENT_SOCKET_H_
#define _POP3CLIENT_SOCKET_H_

#define RESERVED_POP3_PORT (USHORT(110))
#define RESERVED_POP3_PREFIX "pop"
#define BUFFER_SIZE 4096

#include "TCPClientSocket.h"

using namespace std;


class POP3ClientSocket : TCPClientSocket
{
public:
	POP3ClientSocket();
	POP3ClientSocket(const POP3ClientSocket& pop3cliSocket);
	POP3ClientSocket(const char* at, int tmo = DEFAULT_WRITE_TIMEOUT);
	POP3ClientSocket(const char* at, USHORT, int tmo = DEFAULT_WRITE_TIMEOUT);
	virtual ~POP3ClientSocket();

	virtual bool connect2Server(const char* at, int tmo = DEFAULT_WRITE_TIMEOUT);
	virtual bool connect2Server(const char* at, USHORT port, int tmo = DEFAULT_WRITE_TIMEOUT);

	int user(const char* usr, char ** reply = nullptr, int* outlen = nullptr);
	int pass(const char* passwd, char ** reply = nullptr, int* outlen = nullptr);
	int quit(char** reply = nullptr, int* outlen = nullptr);
	int noop(char** reply = nullptr, int* outlen = nullptr);
	int stat(char** reply, int* outlen);
	int list(char** reply, int* outlen, int no = -1);
	int retr(char** reply, int*outlen, int no);
	int top(char** reply, int* outlen, int no, int lines = 0);
	int dele(int no, char** reply = nullptr, int* outlen = nullptr);
	int rest(char** reply = nullptr, int* outlen = nullptr);
	int uidl(char** reply, int*outlen, int no = -1);

protected:

private:
	virtual char* prefix(const char* host);
	bool cmdOK(const char* resp);
	int inline cmdWithSingLineReply(const char* cmd, char** reply, int* outlen);
	int inline cmdWithMultiLinesReply(const char* cmd, const char* ends, char** reply, int* outlen);
};


#endif // ! _POP3CLIENT_SOCKET_H_