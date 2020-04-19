#ifndef _CONTENT_TYPES_H_
#define _CONTENT_TYPES_H_

#include <string>
#include <map>

typedef std::string rstring;
typedef std::map<rstring, rstring> str_kvmap;


/* �����ʼ�������������� */

/* �ʼ����ݱ��뷽ʽ */
enum class ContentTransferEncoding
{
	SevenBit,
	EightBit,
	QuotedPrintable,
	Base64,
	Binary
};

/* �ʼ��������� */
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

