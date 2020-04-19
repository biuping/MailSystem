#pragma once
#include "MultipartReader.h"
#include "Tools.h"

typedef std::vector<HttpHead_t*> Header_list;

static HttpHead_t* m_header;
static rstring m_content;
static Header_list lists;

class MultipartRecord
{
public:

	void ClearList()
	{
		for (auto h : lists)
		{
			if (h != nullptr)
				delete h;
		}
		lists.clear();
	}
	//通过名称找到header
	HttpHead_t* FindHeaderByName(const rstring& name)
	{
		for (auto h : lists)
		{
			rstring res = FindHeadContent(*h, HTTP_FORM_NAME);
			if (res.compare(name) == 0)
				return h;
		}
		return nullptr;
	}
	//找到header中特定元素
	rstring FindHeadContent(HttpHead_t& header, const rstring& key)
	{
		HttpHead_t::iterator itor = header.find(key);
		if (itor != header.end())
			return itor->second;

		rstring dis = FindHeadContent(header, HTTP_FORM_DISPOSITION);
		rstring prefix = key + "=";
		rstring res = "";
		if (dis.find(prefix) != dis.npos)
		{
			//+1跳过初始的\"
			size_t start = dis.find(prefix) + prefix.size()+1;
			for (size_t i = start; i < dis.size(); i++)
			{
				if (dis[i] == '\"')
					break;
				res+=dis[i];
			}
		}
		return res;//引用不能为空
	}

public:

	MultipartRecord(MultipartReader& reader)
	{
		m_header = nullptr;
		m_content = "";
		reader.onPartBegin = onPartBegin;
		reader.onPartData = onPartData;
		reader.onPartEnd = onPartEnd;
		reader.onEnd = onEnd;
	}
	~MultipartRecord()
	{
		ClearList();
	}

	static void onPartBegin(const MultipartHeaders& headers, void* userData) {
		//printf("onPartBegin:\n");
		if (m_header == nullptr)
			m_header = new HttpHead_t;
		MultipartHeaders::const_iterator it;
		for (it = headers.begin(); it != headers.end(); it++) {
			//printf("  %s = %s\n", it->first.c_str(), it->second.c_str());
			(*m_header)[it->first] = it->second;
		}
	}

	static void onPartData(const char* buffer, size_t size, void* userData) {
		m_content.append(buffer, size);
		//printf("onPartData: (%s)\n", rstring(buffer, size).c_str());
	}

	static void onPartEnd(void* userData) {
		(*m_header)["Content"] = m_content;

		lists.push_back(m_header);
		m_header = nullptr;
		m_content = "";
		//printf("onPartEnd\n");
	}

	static void onEnd(void* userData) {
		printf("onEnd\n");
	}
};