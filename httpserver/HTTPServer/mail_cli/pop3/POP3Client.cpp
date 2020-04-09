#include "POP3Client.h"


POP3Client::POP3Client() : mConn(new TCPClientSocket())
{
}

POP3Client::POP3Client(const POP3Client& pop3cli) : mConn(pop3cli.mConn)
{
}

POP3Client::~POP3Client()
{
	this->quit();

	delete mConn;
}

// ����������ǰ���� pop ǰ׺
// ����ָ��ƴ�Ӻ�����ַ�����ָ�룬ע��ʹ�ú�Ӧʹ�� delete[] �ͷŸ��ַ���
char* POP3Client::prefix(const char* host)
{
	// ��ȡ�ַ�������
	int prefixlen = strlen(RESERVED_POP3_PREFIX);
	int hostlen = strlen(host);
	// �����ڴ�
	char* outs = new char[hostlen + prefixlen + 1];
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
	if (strncmp(resp, "+OK", 3) == 0)
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
		ret = cmdOK(*reply);
	}

	return ret;
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