#include "EncodeUtils.h"


// encode 'text' into base64 str into 'buf'
const char* base64_encode(const unsigned char* text, int size, char* buf, int* out_len)
{
    char* head_buf = buf;
    static const char* base64_encoding =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int buflen = 0;
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
int base64_decode(const char* text, int size, char* buf)
{
    if (size % 4)
        return -1;
    unsigned char chunk[4];
    int parsenum = 0;
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