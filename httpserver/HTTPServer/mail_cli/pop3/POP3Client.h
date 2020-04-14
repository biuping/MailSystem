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
	enum class POP3State {
		Unconnected,		// δ���ӵ�������
		Authorization,		// ��֤״̬
		Transaction,		// ����״̬
		Update				// ����״̬
	};

	POP3Client();
	POP3Client(const POP3Client& pop3cli);
	virtual ~POP3Client();

	/* interfaces implemented */
	virtual bool open(const rstring& at);
	virtual bool open(const rstring& at, USHORT port);
	virtual void close();
	virtual bool alive();
	virtual bool authenticate(const rstring& usr, const rstring& passwd);
	virtual bool getStatus(size_t& mailnum, size_t& totsize);
	virtual bool getMailListWithSize(std::vector<Mail*>& mails);
	virtual bool getMailListWithUID(std::vector<Mail*>& mails);
	virtual bool retrMail(int i, Mail* mail);

	slist& getCapabilities();

private:
	TCPClientSocket* mConn;		// �׽���
	POP3State mState;			// �Ự״̬
	slist capabilities;			// ������


	inline char* prefix(const char* host);
	
	/* command checking and sending */
	bool cmdOK(const char* resp);
	inline int cmdWithSingLineReply(const char* cmd, char** reply, int* outlen);
	inline int cmdWithMultiLinesReply(const char* cmd, const char* ends, char** reply, int* outlen);

	int conn(const char* addr, USHORT port, char** reply = nullptr, int* outlen = nullptr);
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
	int capa(char** reply, int* outlen);

	// ������Ϣ
	virtual void report(const rstring& msg);
};


#endif // !_POP3CLIENT_H_