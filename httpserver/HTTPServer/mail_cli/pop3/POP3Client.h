#ifndef _POP3CLIENT_H_
#define _POP3CLIENT_H_

#define RESERVED_POP3_PORT (USHORT(110))
#define RESERVED_POP3_PREFIX "pop"
#define BUFFER_SIZE 4096

#include "../MailReceiver.h"
#include "../../socket/TCPClientSocket.h"

class POP3Client : public MailReceiver
{
public:
	POP3Client();
	POP3Client(const POP3Client& pop3cli);
	virtual ~POP3Client();

	bool login(rstring usr, rstring passwd);
	void collectmail();
	void logout();
private:
	TCPClientSocket* mConn;		// �׽���
	POP3State mState;			// �Ự״̬	


	virtual char* prefix(const char* host);
	
	/* command checking and sending */
	bool cmdOK(const char* resp);
	int inline cmdWithSingLineReply(const char* cmd, char** reply, int* outlen);
	int inline cmdWithMultiLinesReply(const char* cmd, const char* ends, char** reply, int* outlen);

	/* command list */
	int user(const char* usr, char** reply = nullptr, int* outlen = nullptr);
	int pass(const char* passwd, char** reply = nullptr, int* outlen = nullptr);
	int apop(const char* name, const char* digest, char** reply = nullptr, int* outlen = nullptr);
	int quit(char** reply = nullptr, int* outlen = nullptr);
	int noop(char** reply = nullptr, int* outlen = nullptr);
	int stat(char** reply, int* outlen);
	int list(char** reply, int* outlen, int no = -1);
	int retr(char** reply, int* outlen, int no);
	int top(char** reply, int* outlen, int no, int lines = 0);
	int dele(int no, char** reply = nullptr, int* outlen = nullptr);
	int rest(char** reply = nullptr, int* outlen = nullptr);
	int uidl(char** reply, int* outlen, int no = -1);
};

enum POP3State {
	Unconnected,		// δ���ӵ�������
	Authorization,		// ��֤״̬
	Transaction,		// ����״̬
	Update				// ����״̬
};


#endif // !_POP3CLIENT_H_