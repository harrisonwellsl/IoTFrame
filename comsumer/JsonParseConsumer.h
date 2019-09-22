#pragma once

#include <document.h>
#include <iostream>

class JsonParseConsumer {
public:
	JsonParseConsumer() = default;
	~JsonParseConsumer() = default;
	static void EnumContent(std::string json_str);
	static std::string InsertMySQL(std::string json_str);
	static std::string CreateTableForMySQL(std::string json_str);
};

