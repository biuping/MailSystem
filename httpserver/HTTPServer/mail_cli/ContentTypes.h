#ifndef _CONTENT_TYPES_H_
#define _CONTENT_TYPES_H_

#include <string>
#include <map>

typedef std::string rstring;
typedef std::map<rstring, rstring> str_kvmap;


/* 定义邮件的内容相关类型 */

/* 邮件内容编码方式 */
enum class ContentTransferEncoding
{
	SevenBit,
	EightBit,
	QuotedPrintable,
	Base64,
	Binary
};

/* 邮件内容类型 */
typedef struct {
	rstring media;
	rstring boundary;
	rstring charset;
	rstring name;
	str_kvmap params;
} mail_content_type_t;


typedef struct mail_content_disposition_t {
	long long size;
	rstring filename;
	rstring type;
	rstring creation_date;
	rstring modification_date;
	rstring read_date;
	str_kvmap params;
	mail_content_disposition_t(): size(0) {}
} mail_content_disposition_t;



#endif // !_CONTENT_TYPES_H_

