#pragma once
#include <iostream>
#include <string>
#include "config.h"
#include "http_const.h"
#include <map>
#include "json/json.h"

typedef std::string rstring;
typedef std::map<rstring, rstring> HttpHead_t;
typedef std::pair<rstring, rstring> HttpHeadPair_t;


class Tools
{
public:
	//��װcout
	static void report(rstring info);
	//��װcout+������
	static void report(rstring info, int error_code);

	//תСд
	static void to_lower(rstring& s);
	//ת��д
	static void to_upper(rstring& s);

public:
	/*
	���㿪ʼָ�������ָ��֮��ĳ���
	*/
	static size_t cal_len(const char* start, const char* end);

	/*
	�ҵ��ַ����е�һ�У���\r\n��β��
	start����ʼλ��ָ��
	end������λ��ָ��
	return���ҵ�����һ�У�����\n��һλ�ַ�ָ��
	�Ҳ�������һ�У�����nullptr
	*/
	static const char* find_line(const char* start, const char* end);

	/*
	�ҵ�������headerλ�ã���\r\n\r\n��β, ���濪ʼΪbody���֣�
	start����ʼλ��ָ��
	end������λ��ָ��
	return���ҵ�����һ�У�����\n��һλ�ַ�ָ��
		�Ҳ�������һ�У�����nullptr
	*/
	static const char* find_head(const char* start, const char* end);

	/*
	��һ����end_c�ַ������Ĵ�start��end������
	start����ʼλ��ָ��
	end������λ��ָ��
	c_end: �����ַ�
	content_len: ���ݳ���
	sum_len: ���ҹ����м��õ��ո����ݼ��Ͽո�ĳ���
	return���ҵ�����һ�У�����\n��һλ�ַ�ָ��
		�Ҳ�������һ�У�����nullptr
	*/
	static const char* find_content(const char* start, const char* end,
		char c_end, size_t& content_len, size_t& sum_len);
};