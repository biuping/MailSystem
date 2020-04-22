#include "CharsetUtils.h"
#include <Windows.h>
#include "GeneralUtils.h"


// 效果等同于 wstring to string
const rstring CharsetUtil::UnicodeToAnsi(const rwstring& unicode)
{
    LPCWCH ptr = unicode.c_str();
    /** 分配目标空间, 一个16位Unicode字符最多可以转为4个字节int size = static_cast<int>( wstrSrc.size() * 4 + 10 );*/
    int size = WideCharToMultiByte(CP_THREAD_ACP, 0, ptr, unicode.size(), NULL, 0, NULL, NULL);

    rstring strRet(size, 0);
    int len = WideCharToMultiByte(CP_THREAD_ACP, 0, ptr, unicode.size(), (LPSTR)strRet.c_str(), size, NULL, NULL);

    return strRet;
}

// 效果等同于 string to wstring
const rwstring CharsetUtil::AnsiToUnicode(const rstring& ansi)
{
    LPCCH ptr = ansi.c_str();
    int size = MultiByteToWideChar(CP_ACP, 0, ptr, ansi.size(), NULL, NULL);

    rwstring wstrRet(size, 0);
    int len = MultiByteToWideChar(CP_ACP, 0, ptr, ansi.size(), (LPWSTR)wstrRet.c_str(), size);

    return wstrRet;
}

const rstring CharsetUtil::AnsiToUtf8(const rstring& ansi)
{
    LPCCH ptr = ansi.c_str();
    /* 分配目标空间, 长度为 Ansi 编码的两倍 */
    int size = MultiByteToWideChar(CP_ACP, 0, ptr, ansi.size(), NULL, NULL);

    rwstring wstrTemp(size, 0);
    int len = MultiByteToWideChar(CP_ACP, 0, ptr, ansi.size(), (LPWSTR)wstrTemp.c_str(), size);

    return UnicodeToUtf8(wstrTemp);
}

const rstring CharsetUtil::Utf8ToAnsi(const rstring& utf8)
{
    rwstring wstrTemp = Utf8ToUnicode(utf8);

    LPCWCH ptr = wstrTemp.c_str();
    int size = WideCharToMultiByte(CP_ACP, 0, ptr, utf8.size(), NULL, 0, NULL, NULL);

    rstring strRet(size, 0);
    int len = WideCharToMultiByte(CP_ACP, 0, ptr, utf8.size(), (LPSTR)strRet.c_str(), size, NULL, NULL);

    return strRet;
}

const rstring CharsetUtil::UnicodeToUtf8(const rwstring& unicode)
{
    /* 分配目标空间, 一个16位Unicode字符最多可以转为4个字节 */
    LPCWCH ptr = unicode.c_str();
    int size = WideCharToMultiByte(CP_UTF8, 0, ptr, unicode.size(), NULL, 0, NULL, NULL);

    rstring strRet(size, 0);
    int len = WideCharToMultiByte(CP_UTF8, 0, ptr, unicode.size(), (char*)strRet.c_str(), size, NULL, NULL);

    return strRet;
}

const rwstring CharsetUtil::Utf8ToUnicode(const rstring& utf8)
{
    LPCCH ptr = utf8.c_str();
    int size = MultiByteToWideChar(CP_UTF8, 0, ptr, utf8.size(), NULL, NULL);

    rwstring wstrRet(size, 0);
    int len = MultiByteToWideChar(CP_UTF8, 0, ptr, utf8.size(), (LPWSTR)wstrRet.c_str(), size);

    return wstrRet;
}


const rstring CharsetUtil::GBKToUtf8(const rstring& gbk)
{
    return CharsetUtil::AnsiToUtf8(gbk);
}

const rstring CharsetUtil::Utf8ToGBK(const rstring& utf8)
{
    return CharsetUtil::Utf8ToAnsi(utf8);
}

bool CharsetUtil::IsUTF8(const void* pBuffer, long size)
{
    bool isUTF8 = true;
    unsigned char* start = (unsigned char*)pBuffer;
    unsigned char* end = (unsigned char*)pBuffer + size;
    while (start < end)
    {
        if (*start < 0x80) { /*(10000000): 值小于0x80的为ASCII字符*/
            start++;
        }
        else if (*start < (0xC0)) { /*(11000000): 值介于0x80与0xC0之间的为无效UTF-8字符*/
            isUTF8 = false;
            break;
        }
        else if (*start < (0xE0)) { /*(11100000): 此范围内为2字节UTF-8字符  */
            if (start >= end - 1) {
                break;
            }
            if ((start[1] & (0xC0)) != 0x80) {
                isUTF8 = false;
                break;
            }
            start += 2;
        }
        else if (*start < (0xF0)) { /**(11110000): 此范围内为3字节UTF-8字符*/
            if (start >= end - 2) {
                break;
            }
            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80) {
                isUTF8 = false;
                break;
            }
            start += 3;
        }
        else {
            isUTF8 = false;
            break;
        }
    }

    return isUTF8;
}

rstring CharsetUtil::StripAnsiEndingZero(const rstring& ansi)
{
    char removes[1] = { 0 };
    rstring bytes = ansi;
    GeneralUtil::strStripCharsInEnding(bytes, rstring(removes, 1));
    return bytes;
}



//GB2312 转换成 Unicode
const rwstring CharsetUtil::GB2312ToUnicode(const rstring& gb2312)
{
    UINT nCodePage = 936; //GB2312
    int size = MultiByteToWideChar(nCodePage, 0, gb2312.c_str(), gb2312.size(), NULL, 0);

    rwstring wstrRet(size, 0);
    MultiByteToWideChar(nCodePage, 0, gb2312.c_str(), gb2312.size(), (LPWSTR)wstrRet.c_str(), size);

    return wstrRet;
}

//BIG5 转换成 Unicode
const rwstring CharsetUtil::BIG5ToUnicode(const rstring& big5)
{
    UINT nCodePage = 950; //BIG5
    int size = MultiByteToWideChar(nCodePage, 0, big5.c_str(), big5.size(), NULL, 0);

    rwstring wstrRet(size, 0);
    MultiByteToWideChar(nCodePage, 0, big5.c_str(), big5.size(), (LPWSTR)wstrRet.c_str(), size);

    return wstrRet;
}

//Unicode 转换成 GB2312
const rstring CharsetUtil::UnicodeToGB2312(const rwstring& unicode)
{
    UINT nCodePage = 936; //GB2312
    int size = WideCharToMultiByte(nCodePage, 0, unicode.c_str(), unicode.size(), NULL, 0, NULL, NULL);

    rstring strRet(size, 0);
    WideCharToMultiByte(nCodePage, 0, unicode.c_str(), unicode.size(), (LPSTR)strRet.c_str(), size, NULL, NULL);

    return strRet;
}

//Unicode 转换成 BIG5
const rstring CharsetUtil::UnicodeToBIG5(const rwstring& unicode)
{
    UINT nCodePage = 950; //BIG5
    int size = WideCharToMultiByte(nCodePage, 0, unicode.c_str(), unicode.size(), NULL, 0, NULL, NULL);

    rstring strRet(size, 0);
    WideCharToMultiByte(nCodePage, 0, unicode.c_str(), unicode.size(), (LPSTR)strRet.c_str(), size, NULL, NULL);

    return strRet;
}

//繁体中文BIG5 转换成 简体中文 GB2312
const rstring CharsetUtil::FBIG5ToGB2312(const rstring& big5)
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
    rwstring unicode = CharsetUtil::BIG5ToUnicode(big5);

    rstring gb2312 = CharsetUtil::UnicodeToGB2312(unicode);
    int size = LCMapStringA(lcid, LCMAP_SIMPLIFIED_CHINESE, gb2312.c_str(), big5.size(), NULL, 0);

    rstring strRet(size, 0);
    LCMapStringA(0x0804, LCMAP_SIMPLIFIED_CHINESE, gb2312.c_str(), big5.size(), (LPSTR)strRet.c_str(), size);

    return strRet;
}

//简体中文 GB2312 转换成 繁体中文BIG5
const rstring CharsetUtil::GB2312ToFBIG5(const rstring gb2312)
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
    int size = LCMapStringA(lcid, LCMAP_TRADITIONAL_CHINESE, gb2312.c_str(), gb2312.size(), NULL, 0);

    rstring strRet(size, 0);
    LCMapStringA(lcid, LCMAP_TRADITIONAL_CHINESE, gb2312.c_str(), gb2312.size(), (LPSTR)strRet.c_str(), size);

    rwstring unicode = CharsetUtil::GB2312ToUnicode(strRet);
    rstring big5 = CharsetUtil::UnicodeToBIG5(unicode);

    return big5;
}

void CharsetUtil::showHex(const char* bytes, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x ", (unsigned char)bytes[i]);
    }
}

void CharsetUtil::showHex(const rstring& charset, const rstring& str) {
    printf("%10s: ", charset.data());
    showHex(str.data(), str.size());
    printf("\n");
}

void CharsetUtil::showHex(const rstring& charset, const rwstring& str) {
    printf("%10s: ", charset.data());
    showHex((char*)str.data(), 2 * str.size());
    printf("\n");
}

//将string转换成wstring  
const rwstring CharsetUtil::string2wstring(const rstring& str)
{
    rwstring result;
    //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    wchar_t* buffer = new wchar_t[len + 1];
    //多字节编码转换成宽字节编码  
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';             //添加字符串结尾  
    //删除缓冲区并返回值  
    result.append(buffer);
    delete[] buffer;
    return result;
}

//将wstring转换成string  
const rstring CharsetUtil::wstring2string(const rwstring& wstr)
{
    rstring result;
    //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    //宽字节编码转换成多字节编码  
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    //删除缓冲区并返回值  
    result.append(buffer);
    delete[] buffer;
    return result;
}