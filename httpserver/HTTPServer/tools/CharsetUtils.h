#ifndef _CHARSET_UTIL_H_
#define _CHARSET_UTIL_H_

#include <string>
#include <iostream>

typedef std::string rstring;
typedef std::wstring rwstring;


namespace CharsetUtil
{
    /* charset conversion */
    const rstring  UnicodeToAnsi(const rwstring& unicode);
    const rwstring AnsiToUnicode(const rstring& ansi);

    const rstring  AnsiToUtf8(const rstring& strSrc);
    const rstring  Utf8ToAnsi(const rstring& strSrc);

    const rstring  UnicodeToUtf8(const rwstring& wstrSrc);
    const rwstring Utf8ToUnicode(const rstring& strSrc);

    const rstring  GBKToUtf8(const rstring& gbk);
    const rstring  Utf8ToGBK(const rstring& utf8);

    const rwstring GB2312ToUnicode(const rstring& gb2312);
    const rstring  UnicodeToGB2312(const rwstring& unicode);

    const rwstring BIG5ToUnicode(const rstring& big5);
    const rstring  UnicodeToBIG5(const rwstring& unicode);

    const rstring  FBIG5ToGB2312(const rstring& big5);
    const rstring  GB2312ToFBIG5(const rstring gb2312);

    bool IsUTF8(const void* pBuffer, long size);

    /* show hex utilities */
    void showHex(const char* bytes, int len);
    void showHex(const rstring& charset, const rstring& str);
    void showHex(const rstring& charset, const rwstring& str);

    const rstring wstring2string(const rwstring& wstr);
    const rwstring string2wstring(const rstring& str);
};




#endif // !_CHARSET_UTIL_H_
