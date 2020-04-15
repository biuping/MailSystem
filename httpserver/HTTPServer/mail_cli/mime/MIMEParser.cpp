#include "MIMEParser.h"


MIMEParser* MIMEParser::mInstance = nullptr;

MIMEParser::MIMEParser()
{
}

MIMEParser::~MIMEParser()
{
}

// singleton
MIMEParser* MIMEParser::instance()
{
	if (MIMEParser::mInstance == nullptr) {
		mInstance = new MIMEParser();
	}

	return MIMEParser::mInstance;
}

// 解析邮件原始字符串，返回邮件内容
// raw: 邮件原始字符串
// mail: 邮件指针
void MIMEParser::parseMail(const rstring& raw, Mail* mail)
{
	// 寻找信头信体空行分隔
	size_t pos = raw.find("\r\n\r\n", 0);
	mail_header_t header;
	mail_body_t body;
	
	str_citer begin = raw.begin();
	str_citer end = raw.end();
	if (pos != rstring::npos) {
		// 解析信头部分
		parseHeader(begin, begin+pos, header);
	}
	else {
		pos = 0;
	}
	// 解析信体部分
	size_t endpos = raw.find("\r\n.\r\n");
	parseBody(begin + pos + 4, endpos == rstring::npos ? end : begin + endpos + 2, body);

	mail->setHeader(header);
	mail->setBody(body);
}

// 解析信头
// begin: 字符串开始位置const_iterator
// end: 字符串结束位置const_iterator
// header: 信头结构引用
void MIMEParser::parseHeader(const str_citer& begin, const str_citer& end, mail_header_t& header)
{
	str_kv_t field;			// 字段
	size_t sz = 0;		// 辅助迭代器进行定位，已读字符数

	while (begin + sz < end) {
		// 取出一对字段
		size_t temp = extractField(begin + sz, end, field);
		if (temp == 0) {
			break;
		}
		sz += temp;
		setHeaderField(header, field);
	}
}

// 解析信体
// begin: 字符串开始位置const_iterator
// end: 字符串结束位置const_iterator
// body: 信体结构引用
void MIMEParser::parseBody(const str_citer& begin, const str_citer& end, mail_body_t& body)
{
	body.message = rstring(begin, end);
}

// 跳过空白字符
// begin: 字符串开始位置const_iterator
// end: 字符串结束位置const_iterator
// return: 跳过的字符数
size_t MIMEParser::skipWhiteSpaces(const str_citer& begin, const str_citer& end)
{
	size_t tot = 0;
	// skip white spaces
	while (begin < end && strchr(whitespaces, *(begin + tot))) {
		++tot;
	}
	return tot;
}

// 从 raw 中抽取一个字段，不进行特定解码
// begin: 原始字符串
// field: 将字段放入该结构内
// return: 返回该字段在原始字符串中的长度
size_t MIMEParser::extractField(const str_citer& begin, const str_citer& end, str_kv_t& field)
{
	if (*begin == '\r') {
		// 出现空行
		return 0;
	}

	size_t keysz = 0, valsz = 0;
	// 字段名解析
	while (begin + keysz < end) {
		if (*(begin + keysz) == ':') {
			field.first = rstring(begin, begin + keysz);
			++keysz;
			break;
		}
		
		++keysz;
	}

	// 跳过空白符
	keysz += skipWhiteSpaces(begin + keysz, end);

	// 字段值解析
	field.second.clear();
	while (begin + keysz + valsz < end) {
		char ch = *(begin + keysz + valsz);
		if (ch == '\r') {
			field.second.append(rstring(begin + keysz, begin + keysz + valsz));
			// 跳过换行符
			valsz += 2;
			// 包含空白符则表示有多行值
			if (skipWhiteSpaces(begin + keysz + valsz, end) > 0) {
				continue;
			}
			else {
				break;
			}
		}

		++valsz;
	}

	return keysz + valsz;
}

// 通过字段名设置信头不同属性
// header: 信头结构引用
// field: 字段键值对
void MIMEParser::setHeaderField(mail_header_t& header, const str_kv_t& field)
{
	// 字段名
	rstring key = field.first;
	
	// 跟绝字段名进行具体设置
	if (_strnicmp(key.c_str(), "Subject", 7) == 0) {
		setSubject(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "Date", 4) == 0) {
		setDate(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "From", 4) == 0) {
		setFrom(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "To", 2) == 0) {
		setTo(header, field.second);
	}
	else {
		setOthers(header, key, field.second);
	}
}

// 根据原始主题字符串设置信头主题属性
// header: 信头结构引用
// subject: 主题原始字符串
void MIMEParser::setSubject(mail_header_t& header, const rstring& subject)
{
	header.subject = subject;
}

// 根据原始日期字符串设置信头日期属性
// header: 信头结构引用
// date: 日期原始字符串
void MIMEParser::setDate(mail_header_t& header, const rstring& date)
{
	header.date = date;
}

// 根据原始发件人字符串设置信头发件人属性
// header: 信头结构引用
// from: 发件人原始字符串
void MIMEParser::setFrom(mail_header_t& header, const rstring& from)
{
	header.from = from;
}

// 根据原始收件人字符串设置信头收件人属性
// header: 信头结构引用
// to: 收件人原始字符串
void MIMEParser::setTo(mail_header_t& header, const rstring& to)
{
	// TODO: to 可能有多个
	header.to.clear();
	header.to.push_back(to);
}

// 根据原始其他字段的字段名和值字符串设置信头属性
// header: 信头结构引用
// key: 字段名
// val: 字段值原始字符串
void MIMEParser::setOthers(mail_header_t& header, const rstring & key, const rstring & val)
{
	header.xfields.insert(std::make_pair(key, val));
}
