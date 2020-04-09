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

// 在主机域名前加上 pop 前缀
// 返回指向拼接后的新字符串的指针，注意使用后应使用 delete[] 释放该字符串
char* POP3Client::prefix(const char* host)
{
	// 获取字符串长度
	int prefixlen = strlen(RESERVED_POP3_PREFIX);
	int hostlen = strlen(host);
	// 分配内存
	char* outs = new char[hostlen + prefixlen + 1];
	// 拼接
	strncpy(outs, RESERVED_POP3_PREFIX, prefixlen);
	outs[prefixlen] = '.';
	strncpy(outs + prefixlen + 1, host, hostlen + 1);

	return outs;
}

// 检查命令是否成功
// resp 为命令发送后收到的回复
bool POP3Client::cmdOK(const char* resp)
{
	if (strncmp(resp, "+OK", 3) == 0)
		return true;

	return false;
}

// 回复单行内容的命令
// cmd: 命令字符串
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回0，回复错误返回1，读写错误或内存分配失败返回-1
int inline POP3Client::cmdWithSingLineReply(const char* cmd, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];

	int cmdlen = strlen(cmd);
	int r = mConn->write(cmd, cmdlen);		// 发送命令

	// 检查写错误
	if (r <= 0)
		return -1;

	r = mConn->readline(buf, BUFFER_SIZE);	// 读取回复

	// 检查读错误或无回复
	if (r <= 0)
		return -1;

	if (reply != nullptr) {
		// 返回回复
		int newlen = strlen(buf) + 1;
		*reply = (char*)calloc(newlen, sizeof(char));
		if (*reply == nullptr) {
			// 内存分配失败
			return -1;
		}
		strncpy(*reply, buf, newlen);
		*outlen = newlen;
	}

	return cmdOK(buf) ? 0 : 1;
}

// 回复多行内容的命令
// cmd: 命令
// ends: 结束标志字符串，返回的回复内容遇到 ends 后结束，不包含 ends
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 给 reply 分配的内存空间的大小，一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int inline POP3Client::cmdWithMultiLinesReply(
	const char* cmd, const char* ends, char** reply, int* outlen
)
{
	int cmdlen = strlen(cmd);
	int r = mConn->write(cmd, cmdlen);	// 发送命令
	int ret = 0;	// 返回值

	if (r < 0) {
		ret = -1;
	}
	else {
		int totsz = BUFFER_SIZE;
		char* newBuf = (char*)calloc(BUFFER_SIZE, sizeof(char));	// 多分配一个字节用于判断是否需要新增分配空间
		char* pEnds = nullptr;		// 暂时存储 ends 标志所在位置的指针
		if (newBuf == nullptr) {
			ret = -1;
		}

		int sz = 0;		// 已收到的字节数
		while (ret >= 0) {
			int r = mConn->readblock(newBuf + sz, totsz - sz);
			if (r < 0) {
				// 发生读错误
				ret = -1;
				break;
			}

			sz += r;
			newBuf[sz] = '\0';

			if ((pEnds = strstr(newBuf, ends)) != nullptr) {
				// 以 ends 标志结束
				*pEnds = '\0';		// 返回的回复中不含 ends
				break;
			}
			if (sz >= totsz) {
				// 已读字节数溢出当前分配的空间
				totsz += BUFFER_SIZE;
				char* tmpbuf = (char*)realloc(newBuf, totsz + 1);		// 多1个字节检测溢出

				if (tmpbuf == nullptr) {
					ret = -1;
					break;
				}

				newBuf = tmpbuf;
			}
		}
		if (ret >= 0) {
			// 未出现错误，输出回复内容及分配空间大小
			*reply = newBuf;
			*outlen = totsz + 1;
		}
		else if (newBuf != nullptr) {
			// 出现错误，若已分配内存则清空
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

// POP3 USER 命令，指定用户名
// usr: 用户名
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::user(const char* usr, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// 缓冲区

	sprintf(buf, "USER %s\r\n", usr);		// 放入 USER 命令

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 PASS 命令，验证密码
// passwd: 密码
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::pass(const char* passwd, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// 缓冲区

	sprintf(buf, "PASS %s\r\n", passwd);		// PASS 命令放入缓冲区

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 APOP 命令，另一种提供确认过程的方式
// name: 邮箱字串，同用户名
// digest: MD5 产生的包括时间戳和共享密钥的字串
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::apop(const char* name, const char* digest, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];

	sprintf(buf, "APOP %s,%s\r\n", name, digest);

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 QUIT 命令，退出连接，同时断开 socket 连接
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::quit(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// 缓冲区

	sprintf(buf, "QUIT\r\n");

	int ret = cmdWithSingLineReply(buf, reply, outlen);

	mConn->closeSocket();

	return ret;
}

// POP3 NOOP 命令，检查连接
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
// 正常回复表连接正常
int POP3Client::noop(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// 缓冲区

	sprintf(buf, "NOOP\r\n");

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 STAT 命令，查看状态
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
// 正常回复内容包含邮件数量以及所占空间的大小
int POP3Client::stat(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// 缓冲区

	sprintf(buf, "STAT\r\n");

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 LIST 命令，查看邮件大小信息，指定序号返回单行，不指定返回多行
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 为给 reply 分配的内存空间的大小，返回多行时一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// no 指定返回特定序号邮件的大小信息，默认值 -1 表示不指定
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int POP3Client::list(char** reply, int* outlen, int no)
{
	if (no > 0) {
		// 指定序号，返回单行内容
		char buf[BUFFER_SIZE];		// 缓冲区
		sprintf(buf, "LIST %d\r\n", no);
		return cmdWithSingLineReply(buf, reply, outlen);
	}
	else {
		// 不指定序号，返回多行内容
		return cmdWithMultiLinesReply("LIST\r\n", "\r\n.\r\n", reply, outlen);
	}
}

// POP3 RETR 命令，取特定序号邮件信息
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 为给 reply 分配的内存空间的大小，一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int POP3Client::retr(char** reply, int* outlen, int no)
{
	char buf[BUFFER_SIZE];		// 缓冲区
	sprintf(buf, "RETR %d\r\n", no);

	return cmdWithMultiLinesReply(buf, "\r\n.\r\n", reply, outlen);
}

// POP3 TOP 命令，取特定序号邮件的头部和信体的指定行数内容
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 为给 reply 分配的内存空间的大小，一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// no: 指定返回特定序号邮件的相关内容
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int POP3Client::top(char** reply, int* outlen, int no, int lines)
{
	char buf[BUFFER_SIZE];		// 缓冲区
	sprintf(buf, "TOP %d %d\r\n", no, lines);

	return cmdWithMultiLinesReply(buf, "\r\n.\r\n", reply, outlen);
}

// POP3 DELE 命令，删除指定序号邮件
// no: 指定删除特定序号邮件
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::dele(int no, char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// 缓冲区

	sprintf(buf, "DELE %d\r\n", no);		// PASS 命令放入缓冲区

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 REST 命令，复位POP3会话，标志删除的邮件取消删除标记
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::rest(char** reply, int* outlen)
{
	char buf[BUFFER_SIZE];		// 缓冲区

	sprintf(buf, "REST\r\n");		// PASS 命令放入缓冲区

	return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 UIDL 命令，查看邮件唯一标识，指定序号返回单行，不指定返回多行
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存
// outlen: 为给 reply 分配的内存空间的大小，返回多行时一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// no 指定返回特定序号邮件的大小信息，默认值 -1 表示不指定
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int POP3Client::uidl(char** reply, int* outlen, int no)
{
	if (no > 0) {
		// 取单行内容
		char buf[BUFFER_SIZE];		// 缓冲区
		sprintf(buf, "UIDL %d\r\n", no);
		return cmdWithSingLineReply(buf, reply, outlen);
	}
	else {
		return cmdWithMultiLinesReply("UIDL\r\n", "\r\n.\r\n", reply, outlen);
	}
}