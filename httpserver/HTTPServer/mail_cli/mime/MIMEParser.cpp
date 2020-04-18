#include "MIMEParser.h"
#include "../../tools/GeneralUtils.h"
#include "MIMEDecoder.h"
#include <stack>


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
		parseHeader(begin, begin + pos + 2, header);
	}
	else {
		pos = 0;
	}
	// 解析信体部分
	size_t endpos = raw.find("\r\n.\r\n");
	// 去掉结尾点之后的内容
	parseBody(begin + pos + 4, endpos == rstring::npos ? end : begin + endpos + 2, header, body);

	mail->setHeader(header);
	mail->setBody(body);
}

// 解析信头
// begin: 字符串开始位置const_iterator
// end: 字符串结束位置const_iterator
// header: 信头结构引用
void MIMEParser::parseHeader(const str_citer& begin, const str_citer& end, mail_header_t& header)
{
	if (begin >= end) {
		return;
	}

	str_kv_t field;			// 字段
	size_t sz = 0;		// 辅助迭代器进行定位，已读字符数

	while (sz < end - begin) {
		// 取出一个字段
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
void MIMEParser::parseBody(const str_citer& begin, const str_citer& end,
	const mail_header_t& header, mail_body_t& body)
{
	rstring raw = rstring(begin, end);
	cleanBody(raw);
	GeneralUtil::strTrim(raw);
	clearParts(body);

	if (GeneralUtil::strStartWith(header.content_type.media, "multipart/", true) &&
		header.content_type.boundary.size() > 0) {
		// has multipart
		// 解析多部分的信体内容
		slist parts;
		extractParts(begin, end, header.content_type.boundary, parts);
		
		for (rstring p : parts) {
			if (p.size() > 0) {
				MessagePart* part = new MessagePart();
				parseMessagePart(p, part);
				body.parts.push_back(part);
			}
		}
	}
	else {
		// non multipart message
		// 直接根据字符集与编码方式解码
		MIMEDecoder::decodeMailBody(raw, header.content_type.charset,
			header.content_transfer_encoding, body.message);
	}
}

// 解析内容类型
void MIMEParser::parseContentType(const str_citer& begin, const str_citer& end,
	mail_content_type_t& contentType)
{
	str_kvlist params;
	MIMEDecoder::rfc2231Decode(rstring(begin, end), params);

	bool mediaSet = false;
	for (str_kv_t kv : params) {
		rstring& key = kv.first;
		rstring& val = kv.second;
		GeneralUtil::strTrim(key);
		GeneralUtil::strTrim(val);
		GeneralUtil::strRemoveQuotes(val);

		// 解析字段
		if (key.size() == 0) {
			// media type
			if (!mediaSet) {
				// 检查 illegal type
				if (_strnicmp(val.c_str(), "text", 4) == 0 ||
					_strnicmp(val.c_str(), "text/", 5) == 0) {
					val = "text/plain";
				}

				cleanMediaType(val);
				contentType.media = val;
				mediaSet = true;
			}
			// else 有多个media type，只使用第一个
		}
		else if (_strnicmp(key.c_str(), "boundary", 8) == 0) {
			// set boundary
			contentType.boundary = val;
		}
		else if (_strnicmp(key.c_str(), "charset", 7) == 0) {
			// set charset
			contentType.charset = val;
		}
		else if (_strnicmp(key.c_str(), "name", 4) == 0) {
			// set name
			contentType.name = val;
		}
		else {
			// add parameters
			contentType.params.emplace(key, val);
		}
	}
}

// 解析内容排布
void MIMEParser::parseContentDispostion(const str_citer& begin, const str_citer& end,
	mail_content_disposition_t& contentDisposition)
{
	str_kvlist params;
	MIMEDecoder::rfc2231Decode(rstring(begin, end), params);

	for (str_kv_t kv : params) {
		rstring& key = kv.first;
		rstring& val = kv.second;

		GeneralUtil::strTrim(key);
		GeneralUtil::strTrim(val);
		GeneralUtil::strRemoveQuotes(val);

		if (key.size() == 0) {
			contentDisposition.type = val;
		}
		else if (_strnicmp(key.c_str(), "name", 4) == 0 ||
			_strnicmp(key.c_str(), "filename", 8) == 0) {
			rstring decoded = val;
			MIMEDecoder::decodeWord(val, decoded);
			contentDisposition.filename = decoded;
		}
		else if (_strnicmp(key.c_str(), "size", 4) == 0) {
			parseSize(val.begin(), val.end(), contentDisposition.size);
		}
		else if (_strnicmp(key.c_str(), "creation-date", 13) == 0) {
			contentDisposition.creation_date = val;
		}
		else if (_strnicmp(key.c_str(), "modification-date", 17) == 0) {
			contentDisposition.modification_date = val;
		}
		else if (_strnicmp(key.c_str(), "read-date", 9) == 0) {
			contentDisposition.read_date = val;
		}
		else {
			// add other params
			contentDisposition.params.emplace(key, val);
		}
	}
}

void MIMEParser::parseContentTransferEncoding(const str_citer& begin, const str_citer& end,
	ContentTransferEncoding& encoding)
{
	rstring val = rstring(begin, end);
	GeneralUtil::strTrim(val);

	if (_strnicmp(val.c_str(), "7bit", 4) == 0) {
		encoding = ContentTransferEncoding::SevenBit;
	}
	else if (_strnicmp(val.c_str(), "8bit", 4) == 0) {
		encoding = ContentTransferEncoding::EightBit;
	}
	else if (_strnicmp(val.c_str(), "quoted-printable", 16) == 0) {
		encoding = ContentTransferEncoding::QuotedPrintable;
	}
	else if (_strnicmp(val.c_str(), "base64", 6) == 0) {
		encoding = ContentTransferEncoding::Base64;
	}
	else if (_strnicmp(val.c_str(), "binary", 6) == 0) {
		encoding = ContentTransferEncoding::Binary;
	}
	else {
		// default use 7bit
		encoding = ContentTransferEncoding::SevenBit;
	}
}

void MIMEParser::parseSize(const str_citer& begin, const str_citer& end, long long& size)
{
	if (begin >= end) {
		size = 0;
	}

	rstring val = rstring(begin, end);
	GeneralUtil::strTrim(val);

	size_t passed = 0;
	while (passed < end - begin) {
		char ch = *(begin + passed);
		if ('0' > ch || '9' < ch) {
			break;
		}

		++passed;
	}

	if (passed == 0) {
		// invalid
		size = 0;
	}
	else {
		// 提取单位前的值
		double val = 0;
		sscanf(rstring(begin, begin + passed).c_str(), "%lf", &val);
		// 提取单位
		while (passed < end - begin && *(begin + passed) == ' ') {
			// 去掉空格
			++passed;
		}
		rstring unit = rstring(begin + passed, end);
		if (sizeMap.find(unit) == sizeMap.end()) {
			// 无法识别单位，采用 byte
			unit = "B";
		}
		long long multi = sizeMap.find(unit)->second;
		size = (long long)(val * multi);
	}
}

void MIMEParser::parseMessagePart(const rstring& raw, MessagePart* part)
{
	if (raw.size() == 0 || part == nullptr) {
		return;
	}

	// 寻找子部份的头部和信体的空行分隔
	size_t pos = raw.find("\r\n\r\n", 0);

	str_citer begin = raw.begin();
	str_citer end = raw.end();
	if (pos != rstring::npos) {
		// 解析子部份信头部分
		parseMessagePartHeader(begin, begin + pos + 2, part);
	}
	else {
		pos = 0;
	}
	// 解析子部分的信体
	parseMessagePartBody(begin + pos + 4, end, part);
}

void MIMEParser::parseMessagePartHeader(const str_citer& begin, const str_citer& end, MessagePart* part)
{
	if (begin >= end) {
		return;
	}

	str_kv_t field;			// 字段
	size_t sz = 0;		// 辅助迭代器进行定位，已读字符数

	while (sz < end - begin) {
		// 取出一个字段
		size_t temp = extractField(begin + sz, end, field);
		if (temp == 0) {
			break;
		}
		sz += temp;
		setPartHeaderField(part, field);
	}
}

void MIMEParser::parseMessagePartBody(const str_citer& begin, const str_citer& end, MessagePart* part)
{
	if (begin >= end || part == nullptr) {
		return;
	}

	rstring raw = rstring(begin, end);
	GeneralUtil::strTrim(raw);
	part->clearParts();

	if (GeneralUtil::strStartWith(part->getContentType().media, "multipart/", true) &&
		part->getContentType().boundary.size() > 0) {
		// has multipart
		// 解析多部分的信体内容
		slist parts;
		extractParts(begin, end, part->getContentType().boundary, parts);

		for (rstring p : parts) {
			if (p.size() > 0) {
				MessagePart* subpart = new MessagePart();
				parseMessagePart(p, subpart);
				part->addPartBack(subpart);
			}
		}
	}
	else {
		// non multipart message
		// 直接根据字符集与编码方式解码
		rstring message = "";
		MIMEDecoder::decodeMailBody(raw, part->getContentType().charset,
			part->getEncoding(), message);
		part->setMessage(message);
	}
}

// 从 raw 中抽取一个字段，不进行特定解码
// begin: 原始字符串
// field: 将字段放入该结构内
// return: 返回该字段在原始字符串中的长度
size_t MIMEParser::extractField(const str_citer& begin, const str_citer& end, str_kv_t& field)
{
	if (begin >= end) {
		return 0;
	}

	if (*begin == '\r' && 1 < end - begin && *(begin + 1) == '\n') {
		// 出现空行
		return 0;
	}

	size_t keysz = 0, valsz = 0;
	// 字段名解析
	while (keysz < end - begin) {
		if (*(begin + keysz) == ':') {
			field.first = rstring(begin, begin + keysz);
			++keysz;
			break;
		}
		
		++keysz;
	}

	// 跳过空白符
	keysz += GeneralUtil::strSkipWhiteSpaces(begin + keysz, end);

	// 字段值解析
	field.second.clear();
	while (keysz + valsz < end - begin) {
		size_t pos = keysz + valsz;

		if (*(begin + pos) == '\r' && begin + pos + 1 < end && *(begin + pos + 1) == '\n') {
			field.second.append(rstring(begin + keysz, begin + keysz + valsz));
			// unfolding，跳过换行符
			valsz += 2;
			keysz = keysz + valsz;
			valsz = 0;
			// 包含空白符则表示有多行值
			if (GeneralUtil::strSkipWhiteSpaces(begin + keysz + valsz, end) > 0) {
				continue;
			}
			else {
				break;
			}
		}

		++valsz;
	}

	if (keysz + valsz == end - begin) {
		// 结尾无换行符
		field.second.append(rstring(begin + keysz, begin + keysz + valsz));
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
	else if (_strnicmp(key.c_str(), "Received", 8) == 0) {
		setReceived(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "Mime-Version", 12) == 0) {
		setMimeVersion(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "To", 2) == 0) {
		setTo(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "Cc", 2) == 0) {
		setCc(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "Bcc", 4) == 0) {
		setBcc(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "Content-Type", 12) == 0) {
		setHeaderContentType(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "Content-Transfer-Encoding", 25) == 0) {
		setHeaderContentTransferEncoding(header, field.second);
	}
	else if (_strnicmp(key.c_str(), "Content-Disposition", 19) == 0) {
		setHeaderContentDisposition(header, field.second);
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
	GeneralUtil::strTrim(header.subject);
	MIMEDecoder::decodeWord(subject, header.subject);
}

// 根据原始日期字符串设置信头日期属性
// header: 信头结构引用
// date: 日期原始字符串
void MIMEParser::setDate(mail_header_t& header, const rstring& date)
{
	header.date = date; 
	GeneralUtil::strTrim(header.date);
}

// 根据原始发件人字符串设置信头发件人属性
// header: 信头结构引用
// from: 发件人原始字符串
void MIMEParser::setFrom(mail_header_t& header, const rstring& from)
{
	parseSingleMailAddr(from, header.from);
}

// 根据原始回复字符串设置信头回复属性
// header: 信头结构引用
// replyto: 回复原始字符串
void MIMEParser::setReplyTo(mail_header_t& header, const rstring& replyto)
{
	parseSingleMailAddr(replyto, header.reply_to);
}

// 根据原始sender字符串设置信头sender属性
// header: 信头结构引用
// sender: sender原始字符串
void MIMEParser::setSender(mail_header_t& header, const rstring& sender)
{
	parseSingleMailAddr(sender, header.sender);
}

// 根据原始Received字符串设置信头received属性
// header: 信头结构引用
// received: Received原始值
void MIMEParser::setReceived(mail_header_t& header, const rstring& received)
{
	header.received.names.clear();

	rstring recvd = received;
	GeneralUtil::strTrim(recvd);

	size_t datepos = recvd.rfind(';');
	if (datepos != rstring::npos) {
		header.received.date = recvd.substr(datepos + 1);
		GeneralUtil::strTrim(header.received.date);
		recvd.erase(datepos);
	}

	// 保留一个空格
	recvd = std::regex_replace(recvd, r_regex("\\s+"), " ");

	// 匹配 name value 对
	r_regex nameValRegex = r_regex(R"(([^\s]+)\s([^\s]+(\s\(.+?\))*))");
	r_smatch matches;
	while (std::regex_search(recvd, matches, nameValRegex)) {
		if (matches.str(1)[0] == '(') {
			// () 内容表示 comment
			continue;
		}
		
		rstring name = matches.str(1);
		rstring val = matches.str(2);

		if (header.received.names.find(name) == header.received.names.end()) {
			// 新 name
			header.received.names.emplace(name, val);
		}

		recvd = matches.suffix();
	}
}

// 设置 MIME 协议版本
// header: 信头结构引用
// version: mime-version原始值
void MIMEParser::setMimeVersion(mail_header_t& header, const rstring& version)
{
	header.mime_version = version;
	GeneralUtil::strTrim(header.mime_version);
}

// 根据原始收件人字符串设置信头收件人属性
// header: 信头结构引用
// to: 收件人原始字符串
void MIMEParser::setTo(mail_header_t& header, const rstring& to)
{
	header.to.clear();
	parseRfcMailAddrs(to, header.to);
}

// 根据原始抄写字符串设置信头抄写属性
// header: 信头结构引用
// cc: 抄写原始字符串
void MIMEParser::setCc(mail_header_t& header, const rstring& cc)
{
	header.cc.clear();
	parseRfcMailAddrs(cc, header.cc);
}

// 根据原始密送字符串设置信头密送属性
// header: 信头结构引用
// bcc: 密送原始字符串
void MIMEParser::setBcc(mail_header_t& header, const rstring& bcc)
{
	header.bcc.clear();
	parseRfcMailAddrs(bcc, header.bcc);
}

// 根据原始内容类型字符串设置信头content-type属性
// header: 信头结构引用
// contentType: 内容类型原始字符串
void MIMEParser::setHeaderContentType(mail_header_t& header, const rstring& contentType)
{
	parseContentType(contentType.begin(), contentType.end(), header.content_type);
}

// 根据原始内容传输编码字符串设置信头相应属性
// header: 信头结构引用
// encoding: 内容传输编码原始字符串
void MIMEParser::setHeaderContentTransferEncoding(mail_header_t& header, const rstring& encoding)
{
	parseContentTransferEncoding(encoding.begin(), encoding.end(), header.content_transfer_encoding);
}

void MIMEParser::setHeaderContentDisposition(mail_header_t& header, const rstring& disposition)
{
	parseContentDispostion(disposition.begin(), disposition.end(), header.content_disposition);
}

// 根据原始其他字段的字段名和值字符串设置信头属性
// header: 信头结构引用
// key: 字段名
// val: 字段值原始字符串
void MIMEParser::setOthers(mail_header_t& header, const rstring & key, const rstring & val)
{
	header.xfields.insert(std::make_pair(key, val));
}

// 解析邮件地址用户名和邮箱地址，可能有多项值
// raw: 原始字符串
// addrlist: 邮箱地址列表引用
void MIMEParser::parseRfcMailAddrs(const rstring& raw, maddr_list& addrlist)
{
	// split with ,
	slist mailaddrs;
	GeneralUtil::strSplitIgnoreQuoted(raw, ',', mailaddrs);

	// 逐个解析，添加到列表中
	for (const rstring s : mailaddrs) {
		mail_addr_t addr;
		MIMEParser::parseSingleMailAddr(s, addr);
		addrlist.push_back(addr);
	}
}

// 解析单个地址
// raw: 单地址原始字符串
// addr: 邮箱地址引用
void MIMEParser::parseSingleMailAddr(const rstring& raw, mail_addr_t& addr)
{
	// 去除空格并解码
	rstring addrstr = raw;
	GeneralUtil::strTrim(addrstr);
	rstring decoded;
	MIMEDecoder::decodeWord(addrstr, decoded);

	// 去除尾部邮件地址的 <>
	// 先取最后出现的 <> 位置
	int lastOpen = decoded.rfind('<');
	int lastClosed = decoded.rfind('>');

	// 寻找相邻最先出现的 <> 位置
	int firstOpen = lastOpen;
	int firstClosed = lastClosed;
	while (firstOpen > 0 && decoded[firstOpen - 1] == '<' && 
		decoded[firstClosed - 1] == '>') {
		--firstOpen;
		--firstClosed;
	}

	if (firstOpen != lastOpen) {
		// multiple brackets
		// 保留一对 <>
		decoded = decoded.substr(0, firstOpen) +
			decoded.substr(lastOpen, firstClosed - lastOpen + 1);
	}

	// 定位尾部邮件位置
	int emailstart = decoded.rfind('<');
	int emailend = decoded.rfind('>');

	addr.name = "";
	if (emailstart >= 0 && emailend >= 0) {
		if (emailstart > 0) {
			// 邮件地址前有用户名信息
			addr.name = decoded.substr(0, emailstart);
			GeneralUtil::strTrim(addr.name);
		}

		addr.addr = decoded.substr(emailstart + 1, emailend - emailstart - 1);
		GeneralUtil::strTrim(addr.addr);
	}
	else {
		// 认为只包含邮件地址
		addr.addr = decoded;
	}
}

// 清理 media-type 中的非法字符
void MIMEParser::cleanMediaType(rstring& mediatype)
{
	size_t typeEnd = mediatype.find('/');
	if (typeEnd == rstring::npos || typeEnd == mediatype.size()-1) {
		// invalid
		return;
	}

	// extract type and subtype
	rstring type = mediatype.substr(0, typeEnd);
	rstring subtype = mediatype.substr(typeEnd + 1);
	
	stripRfc2045TSpecials(type);
	stripRfc822Ctls(type);
	GeneralUtil::strReplaceAll(type, " ", "");

	stripRfc2045TSpecials(subtype);
	stripRfc822Ctls(subtype);
	GeneralUtil::strReplaceAll(subtype, " ", "");

	// output cleaned
	mediatype = type + "/" + subtype;
}

// 清理 Rfc2045 中说明的特殊字符
void MIMEParser::stripRfc2045TSpecials(rstring& raw)
{
	char* buf = new char[raw.size() + 3];
	size_t sz = 0;
	for (size_t i = 0; i < raw.size(); ++i) {
		if (strchr(tspecials, raw[i]) == nullptr) {
			buf[sz++] = raw[i];
		}
		// else 为非法字符，去除
	}
	buf[sz] = '\0';
	
	raw = rstring(buf);
	delete[] buf;
}

// 清理 Rfc822 中说明的控制字符
void MIMEParser::stripRfc822Ctls(rstring& raw)
{
	char* buf = new char[raw.size() + 3];
	int sz = 0;
	for (int i = 0; i < raw.size(); ++i) {
		if (((char)0 <= raw[i] && (char)31 >= raw[i]) ||
			(char)127 == raw[i]) {
			// ASCII Control Chars or Del
			continue;
		}
		buf[sz++] = raw[i];
	}
	buf[sz] = '\0';

	raw = rstring(buf);
	delete[] buf;
}

// 对 body 中填充的内容进行清理（点填充）
void MIMEParser::cleanBody(rstring& rawbody)
{
	size_t sz = 0;
	rstring cleaned = "";
	str_citer begin = rawbody.begin();
	str_citer end = rawbody.end();

	while (sz < end - begin) {
		// 查看一行
		size_t linepos = GeneralUtil::strFindLineEnd(begin + sz, end);
		size_t lineEnd = 0;
		if (linepos == rstring::npos) {
			// 将 linepos 和 lineEnd 均设为 end 位置
			linepos = end - begin - sz;
			lineEnd = end - begin - sz;
		}
		else {
			// 将 lineEnd 设为跳过 \r\n 之后的字符
			lineEnd = linepos + 2;
		}

		if (linepos == 2 && *(begin + sz) == '.' && *(begin + sz + 1) == '.') {
			// 一行内容仅为 ..\r\n，跳过第一个字符
			cleaned.append(begin + sz + 1, begin + sz + lineEnd);
		}
		else {
			// 直接加入
			cleaned.append(begin + sz, begin + sz + lineEnd);
		}

		sz += lineEnd;
	}
}

void MIMEParser::extractParts(const str_citer& begin, const str_citer& end,
	const rstring& boundary, slist& parts)
{
	if (begin >= end) {
		return;
	}

	parts.clear();
	size_t passed = 0;
	size_t boundstart = 0;

	boundstart = findBoundary(begin, end, boundary);
	if (boundstart == rstring::npos ||
		GeneralUtil::strStartWith(rstring(begin+boundstart, end), "--" + boundary + "--", true)) {
		// 找不到起始边界入口
		// 或找到的是结束边界
		return;
	}
	
	// 边界起始前的字节以及之后的换行符忽略（如果没有换行符则认为报文已经结束）
	// 注意边界的大小为 boundary.size() + 2（前缀 -- ）
	passed += boundstart + boundary.size() + 4;
	while (passed < end - begin) {
		size_t next = findBoundary(begin + passed, end, boundary);
		if (next == rstring::npos) {
			// 找不到下一个边界，认为剩下的部分均为part内的
			parts.emplace_back(begin + passed, end);
			passed = end - begin;
		}
		else {
			// 找到了下一个边界
			if (next != 0) {
				// 防止出现空part
				parts.emplace_back(begin + passed, begin + passed + next);
			}
			// 加上边界长度（2为前缀 -- ）
			passed += next + boundary.size() + 2;
			if (*(begin + passed) == '-' &&
				passed + 1 < end - begin &&
				*(begin + passed + 1) == '-') {
				// 为结束边界
				return;
			}
			passed += 2;		// 否则应以 \r\n 结尾
		}
	}
}

void MIMEParser::parseMessage(const rstring& raw, rstring& message)
{
}

size_t MIMEParser::findBoundary(const str_citer& begin, const str_citer& end, const rstring& boundary)
{
	if (begin >= end) {
		return rstring::npos;
	}

	size_t passed = 0;
	rstring line;
	
	// 遍历每一行
	while (passed < end - begin) {
		size_t linepos = GeneralUtil::strFindLineEnd(begin + passed, end);
		if (linepos != rstring::npos) {
			line = rstring(begin + passed, begin + passed + linepos);
		}
		else {
			line = rstring(begin + passed, end);
		}

		if (GeneralUtil::strStartWith(line, "--" + boundary, true)) {
			// 找到边界
			return passed;
		}

		passed += line.size() + 2;
	}

	return rstring::npos;
}

void MIMEParser::setPartHeaderField(MessagePart* part, const str_kv_t& field)
{
	if (part == nullptr) {
		return;
	}

	// 字段名
	rstring key = field.first;

	// 跟绝字段名进行具体设置
	if (_strnicmp(key.c_str(), "Content-Type", 12) == 0) {
		mail_content_type_t temp;
		parseContentType(field.second.begin(), field.second.end(), temp);
		part->setContentType(temp);
	}
	else if (_strnicmp(key.c_str(), "Content-Transfer-Encoding", 25) == 0) {
		ContentTransferEncoding encoding = ContentTransferEncoding::SevenBit;
		parseContentTransferEncoding(field.second.begin(), field.second.end(),
			encoding);
		part->setEncoding(encoding);
	}
	else if (_strnicmp(key.c_str(), "Content-Disposition", 19) == 0) {
		mail_content_disposition_t disposition;
		parseContentDispostion(field.second.begin(), field.second.end(), disposition);
		part->setContentDisposition(disposition);
	}
	else {
		part->setOrAddParam(key, field.second);
	}
}
