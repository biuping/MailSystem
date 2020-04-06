#include "POP3ClientSocket.h"
#include <iostream>
#include <string>
#include <cstring>
#include <WS2tcpip.h>


POP3ClientSocket::POP3ClientSocket()
{
}

POP3ClientSocket::POP3ClientSocket(const POP3ClientSocket& pop3cliSocket) : TCPClientSocket(pop3cliSocket)
{
}

POP3ClientSocket::POP3ClientSocket(const char* at, int tmo) : TCPClientSocket(at, RESERVED_POP3_PORT, tmo)
{
}

POP3ClientSocket::POP3ClientSocket(const char* at, USHORT port, int tmo) : TCPClientSocket(at, port, tmo)
{
}

POP3ClientSocket::~POP3ClientSocket()
{
}

// ����������ǰ���� pop ǰ׺
// ����ָ��ƴ�Ӻ�����ַ�����ָ�룬ע��ʹ�ú�Ӧʹ�� delete[] �ͷŸ��ַ���
char* POP3ClientSocket::prefix(const char* host)
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

bool POP3ClientSocket::connect2Server(const char* at, int tmo)
{
	// �� at ����ǰ�������������ǰ׺
	char* addr = prefix(at);

	bool succ = TCPClientSocket::connect2Server(addr, RESERVED_POP3_PORT, tmo);
	
	delete[] addr;

	return succ;
}

bool POP3ClientSocket::connect2Server(const char* at, USHORT port, int tmo)
{
	// �� at ����ǰ���� ��Ϸ������ǰ׺
	char* addr = prefix(at);

	bool succ = TCPClientSocket::connect2Server(addr, port, tmo);

	delete[] addr;

	return succ;
}

// ��������Ƿ�ɹ�
// resp Ϊ����ͺ��յ��Ļظ�
bool POP3ClientSocket::cmdOK(const char* resp)
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
int inline POP3ClientSocket::cmdWithSingLineReply(const char* cmd, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];

	int cmdlen = strlen(cmd);
	int r = this->write(cmd, cmdlen);		// ��������

	// ���д����
	if (r <= 0)
		return -1;

	r = this->readline(buf, BUFFER_SIZE);	// ��ȡ�ظ�

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
int inline POP3ClientSocket::cmdWithMultiLinesReply(
	const char* cmd, const char* ends, char** reply, int* outlen
)
{
	int cmdlen = strlen(cmd);
	int r = this->write(cmd, cmdlen);	// ��������
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
			int r = this->readblock(newBuf + sz, totsz - sz);
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
				char* tmpbuf = (char*)realloc(newBuf, totsz+1);		// ��1���ֽڼ�����

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
int POP3ClientSocket::user(const char* usr, char ** reply, int* outlen)
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
int POP3ClientSocket::pass(const char* passwd, char ** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "PASS %s\r\n", passwd);		// PASS ������뻺����
	
	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 QUIT ����˳����ӣ�ͬʱ�Ͽ� socket ����
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
int POP3ClientSocket::quit(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// ������

	sprintf(buf, "QUIT\r\n");

	int ret = cmdWithSingLineReply(buf, reply, outlen);

	this->closeSocket();

	return ret;
}

// POP3 NOOP ����������
// reply: ���� nullptr(Ĭ��) ��ʾ�����Ļظ���
//		  �ǿ�ʱΪָ���ַ�ָ���ָ�룬���������0����replyָ���ָ��ָ��һ���µ��ڴ�
// outlen: ��� reply ����·���ռ�Ĵ�С
// �����ظ����� 0������ظ����󷵻� 1����д������޻ظ����ڴ����ʧ�ܷ��� -1
// �����ظ�����������
int POP3ClientSocket::noop(char** reply, int* outlen)
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
int POP3ClientSocket::stat(char** reply, int* outlen)
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
int POP3ClientSocket::list(char** reply, int*outlen, int no)
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
int POP3ClientSocket::retr(char** reply, int* outlen, int no)
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
int POP3ClientSocket::top(char** reply, int*outlen, int no, int lines)
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
int POP3ClientSocket::dele(int no, char** reply, int* outlen)
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
int POP3ClientSocket::rest(char** reply, int* outlen)
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
int POP3ClientSocket::uidl(char** reply, int* outlen, int no)
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