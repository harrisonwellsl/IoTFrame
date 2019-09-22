#pragma once

#include <string>
#include <document.h>

class JsonParse {
public:
	JsonParse(const char* json_str);
	~JsonParse();
	int GetIntMember(const char* member_name) const;
	std::string GetStrMember(const char* member_name) const;

private:
	rapidjson::Document document;

	void CheckArguments(const char* member_name) const;
};

