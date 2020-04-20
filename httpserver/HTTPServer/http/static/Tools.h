#pragma once
#include <iostream>
#include <string>
#include "config.h"
#include "http_const.h"
#include <map>
#include "json/json.h"
#include <stdio.h>  
#include <objbase.h>

#define GUID_LEN 64

typedef std::string rstring;
typedef std::map<rstring, rstring> HttpHead_t;
typedef std::pair<rstring, rstring> HttpHeadPair_t;

//附件结构
typedef struct Attachment{ 
	rstring content_type;
	rstring file_name;
	rstring content;
	Attachment(rstring contentType, rstring fileName, rstring contents) 
	{
		content_type = contentType;
		file_name = fileName;
		content = contents;
	};
	Attachment() {};
};
//用户信息结构
typedef struct UserInfo {
	rstring email_address;
	rstring pass;
	UserInfo() {};
	UserInfo(rstring email, rstring pass)
	{
		this->email_address = email;
		this->pass = pass;
	}
};

typedef std::map<rstring, UserInfo*> UserStore;

class Tools
{
public:
	//封装cout
	static void report(rstring info);
	//封装cout+错误码
	static void report(rstring info, int error_code);

	//转小写
	static void to_lower(rstring& s);
	//转大写
	static void to_upper(rstring& s);
	//获得uuid
	static rstring getUUID();

public:
	/*
	解析json对象
	start: 需要解析的字符串起始位置
	len: 需要解析的字符串的长度
	json_object: 将解析后的json对象存放在其中
	errs：出错的信息
	*/
	static void json_read(const char* start,size_t len,Json::Value& json_object, Json::String& errs);
	/*
	从json对象生成json字符串
	root：json对象的根节点
	res：生成后的字符串
	beautify：是否美化输出，默认为否
	*/
	static void json_write(const Json::Value& root,rstring& res,bool beautify=false);
public:
	/*
	计算开始指针与结束指针之间的长度
	*/
	static size_t cal_len(const char* start, const char* end);

	/*
	找到字符串中的一行（以\r\n结尾）
	start：开始位置指针
	end：结束位置指针
	return：找到这样一行，返回\n后一位字符指针
	找不到这样一行，返回nullptr
	*/
	static const char* find_line(const char* start, const char* end);

	/*
	找到请求中header位置（以\r\n\r\n结尾, 后面开始为body部分）
	start：开始位置指针
	end：结束位置指针
	return：找到这样一行，返回\n后一位字符指针
		找不到这样一行，返回nullptr
	*/
	static const char* find_head(const char* start, const char* end);

	/*
	第一个以end_c字符结束的从start到end的内容
	start：开始位置指针
	end：结束位置指针
	c_end: 结束字符
	content_len: 内容长度
	sum_len: 查找过程中剪裁掉空格，内容加上空格的长度
	return：找到这样一行，返回\n后一位字符指针
		找不到这样一行，返回nullptr
	*/
	static const char* find_content(const char* start, const char* end,
		char c_end, size_t& content_len, size_t& sum_len);

	
};