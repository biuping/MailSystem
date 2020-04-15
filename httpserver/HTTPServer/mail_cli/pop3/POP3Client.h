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
		Unconnected,		// 未连接到服务器
		Authorization,		// 验证状态
		Transaction,		// 事务状态
		Update				// 更新状态
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
	virtual bool retrMail(size_t i, Mail* mail);
	virtual bool deleteMail(size_t i);
	virtual bool deleteMail(rstring uid);

	slist& getCapabilities();

private:
	TCPClientSocket* mConn;		// 套接字
	POP3State mState;			// 会话状态
	slist capabilities;			// 兼容性

	size_t getNo(const rstring& uid);

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
	int retr(char** reply, int* outlen, size_t no);
	int top(char** reply, int* outlen, size_t no, size_t lines = 0);
	int dele(size_t no, char** reply = nullptr, int* outlen = nullptr);
	int rest(char** reply = nullptr, int* outlen = nullptr);
	int uidl(char** reply, int* outlen, int no = -1);
	int capa(char** reply, int* outlen);

	// 报告信息
	virtual void report(const rstring& msg);
};


#endif // !_POP3CLIENT_H_