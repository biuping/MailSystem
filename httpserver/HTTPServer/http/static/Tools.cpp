#include "Tools.h"

void Tools::report(rstring info)
{
	std::cout << info << std::endl;
}

void Tools::report(rstring info, int error_code)
{
	std::cout << info + std::to_string(error_code) << std::endl;
}

void Tools::to_lower(rstring& s)
{
	int len = s.size();
	for (int i = 0; i < len; i++)
	{
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += 32;
	}
}

void Tools::to_upper(rstring& s)
{
	int len = s.size();
	for (int i = 0; i < len; i++)
	{
		if (s[i] >= 'a' && s[i] <= 'z')
			s[i] -= 32;
	}
}

rstring Tools::getUUID()
{
	char buf[GUID_LEN] = { 0 };
	GUID guid;

	if (CoCreateGuid(&guid))
	{
		return std::move(std::string(""));
	}

	sprintf(buf,
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);

	return std::move(std::string(buf));
	return "";
}

void Tools::json_read(const char* start, size_t len, Json::Value& json_object, Json::String& errs)
{
	Json::CharReaderBuilder read_builder;
	std::unique_ptr<Json::CharReader> json_reader(read_builder.newCharReader());
	
	json_reader->parse(start, start + len, &json_object, &errs);

}

void Tools::json_write(const Json::Value& root,rstring& res, bool beautify)
{
	Json::StreamWriterBuilder writer_builder;
	if (!beautify)
		writer_builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> json_writer(writer_builder.newStreamWriter());
	std::ostringstream os;
	json_writer->write(root, &os);
	res = os.str();

}

size_t Tools::cal_len(const char* start, const char* end)
{
	return (size_t)(end - start);
}

const char* Tools::find_line(const char* start, const char* end)
{
	//查找长度为2，可以-1判断
	for (const char* lstart = start; lstart < (end - 1); lstart++) {
		if ('\r' == lstart[0] && '\n' == lstart[1])
		{
			return &lstart[2];
		}
	}
	return nullptr;
}

const char* Tools::find_head(const char* start, const char* end)
{
	for (const char* hstart = start; hstart < (end - 3); hstart++) {
		if (hstart[0] == '\r' && hstart[1] == '\n' && \
			hstart[2] == '\r' && hstart[3] == '\n')
		{
			return &hstart[4];
		}
	}
	return nullptr;
}

const char* Tools::find_content(const char* start, const char* end, char c_end, size_t& content_len, size_t& sum_len)
{
	size_t _content_len = 0;
	const char* content_start = nullptr;
	for (const char* _start = start; _start < end; _start++)
	{
		if (content_start == nullptr)
		{
			//找到非空格的开始
			if (*_start != ' ')
			{
				content_start = _start;
				_content_len = 1;
			}
		}
		else
		{
			//遍历到结尾
			if (*_start == c_end)
			{
				content_len = _content_len;
				sum_len = cal_len(start, _start);
				return content_start;
			}
			_content_len++;
		}
	}
	return nullptr;
}
