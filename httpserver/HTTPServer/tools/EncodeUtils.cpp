#include "EncodeUtils.h"
#include "CharsetUtils.h"
#include "GeneralUtils.h"


// encode 'text' into base64 str into 'buf'
const char* EncodeUtil::base64_encode(const unsigned char* text, size_t size, char* buf, size_t* out_len)
{
    char* head_buf = buf;
    static const char* base64_encoding =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t buflen = 0;
    while (size > 0)
    {
        *buf++ = base64_encoding[(text[0] >> 2) & 0x3f];
        if (size > 2)
        {
            *buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)];
            *buf++ = base64_encoding[((text[1] & 0xF) << 2) | (text[2] >> 6)];
            *buf++ = base64_encoding[text[2] & 0x3F];
        }
        else
        {
            switch (size)
            {
            case 1:
                *buf++ = base64_encoding[(text[0] & 3) << 4];
                *buf++ = '=';
                *buf++ = '=';
                break;
            case 2:
                *buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)];
                *buf++ = base64_encoding[((text[1] & 0x0F) << 2) | (text[2] >> 6)];
                *buf++ = '=';
                break;
            }
        }

        text += 3;
        size -= 3;
        buflen += 4;
    }
    *buf = 0;
    *out_len = buflen;
    //    return buflen;
    return head_buf;
}

static inline
char GetBase64Value(char ch)
{
    if ((ch >= 'A') && (ch <= 'Z'))
        return ch - 'A';
    if ((ch >= 'a') && (ch <= 'z'))
        return ch - 'a' + 26;
    if ((ch >= '0') && (ch <= '9'))
        return ch - '0' + 52;
    switch (ch)
    {
    case '+':
        return 62;
    case '/':
        return 63;
    case '=': /* base64 padding */
        return 0;
    default:
        return 0;
    }
}

// decode base64 str 'text' into 'buf'
size_t EncodeUtil::base64_decode(const char* text, size_t size, char* buf)
{
    if (size % 4)
        return -1;
    unsigned char chunk[4];
    size_t parsenum = 0;
    while (size > 0)
    {
        chunk[0] = GetBase64Value(text[0]);
        chunk[1] = GetBase64Value(text[1]);
        chunk[2] = GetBase64Value(text[2]);
        chunk[3] = GetBase64Value(text[3]);

        *buf++ = (chunk[0] << 2) | (chunk[1] >> 4);
        *buf++ = (chunk[1] << 4) | (chunk[2] >> 2);
        *buf++ = (chunk[2] << 6) | (chunk[3]);

        text += 4;
        size -= 4;
        parsenum += 3;
    }
    return parsenum;
}

// decode quoted printable
// encoded: ���봮
// decoded: ���������
// variant: true��ʾʹ�� rfc2047 decoding��encoded word ʹ��
//          false ��ʾʹ�� rfc2045 decodig��content-transfer-encoding ʹ��
void EncodeUtil::quoted_printable_decode(const rstring& encoded, rstring& decoded, bool variant)
{
    decoded.clear();

    // �����ַ�
    for (size_t i = 0; i < encoded.size(); ++i) {
        if (encoded[i] == '=') {
            // ���������2���ַ��� '=' ��
            if (encoded.size() - i < 3) {
                rstring remain = encoded.substr(i);
                EncodeUtil::decodeMissingBehindEqual(remain);
                decoded.append(remain);

                break;
            }

            // ���� qp
            rstring qp = encoded.substr(i, 3);
            EncodeUtil::decodeEqual(qp);
            i += 2;
        }
        else {
            // �Ǳ����ַ�
            if (encoded[i] == '_' && variant) {
                // _ ��ʾ�ո�0x20��
                decoded.append(std::to_string((char)0x20));
            }
            else {
                decoded.append(std::to_string(encoded[i]));
            }
        }
    }
}

// ʹ�ò�ͬ�ַ����� ascii �ַ������ʼ��ַ���������
// asciitext: ascii �ַ���
// charset: �ַ���
const rstring EncodeUtil::convert2UTF8(const rstring& bytes, const rstring& charset)
{
    // TODO: ���Լ�������
    if (GeneralUtil::strEquals(charset.c_str(), "binary", true) ||
        GeneralUtil::strEquals(charset.c_str(), "US-ASCII", true) ||
        GeneralUtil::strStartWith(charset.c_str(), "iso-8859-", true) ||
        GeneralUtil::strEquals(charset.c_str(), "ANSI", true)) {
        // ascii
        return CharsetUtil::AnsiToUtf8(bytes);
    }
    else if (GeneralUtil::strEquals(charset.c_str(), "UTF-8", true)) {
        // utf8
        return bytes;
    }
    else if (GeneralUtil::strEquals(charset.c_str(), "BIG5", true)) {
        return CharsetUtil::FBIG5ToGB2312(bytes);
    }
    else if (GeneralUtil::strEquals(charset.c_str(), "gb2312", true) ||
        GeneralUtil::strEquals(charset.c_str(), "GBK", true)) {
        // gb2312 �� gbk
        return CharsetUtil::GBKToUtf8(bytes);
    }
    else if (GeneralUtil::strStartWith(charset.c_str(), "gb", true)) {
        // ���� gb �淶��Ŀǰ���� GBK ����ת��
        return CharsetUtil::GBKToUtf8(bytes);
    }
    else {
        return CharsetUtil::AnsiToUtf8(bytes);
    }
}

void EncodeUtil::removeIllegalCtlChars(rstring& raw)
{
    EncodeUtil::removeCarriageNewLineNotPaired(raw);

    // ʣ�ºϷ��� \r\n ������ \t
    // \x0A = \n
    // \x0D = \r
    // \x09 = \t
    raw = std::regex_replace(raw, r_regex("[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]"), "");
}

void EncodeUtil::removeCarriageNewLineNotPaired(rstring& raw)
{
    // ʹ���´�������
    rstring newstr = rstring(raw.size(), '\0');
    newstr.clear();

    for (size_t i = 0; i < raw.size(); ++i) {
        if ( !(raw[i] == '\r' && (i + 1 >= raw.size() || raw[i + 1] != '\n')) &&
            !(raw[i] == '\n' && (i == 0 || raw[i - 1] != '\r'))) {
            // ֻ�в��� lonely \r or \n �ż����´�
            newstr.append(std::to_string(raw[i]));
        }
    }

    // ���
    raw = newstr;
}

void EncodeUtil::decodeMissingBehindEqual(rstring& decoded)
{
    // ��Ϊ�Ⱥź󳤶Ȳ�����Ϊδ���б���Ĳ��֣������иĶ�
}

void EncodeUtil::decodeEqual(rstring& qp)
{
    // �����ϸ�ʽ
    if (qp.size() != 3 || qp[0] != '=')
        return;

    if (qp.find("\r\n") != rstring::npos) {
        // soft break, ���ؿ��ִ�
        qp = "";
    }
    else {
        char pDst[sizeof(int) + 1] = { 0 };
        // ����ʮ��������ֵ��ʾ8bit�ַ�
        sscanf(qp.c_str(), "=%02x", (unsigned int*)pDst);
        qp = pDst;
    }
}
