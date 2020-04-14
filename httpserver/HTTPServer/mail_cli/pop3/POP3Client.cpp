#include "POP3Client.h"


POP3Client::POP3Client() : mConn(new TCPClientSocket()), mState(POP3State::Unconnected)
{
}

POP3Client::POP3Client(const POP3Client& pop3cli) : mConn(pop3cli.mConn), mState(pop3cli.mState)
{
}

POP3Client::~POP3Client()
{
	if (alive()) {
		this->quit();
	}

	delete mConn;
	mConn = nullptr;
}

// ��������
// at: �ʼ���ַ�е�������
bool POP3Client::open(const rstring& at)
{
	return this->open(at, RESERVED_POP3_PORT);
}

// ��������
// at: �ʼ���ַ�е�������
// port: ������ pop3 ����˿ں�
bool POP3Client::open(const rstring& at, USHORT port)
{
	char* addr = prefix(at.c_str());		// ����ǰ׺
	char* reply = nullptr;		// ���ջظ�
	int replyLen = 0;		// ���ջظ�����ռ�ռ䳤��
	int connRet = 0;		// ���Ӳ���״̬
	bool ret = false;		// ����ֵ
	
	if ( (connRet = conn(addr, port, &reply, &replyLen)) < 0 ) {
		// ����ʧ��
		report("cannot open connection");
		mState = POP3State::Unconnected;
		ret = false;
	}
	else {
		// ���ӳɹ�
		mState = POP3State::Authorization;
		ret = true;

		rstring infostr = "connection opened";
		if (connRet > 0) {
			// ���������� ERR
			infostr.append(" but server returned ");
			infostr.append(reply);
		}
		report(infostr);
	}

	// ������Դ
	delete addr;
	delete reply;

	// ��ȡ�������б�
	getCapabilities();

	return ret;
}

void POP3Client::close()
{
	return;
}

// ��֤���
// at: ����������
// usr: �û���
// passwd: �û�����
bool POP3Client::authenticate(const rstring& usr, const rstring& passwd)
{
	if (mState != POP3State::Authorization) {
		// ��������֤״̬
		rstring errstr = "authentication not available in state ";
		errstr += static_cast<int>(mState);
		report(errstr);
	}

	// ������֤״̬
	char* usrReply, * passReply;
	int usrReplyLen, passReplyLen, usrRet, passRet;
	usrReply = passReply = nullptr;
	usrRet = passRet = -1;
	if ( (usrRet =  this->user(usr.c_str(), &usrReply, &usrReplyLen)) == 0
		&& (passRet = this->pass(passwd.c_str(), &passReply, &passReplyLen)) == 0 ) {
		// user�����pass������ȷ�ظ�
		mState = POP3State::Transaction;		// ��������״̬
		report("authenticated");
	}
	else {
		// �����֤ʧ�ܣ�״̬����
		rstring errstr = "authentication failed: ";
		// ��Ӷ�Ӧ������Ϣ
		errstr.append( usrRet <= 0 ? ( (usrRet < 0 || passRet < 0) ? "IO error" : passReply ) : usrReply );
		report(errstr);
	}

	delete usrReply;
	delete passReply;

	return (usrRet == 0) && (passRet == 0);
}

// ��ȡ����ָ���б���ǿ�ƻ�ȡ��
// return: list<string> �ַ�������
slist& POP3Client::getCapabilities()
{
	// �������Ҽ���ָ���б�Ϊ��
	if (this->capabilities.size() == 0 && mState != POP3State::Unconnected) {
		// δ��ȡ�������б�
		char* reply = nullptr, * p;
		int len, capaRet = 0;

		// ���� CAPA ����
		if ((capaRet = this->capa(&reply, &len)) == 0) {
			p = strstr(reply, "\r\n");		// ����ȷ����Ϣ

			// ����ÿһ���м�������
			while (*p != '\0') {
				p += 2;		// �������з�
				char cmd[50];
				int i = 0;

				// ����һ������
				while (*p != '\0') {
					if (*p == '\r' && *(p + 1) == '\n') {
						// ����
						break;
					}
					cmd[i++] = *p++;
				}
				cmd[i] = '\0';

				this->capabilities.emplace_back(cmd);
			}
		}
	}

	return this->capabilities;
}

// ���������Ƿ񱣳�
bool POP3Client::alive()
{
	return this->noop() == 0;
}

// ��������״̬
bool POP3Client::getStatus(size_t& mailnum, size_t& totsize)
{
	char* reply = nullptr;		// �ظ�
	char buf[10];				// �ظ�OK������
	int replyLen, statRet;		// �ظ�ʹ�õ��ڴ��С��stat����ֵ
	bool ret = false;

	if ( (statRet = this->stat(&reply, &replyLen)) == 0) {
		sscanf(reply, "%s %u %u", buf, &mailnum, &totsize);

		ret = true;
	}
	else {
		// ���������Ϣ
		rstring errstr = "get status failed: ";
		errstr.append(statRet > 0 ? reply : "IO error");
		report(errstr);
	}

	delete reply;
	return ret;
}

// ��ȡ�ʼ��б�Ϊÿ���ʼ����ô�С
// mails: �ʼ��б�Ϊ���������չ��������Ҫ���ʼ���������һ��
bool POP3Client::getMailListWithSize(std::vector<Mail*>& mails)
{
	char* reply;
	char buf[10];
	int replyLen, listRet;
	size_t mailnum, totsize;
	bool ret = false;


	if ((listRet = this->list(&reply, &replyLen)) == 0) {
		ret = true;

		// ����������ַ���
		char* p = strstr(reply, "\r\n");		// ��һ��״̬��
		_snscanf(reply, p - reply, "%s %u %u", buf, &mailnum, &totsize);

		// ����ʼ��б�size�����ڵ�ǰ�ʼ���Ŀ��clear��capacity����������ʵ���reserve
		if (mails.size() != mailnum) {
			// �ͷ�ԭ�ʼ���Դ
			for (Mail* m : mails) {
				delete m;
				m = nullptr;
			}
			mails.clear();
			// ����
			if (mails.capacity() < mailnum) {
				mails.reserve(mailnum);
			}
			// ��������Դ
			for (size_t i = 0; i < mailnum; ++i) {
				mails.push_back(new Mail());
			}
		}

		size_t no, eachsize;
		p += 2;
		// ��ȡÿһ�е�����
		for (size_t i = 0; i < mailnum; ++i) {
			// ��ȡ��һ�е�
			char* ptemp = strstr(p + 2, "\r\n");
			_snscanf(p, ptemp - p, "%u %u", &no, &eachsize);
			no = no > 0 ? no - 1 : no;		// for safety
			mails[no]->setSize(eachsize);

			p = ptemp + 2;		// ��һ��
		}
	}
	else {
		// ���������Ϣ
		rstring errstr = "get list failed: ";
		errstr.append(listRet > 0 ? reply : "IO error");
		report(errstr);
	}

	delete reply;
	return ret;
}

// ��ȡ�ʼ��б�Ϊÿ���ʼ����� UID
// mails: �ʼ��б�Ϊ���������չ��������Ҫ���ʼ���������һ��
bool POP3Client::getMailListWithUID(std::vector<Mail*>& mails)
{
	char* reply;
	char buf[10];
	int replyLen, uidlRet;
	size_t mailnum, totsize;
	bool ret = false;


	if ((uidlRet = this->uidl(&reply, &replyLen)) == 0) {
		ret = true;

		// ����������ַ���
		char* p = strstr(reply, "\r\n");		// ��һ��״̬��
		_snscanf(reply, p - reply, "%s %u %u", buf, &mailnum, &totsize);

		// ����ʼ��б�size�����ڵ�ǰ�ʼ���Ŀ��clear��capacity����������ʵ���reserve
		if (mails.size() != mailnum) {
			// �ͷ�ԭ�ʼ���Դ
			for (Mail* m : mails) {
				delete m;
				m = nullptr;
			}
			mails.clear();
			// ����
			if (mails.capacity() < mailnum) {
				mails.reserve(mailnum);
			}
			// ��������Դ
			for (size_t i = 0; i < mailnum; ++i) {
				mails.push_back(new Mail());
			}
		}

		size_t no;
		char uid[BUFFER_SIZE];
		p += 2;
		// ��ȡÿһ�е�����
		for (size_t i = 0; i < mailnum; ++i) {
			// ��ȡ��һ�е�
			char* ptemp = strstr(p + 2, "\r\n");
			_snscanf(p, ptemp - p, "%u %s", &no, uid);
			no = no > 0 ? no - 1 : no;		// for safety
			mails[no]->setUID(uid);

			p = ptemp + 2;		// ��һ��
		}
	}
	else {
		// ���������Ϣ
		rstring errstr = "get uid list failed: ";
		errstr.append(uidlRet > 0 ? reply : "IO error");
		report(errstr);
	}

	delete reply;
	return ret;
}

bool POP3Client::retrMail(int i, Mail* mail)
{
	return false;
}

// ����������ǰ���� pop ǰ׺
// ����ָ��ƴ�Ӻ�����ַ�����ָ�룬ע��ʹ�ú�Ӧʹ�� delete[] �ͷŸ��ַ���
char* POP3Client::prefix(const char* host)
{
	// ��ȡ�ַ�������
	int prefixlen = strlen(RESERVED_POP3_PREFIX);
	int hostlen = strlen(host);
	// �����ڴ�
	char* outs = new char[hostlen + prefixlen + 2];
	// ƴ��
	strncpy(outs, RESERVED_POP3_PREFIX, prefixlen);
	outs[prefixlen] = '.';
	strncpy(outs + prefixlen + 1, host, hostlen + 1);

	return outs;
}

// ��������Ƿ�ɹ�
// resp Ϊ����ͺ��յ��Ļظ�
bool POP3Client::cmdOK(const char* resp)
{
	if (resp[0] == '+')
		return true;

	return false;
}

// �ظ��������ݵ�����
// cmd: �����ַ���
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ�����0���ظ����󷵻�1����д������ڴ����ʧ�ܷ���-1
int inline POP3Client::cmdWithSingLineReply(const char* cmd, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];

	int cmdlen = strlen(cmd);
	int r = mConn->write(cmd, cmdlen);		// ��������

	// ���д����
	if (r <= 0)
		return -1;

	r = mConn->readline(buf, BUFFER_SIZE);	// ��ȡ�ظ�

	// ����������޻ظ�
	if (r <= 0)
		return -1;

	if (reply != nullptr) {
		// ���ػظ�
		int newlen = strlen(buf) + 1;
		*reply = (char*)calloc(newlen, sizeof(char));
		if (*reply == nullptr) {
			// �ڴ����ʧ��
			return -1;
		}
		strncpy(*reply, buf, newlen);
		*outlen = newlen;
	}

	return cmdOK(buf) ? 0 : 1;
}

// �ظ��������ݵ�����
// cmd: ����
// ends: ������־�ַ��������صĻظ��������� ends ������������� ends
// reply: ָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: �� reply ������ڴ�ռ�Ĵ�С��һ��Ϊ BUFFER_SIZE �ı��� + 1�����ܶ���ʵ�ʳ��ȣ�
// �����ظ�����0���ظ����󷵻�1����д�����ڴ����ʧ�ܷ���-1
int inline POP3Client::cmdWithMultiLinesReply(
	const char* cmd, const char* ends, char** reply, int* outlen
)
{
	int cmdlen = strlen(cmd);
	int r = mConn->write(cmd, cmdlen);	// ��������
	int ret = 0;	// ����ֵ

	if (r < 0) {
		ret = -1;
	}
	else {
		int totsz = BUFFER_SIZE;
		char* newBuf = (char*)calloc(BUFFER_SIZE, sizeof(char));	// �����һ���ֽ������ж��Ƿ���Ҫ��������ռ�
		char* pEnds = nullptr;		// ��ʱ�洢 ends ��־����λ�õ�ָ��
		if (newBuf == nullptr) {
			ret = -1;
		}

		int sz = 0;		// ���յ����ֽ���
		while (ret >= 0) {
			int r = mConn->readblock(newBuf + sz, totsz - sz);
			if (r < 0) {
				// ����������
				ret = -1;
				break;
			}

			sz += r;
			newBuf[sz] = '\0';

			if ((pEnds = strstr(newBuf, ends)) != nullptr) {
				// �� ends ��־����
				*pEnds = '\0';		// ���صĻظ��в��� ends
				break;
			}
			if (sz >= totsz) {
				// �Ѷ��ֽ��������ǰ����Ŀռ�
				totsz += BUFFER_SIZE;
				char* tmpbuf = (char*)realloc(newBuf, totsz + 1);		// ��1���ֽڼ�����

				if (tmpbuf == nullptr) {
					ret = -1;
					break;
				}

				newBuf = tmpbuf;
			}
		}
		if (ret >= 0) {
			// δ���ִ�������ظ����ݼ�����ռ��С
			*reply = newBuf;
			*outlen = totsz + 1;
		}
		else if (newBuf != nullptr) {
			// ���ִ������ѷ����ڴ������
			free(newBuf);
		}
	}

	if (ret == -1) {
		*outlen = 0;
	}
	else {
		ret = cmdOK(*reply) ? 0 : 1;
	}

	return ret;
}

// ���ӵ� POP3 ������
// addr: ��������ַ
// port: POP3 ����˿ں�
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3Client::conn(const char* addr, USHORT port, char** reply, int* outlen)
{
	this->mConn->connect(addr, port);

	char buf[BUFFER_SIZE];		// ��������

	int r = mConn->readline(buf, BUFFER_SIZE);		// ��ȡ�ظ�

	// ����������޻ظ�
	if (r <= 0) {
		this->mConn->closeSocket();		// �ر��׽���
		return -1;
	}

	if (reply != nullptr) {
		// ���ػظ�
		int newlen = strlen(buf) + 1;
		*reply = (char*)calloc(newlen, sizeof(char));
		if (*reply == nullptr) {
			// �ڴ����ʧ��
			return -1;
		}
		strncpy(*reply, buf, newlen);
		*outlen = newlen;
	}

	return cmdOK(buf) ? 0 : 1;
}

// POP3 USER ���ָ���û���
// usr: �û���
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3Client::user(const char* usr, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "USER %s\r\n", usr);		// ���� USER ����

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 PASS �����֤����
// passwd: ����
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3Client::pass(const char* passwd, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "PASS %s\r\n", passwd);		// PASS ������뻺����

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 APOP �����һ���ṩȷ�Ϲ��̵ķ�ʽ
// name: �����ִ���ͬ�û���
// digest: MD5 �����İ���ʱ����͹�����Կ���ִ�
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3Client::apop(const char* name, const char* digest, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];

	sprintf(buf, "APOP %s,%s\r\n", name, digest);

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 QUIT ����˳����ӣ�ͬʱ�Ͽ� socket ����
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3Client::quit(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "QUIT\r\n");

	int ret = cmdWithSingLineReply(buf, reply, outlen);

	mConn->closeSocket();

	return ret;
}

// POP3 NOOP ����������
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
// �����ظ�����������
int POP3Client::noop(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "NOOP\r\n");

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 STAT ����鿴״̬
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
// �����ظ����ݰ����ʼ������Լ���ռ�ռ�Ĵ�С
int POP3Client::stat(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "STAT\r\n");

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 LIST ����鿴�ʼ���С��Ϣ��ָ����ŷ��ص��У���ָ�����ض���
// reply: ָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: Ϊ�� reply ������ڴ�ռ�Ĵ�С�����ض���ʱһ��Ϊ BUFFER_SIZE �ı��� + 1�����ܶ���ʵ�ʳ��ȣ�
// no ָ�������ض�����ʼ��Ĵ�С��Ϣ��Ĭ��ֵ -1 ��ʾ��ָ��
// �����ظ�����0���ظ����󷵻�1����д�����ڴ����ʧ�ܷ���-1
int POP3Client::list(char** reply, int* outlen, int no)
{
	if (no > 0) {
		// ָ����ţ����ص�������
		char buf[BUFFER_SIZE];		// ������
		sprintf(buf, "LIST %d\r\n", no);
		return cmdWithSingLineReply(buf, reply, outlen);
	}
	else {
		// ��ָ����ţ����ض�������
		return cmdWithMultiLinesReply("LIST\r\n", "\r\n.\r\n", reply, outlen);
	}
}

// POP3 RETR ���ȡ�ض�����ʼ���Ϣ
// reply: ָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: Ϊ�� reply ������ڴ�ռ�Ĵ�С��һ��Ϊ BUFFER_SIZE �ı��� + 1�����ܶ���ʵ�ʳ��ȣ�
// �����ظ�����0���ظ����󷵻�1����д�����ڴ����ʧ�ܷ���-1
int POP3Client::retr(char** reply, int* outlen, int no)
{
	char buf[BUFFER_SIZE];		// ������
	sprintf(buf, "RETR %d\r\n", no);

	return cmdWithMultiLinesReply(buf, "\r\n.\r\n", reply, outlen);
}

// POP3 TOP ���ȡ�ض�����ʼ���ͷ���������ָ����������
// reply: ָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: Ϊ�� reply ������ڴ�ռ�Ĵ�С��һ��Ϊ BUFFER_SIZE �ı��� + 1�����ܶ���ʵ�ʳ��ȣ�
// no: ָ�������ض�����ʼ����������
// �����ظ�����0���ظ����󷵻�1����д�����ڴ����ʧ�ܷ���-1
int POP3Client::top(char** reply, int* outlen, int no, int lines)
{
	char buf[BUFFER_SIZE];		// ������
	sprintf(buf, "TOP %d %d\r\n", no, lines);

	return cmdWithMultiLinesReply(buf, "\r\n.\r\n", reply, outlen);
}

// POP3 DELE ���ɾ��ָ������ʼ�
// no: ָ��ɾ���ض�����ʼ�
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3Client::dele(int no, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "DELE %d\r\n", no);		// PASS ������뻺����

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 REST �����λPOP3�Ự����־ɾ�����ʼ�ȡ��ɾ�����
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3Client::rest(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "REST\r\n");		// PASS ������뻺����

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 UIDL ����鿴�ʼ�Ψһ��ʶ��ָ����ŷ��ص��У���ָ�����ض���
// reply: ָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: Ϊ�� reply ������ڴ�ռ�Ĵ�С�����ض���ʱһ��Ϊ BUFFER_SIZE �ı��� + 1�����ܶ���ʵ�ʳ��ȣ�
// no ָ�������ض�����ʼ��Ĵ�С��Ϣ��Ĭ��ֵ -1 ��ʾ��ָ��
// �����ظ�����0���ظ����󷵻�1����д�����ڴ����ʧ�ܷ���-1
int POP3Client::uidl(char** reply, int* outlen, int no)
{
	if (no > 0) {
		// ȡ��������
		char buf[BUFFER_SIZE];		// ������
		sprintf(buf, "UIDL %d\r\n", no);
		return cmdWithSingLineReply(buf, reply, outlen);
	}
	else {
		return cmdWithMultiLinesReply("UIDL\r\n", "\r\n.\r\n", reply, outlen);
	}
}

// POP3 capa ����鿴����˼��������б�
// reply: ָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: Ϊ�� reply ������ڴ�ռ�Ĵ�С��һ��Ϊ BUFFER_SIZE �ı��� + 1�����ܶ���ʵ�ʳ��ȣ�
// �����ظ�����0���ظ����󷵻�1����д�����ڴ����ʧ�ܷ���-1
int POP3Client::capa(char** reply, int* outlen)
{
	return cmdWithMultiLinesReply("CAPA\r\n", "\r\n.\r\n", reply, outlen);
}

// ������Ϣ
void POP3Client::report(const rstring& msg)
{
	LogUtil::report("POP3Client " + msg);
}