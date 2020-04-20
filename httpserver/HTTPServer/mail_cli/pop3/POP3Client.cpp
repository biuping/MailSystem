#include "POP3Client.h"
#include "../mime/MIMEParser.h"


POP3Client::POP3Client() : mConn(new TCPClientSocket()), mState(POP3State::Unconnected)
{
}

POP3Client::POP3Client(const POP3Client& pop3cli) : mConn(pop3cli.mConn), mState(pop3cli.mState)
{
}

POP3Client::~POP3Client()
{
    if (alive()) {
        this->close();
    }

    delete mConn;
    mConn = nullptr;
}

// 进行连接
// at: 邮件地址中的主机名
bool POP3Client::open(const rstring& at)
{
    return this->open(at, RESERVED_POP3_PORT);
}

// 进行连接
// at: 邮件地址中的主机名
// port: 服务器 pop3 服务端口号
bool POP3Client::open(const rstring& at, USHORT port)
{
    char* addr = prefix(at.c_str());		// 加上前缀
    char* reply = nullptr;		// 接收回复
    int replyLen = 0;		// 接收回复串所占空间长度
    int connRet = 0;		// 连接操作状态
    bool ret = false;		// 返回值
    
    if ( (connRet = conn(addr, port, &reply, &replyLen)) < 0 ) {
        // 连接失败
        report("cannot open connection");
        mState = POP3State::Unconnected;
        ret = false;
    }
    else {
        // 连接成功
        mState = POP3State::Authorization;
        ret = true;

        rstring infostr = "connection opened";
        if (connRet > 0) {
            // 服务器返回 ERR
            infostr.append(" but server returned ");
            infostr.append(reply);
        }
        report(infostr);
    }

    // 清理资源
    delete[] addr;
    free(reply);

    // 获取兼容性列表
    getCapabilities();

    return ret;
}

void POP3Client::close()
{
    if (mState != POP3State::Unconnected && mState != POP3State::Update) {
        this->quit();
        report("connection closed");

        if (mState == POP3State::Transaction) {
            // 进入更新状态
            mState = POP3State::Update;
        }
        else {
            // 断开连接
            mState = POP3State::Unconnected;
        }
    }
    return;
}

// 验证身份
// at: 邮箱主机名
// usr: 用户名
// passwd: 用户密码
bool POP3Client::authenticate(const rstring& usr, const rstring& passwd)
{
    if (mState != POP3State::Authorization) {
        // 不处于验证状态
        rstring errstr = "authentication not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    // 处于验证状态
    char* usrReply, * passReply;
    int usrReplyLen, passReplyLen, usrRet, passRet;
    usrReply = passReply = nullptr;
    usrRet = passRet = -1;
    if ( (usrRet =  this->user(usr.c_str(), &usrReply, &usrReplyLen)) == 0
        && (passRet = this->pass(passwd.c_str(), &passReply, &passReplyLen)) == 0 ) {
        // user命令和pass命令正确回复
        mState = POP3State::Transaction;		// 进入事务状态
        report("authenticated");
    }
    else {
        // 身份验证失败，状态不变
        rstring errstr = "authentication failed: ";
        // 添加对应错误信息
        errstr.append( usrRet <= 0 ? ( (usrRet < 0 || passRet < 0) ? "IO error" : passReply ) : usrReply );
        report(errstr);
    }

    free(usrReply);
    free(passReply);

    return (usrRet == 0) && (passRet == 0);
}

// 获取兼容指令列表（不强制获取）
// return: list<string> 字符串链表
slist& POP3Client::getCapabilities()
{
    // 已连接且兼容指令列表为空
    if (this->capabilities.size() == 0 &&
        mState != POP3State::Unconnected &&
        mState != POP3State::Update) {
        // 未获取兼容性列表
        char* reply = nullptr, * p;
        int len, capaRet = 0;

        // 发送 CAPA 命令
        if ((capaRet = this->capa(&reply, &len)) == 0) {
            p = strstr(reply, "\r\n");		// 跳过确认信息

            // 加入每一行中兼容命令
            while (*p != '\0') {
                p += 2;		// 跳过换行符
                char cmd[50];
                int i = 0;

                // 拷贝一行命令
                while (*p != '\0') {
                    if (*p == '\r' && *(p + 1) == '\n') {
                        // 换行
                        break;
                    }
                    cmd[i++] = *p++;
                }
                cmd[i] = '\0';

                this->capabilities.emplace_back(cmd);
            }
        }

        free(reply);
    }

    return this->capabilities;
}

// 返回连接是否保持
bool POP3Client::alive()
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "noop not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    bool isAlive = this->noop() == 0;
    if (!isAlive) {
        report("the connection is missing");
    }

    return isAlive;
}

// 返回邮箱状态
bool POP3Client::getStatus(size_t& mailnum, unsigned long long& totsize)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "status showing not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    char* reply = nullptr;		// 回复
    char buf[10];				// 回复OK缓冲区
    int replyLen, statRet;		// 回复使用的内存大小，stat返回值
    bool ret = false;

    if ( (statRet = this->stat(&reply, &replyLen)) == 0) {
        sscanf(reply, "%s %u %I64u", buf, &mailnum, &totsize);

        ret = true;
    }
    else {
        // 报告错误信息
        rstring errstr = "get status failed: ";
        errstr.append(statRet > 0 ? reply : "IO error");
        report(errstr);
    }

    free(reply);
    return ret;
}

// 获取邮件列表并为每封邮件设置大小
// mails: 邮件列表，为空则进行扩展，否则需要与邮件总数保持一致
bool POP3Client::getMailListWithSize(std::vector<Mail*>& mails)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "list not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    char* reply;
    char buf[10];
    int replyLen, listRet;
    size_t mailnum = 0U;
    unsigned long long totsize = 0ULL;
    bool ret = false;


    if ((listRet = this->list(&reply, &replyLen)) == 0) {
        ret = true;

        // 解析命令返回字符串
        char* p = strstr(reply, "\r\n");		// 第一行状态行
        _snscanf(reply, p - reply, "%s %u %I64u", buf, &mailnum, &totsize);

        // list指令回复不标准，且传入数组未初始化
        if (mailnum == 0 && mails.size() == 0) {
            if (!this->getStatus(mailnum, totsize)) {
                report("cannot get status correctly");
                free(reply);
                return false;
            }
        }

        // 检查邮件列表，size不等于当前邮件数目则clear，capacity不足则进行适当的reserve
        if (mails.size() != mailnum) {
            // 释放原邮件资源
            for (Mail* m : mails) {
                delete m;
                m = nullptr;
            }
            mails.clear();
            // 扩容
            if (mails.capacity() < mailnum) {
                mails.reserve(mailnum);
            }
            // 分配新资源
            for (size_t i = 0; i < mailnum; ++i) {
                mails.push_back(new Mail());
            }
        }

        size_t no, eachsize;
        p += 2;
        // 读取每一行的数据
        for (size_t i = 0; i < mailnum; ++i) {
            // 获取下一行的
            char* ptemp = strstr(p + 2, "\r\n");
            _snscanf(p, ptemp - p, "%u %u", &no, &eachsize);
            // 为空则分配新资源
            if (mails[no - 1] == nullptr) {
                // 序号从1开始
                mails[no - 1] = new Mail();
            }
            mails[no - 1]->setSize(eachsize);

            p = ptemp + 2;		// 下一行
        }
    }
    else {
        // 报告错误信息
        rstring errstr = "get list failed: ";
        errstr.append(listRet > 0 ? reply : "IO error");
        report(errstr);
    }

    free(reply);
    return ret;
}

// 获取邮件列表并为每封邮件设置 UID
// mails: 邮件列表，为空则进行扩展，否则需要与邮件总数保持一致
bool POP3Client::getMailListWithUID(std::vector<Mail*>& mails)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "uid list not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    char* reply;
    char buf[10];
    int replyLen, uidlRet;
    size_t mailnum = 0;
    unsigned long long totsize = 0;
    bool ret = false;


    if ((uidlRet = this->uidl(&reply, &replyLen)) == 0) {
        ret = true;

        // 解析命令返回字符串
        char* p = strstr(reply, "\r\n");		// 第一行状态行
        _snscanf(reply, p - reply, "%s %u %I64u", buf, &mailnum, &totsize);

        // uidl指令回复不标准，且传入数组未初始化
        if (mailnum == 0 && mails.size() == 0) {
            if (!this->getStatus(mailnum, totsize)) {
                // 单独的出错处理
                report("cannot get status correctly");
                free(reply);
                return false;
            }
        }

        // 检查邮件列表，size不等于当前邮件数目则clear，capacity不足则进行适当的reserve
        if (mails.size() != mailnum) {
            // 释放原邮件资源
            for (Mail* m : mails) {
                delete m;
                m = nullptr;
            }
            mails.clear();
            // 扩容
            if (mails.capacity() < mailnum) {
                mails.reserve(mailnum);
            }
            // 分配新资源
            for (size_t i = 0; i < mailnum; ++i) {
                mails.push_back(new Mail());
            }
        }

        size_t no;
        char uid[BUFFER_SIZE];
        p += 2;
        // 读取每一行的数据
        for (size_t i = 0; i < mailnum; ++i) {
            // 获取下一行的
            char* ptemp = strstr(p + 2, "\r\n");
            _snscanf(p, ptemp - p, "%u %s", &no, uid);
            // 为空则分配新资源
            if (mails[no-1] == nullptr) {
                // 邮件序号从1开始
                mails[no-1] = new Mail();
            }
            mails[no-1]->setUID(uid);

            p = ptemp + 2;		// 下一行
        }
    }
    else {
        // 报告错误信息
        rstring errstr = "get uid list failed: ";
        errstr.append(uidlRet > 0 ? reply : "IO error");
        report(errstr);
    }

    free(reply);
    return ret;
}

// 取回第 i 封邮件
// i: 邮件序号，从0开始
// mail: 邮件指针，不能为空，需在外部生成对象，防止忘记释放资源
bool POP3Client::retrMail(size_t i, Mail* mail)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "mail retrieving not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    char* reply;
    char okBuf[10], octetsBuf[20];
    int replyLen, retrRet;
    size_t mailsize;
    bool ret = false;

    // i+1 为取邮件时使用的序号
    if ((retrRet = retr(&reply, &replyLen, i+1)) == 0) {
        ret = true;

        // 第一行状态行
        char* p = strstr(reply, "\r\n");
        _snscanf(reply, p - reply, "%s %u %s", okBuf, &mailsize, octetsBuf);
        
        mail->setSize(mailsize);
        MIMEParser::instance()->parseMail(p + 2, mail);
    }
    else {
        // 报告错误信息
        rstring errstr = "retrieve mail failed: ";
        errstr.append(retrRet > 0 ? reply : "IO error");
        report(errstr);
    }

    free(reply);
    return ret;
}

// 取回唯一标识为 uid 的邮件
// uid: 邮件序号，从0开始
// mail: 邮件指针，不能为空，需在外部生成对象，防止忘记释放资源
bool POP3Client::retrMail(const rstring& uid, Mail* mail)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "mail retrieving not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    size_t no = this->getNo(uid);
    if (no == 0) {
        // 没有找到这样一封信，报告错误信息
        rstring errstr = "retrieve mail ";
        errstr.append(uid);
        errstr.append(" failed: cannot find such a mail or encounter an IO error");
        report(errstr);

        return false;
    }

    return this->retrMail(no - 1, mail);
}

// 删除第 i 封邮件，删除邮件后列表变更，注意及时更新邮件列表
// i: 邮件序号，从0开始
bool POP3Client::deleteMail(size_t i)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "mail deleting not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    char* reply;
    int replyLen, deleRet;
    bool ret = false;

    // i+1 为删除邮件所使用的序号
    if ((deleRet = dele(i + 1, &reply, &replyLen)) == 0) {
        // 删除成功
        ret = true;

        rstring msg = "mail ";
        msg.append(std::to_string(i + 1));
        msg.append(" deleted");
        report(msg);
    }
    else {
        // 报告错误信息
        rstring errstr = "delete mail ";
        errstr.append(std::to_string(i + 1));
        errstr.append(" failed: ");
        errstr.append(deleRet > 0 ? reply : "IO error");
        report(errstr);
    }

    free(reply);
    return ret;
}

// 删除指定 UID 的邮件，删除后列表变更，注意及时更新邮件列表
// uid: 邮件 Unique ID
bool POP3Client::deleteMail(const rstring& uid)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "mail deleting not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    size_t no = this->getNo(uid);
    if (no == 0) {
        // 没有找到这样一封信，报告错误信息
        rstring errstr = "delete mail ";
        errstr.append(uid);
        errstr.append(" failed: cannot find such a mail or encounter an IO error");
        report(errstr);

        return false;
    }

    return this->deleteMail(no - 1);
}

// 删除指定的多个 UID 的邮件，删除后列表变更，注意及时更新邮件列表
// uids: 多个邮件 Unique ID
// completed: 成功删除的邮件 UID 列表
void POP3Client::deleteMails(const slist& uids, slist& completed)
{
    if (mState != POP3State::Transaction) {
        // 不处于事务状态
        rstring errstr = "mail deleting not available in state ";
        errstr += static_cast<int>(mState);
        report(errstr);
    }

    for (rstring s : uids) {
        if (deleteMail(s)) {
            completed.emplace_back(s);
        }
    }
}

// 获取 uid 对应邮件的邮件序号，从 1 开始
// uid: 邮件uid字符串
// return: 邮件序号，从 1 开始
size_t POP3Client::getNo(const rstring& uid)
{
    char* reply;
    char buf[10];
    int replyLen, uidlRet;
    size_t mailnum = 0;
    unsigned long long totsize;
    size_t ret = 0;

    if ((uidlRet = this->uidl(&reply, &replyLen)) == 0) {
        // 解析命令返回字符串
        char* p = strstr(reply, "\r\n");		// 第一行状态行
        _snscanf(reply, p - reply, "%s %u %u", buf, &mailnum, &totsize);

        // uidl 指令回复不标准
        if (mailnum == 0) {
            if (!this->getStatus(mailnum, totsize)) {
                // 单独的出错处理
                report("cannot get status correctly");
                free(reply);
                return 0;
            }
        }

        size_t no;
        char _uid[BUFFER_SIZE];
        p += 2;
        // 读取每一行的数据
        for (size_t i = 0; i < mailnum; ++i) {
            // 获取下一行的
            char* ptemp = strstr(p + 2, "\r\n");
            _snscanf(p, ptemp - p, "%u %s", &no, _uid);
            
            if (uid.compare(_uid) == 0) {
                ret = no;
                break;
            }

            p = ptemp + 2;		// 下一行
        }
    }

    free(reply);
    return ret;
}

// 在主机域名前加上 pop 前缀
// return: 指向拼接后的新字符串的指针，注意使用后应使用 delete[] 释放该字符串
char* POP3Client::prefix(const char* host)
{
    // 获取字符串长度
    int prefixlen = strlen(RESERVED_POP3_PREFIX);
    int hostlen = strlen(host);
    // 分配内存
    char* outs = new char[hostlen + prefixlen + 2];
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
    if (resp[0] == '+')
        return true;

    return false;
}

// 回复单行内容的命令
// cmd: 命令字符串
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 配合 reply 输出新分配空间的大小
// return: 正常回复 0，回复错误 1，读写错误或内存分配失败 -1
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
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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
        ret = cmdOK(*reply) ? 0 : 1;
    }

    return ret;
}

// 连接到 POP3 服务器
// addr: 服务器地址
// port: POP3 服务端口号
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::conn(const char* addr, USHORT port, char** reply, int* outlen)
{
    this->mConn->connect(addr, port);

    char buf[BUFFER_SIZE];		// 读缓冲区

    int r = mConn->readline(buf, BUFFER_SIZE);		// 读取回复

    // 检查读错误或无回复
    if (r <= 0) {
        this->mConn->closeSocket();		// 关闭套接字
        return -1;
    }

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

// POP3 USER 命令，指定用户名
// usr: 用户名
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::apop(const char* name, const char* digest, char** reply, int* outlen)
{
    char buf[BUFFER_SIZE];

    sprintf(buf, "APOP %s %s\r\n", name, digest);

    return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 QUIT 命令，退出连接，同时断开 socket 连接
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 为给 reply 分配的内存空间的大小，一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int POP3Client::retr(char** reply, int* outlen, size_t no)
{
    char buf[BUFFER_SIZE];		// 缓冲区
    sprintf(buf, "RETR %u\r\n", no);

    return cmdWithMultiLinesReply(buf, "\r\n.\r\n", reply, outlen);
}

// POP3 TOP 命令，取特定序号邮件的头部和信体的指定行数内容
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 为给 reply 分配的内存空间的大小，一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// no: 指定返回特定序号邮件的相关内容
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int POP3Client::top(char** reply, int* outlen, size_t no, size_t lines)
{
    char buf[BUFFER_SIZE];		// 缓冲区
    sprintf(buf, "TOP %u %u\r\n", no, lines);

    return cmdWithMultiLinesReply(buf, "\r\n.\r\n", reply, outlen);
}

// POP3 DELE 命令，删除指定序号邮件
// no: 指定删除特定序号邮件
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::dele(size_t no, char** reply, int* outlen)
{
    char buf[BUFFER_SIZE];		// 缓冲区

    sprintf(buf, "DELE %u\r\n", no);		// PASS 命令放入缓冲区

    return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 REST 命令，复位POP3会话，标志删除的邮件取消删除标记
// reply: 传入 nullptr(默认) 表示不关心回复；
//		  非空时为指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 配合 reply 输出新分配空间的大小
// 正常回复返回 0，命令回复错误返回 1，读写错误或无回复或内存分配失败返回 -1
int POP3Client::rest(char** reply, int* outlen)
{
    char buf[BUFFER_SIZE];		// 缓冲区

    sprintf(buf, "REST\r\n");		// PASS 命令放入缓冲区

    return cmdWithSingLineReply(buf, reply, outlen);
}

// POP3 UIDL 命令，查看邮件唯一标识，指定序号返回单行，不指定返回多行
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
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

// POP3 capa 命令，查看服务端兼容命令列表
// reply: 指向字符指针的指针，若结果返回0，则将reply指向的指针指向一块新的内存；
//		  使用完毕应使用free()清除
// outlen: 为给 reply 分配的内存空间的大小，一定为 BUFFER_SIZE 的倍数 + 1（可能多余实际长度）
// 正常回复返回0，回复错误返回1，读写错误及内存分配失败返回-1
int POP3Client::capa(char** reply, int* outlen)
{
    return cmdWithMultiLinesReply("CAPA\r\n", "\r\n.\r\n", reply, outlen);
}

// 报告信息（添加类名）
// msg: 字符串信息
void POP3Client::report(const rstring& msg)
{
    LogUtil::report("POP3Client " + msg);
}