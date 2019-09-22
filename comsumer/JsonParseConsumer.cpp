#include "JsonParseConsumer.h"
#include <sstream>

// 展示获取到的json数据
void JsonParseConsumer::EnumContent(std::string json_str) {
	rapidjson::Document document;
	document.SetObject();
	document.Parse(json_str.c_str());
	for (auto itr = document.MemberBegin(); itr != document.MemberEnd(); itr++) {
		std::cout << itr->name.GetString() << "\t------>\t";
		if (document[itr->name.GetString()].IsInt()) {
			std::cout << document[itr->name.GetString()].GetInt() << std::endl;
		}
		if (document[itr->name.GetString()].IsString()) {
			std::cout << document[itr->name.GetString()].GetString() << std::endl;
		}
	}
}

std::string JsonParseConsumer::InsertMySQL(std::string json_str) {
	rapidjson::Document document;
	document.SetObject();
	document.Parse(json_str.c_str());

	// 利用获取到的json字符串生成需要插入的数据的SQL语句
	std::string head_base_str("INSERT INTO `test`.`");
	std::string value_base_str("VALUES (default");
	head_base_str.append(document["device name"].GetString());
	head_base_str.append("` (`ID`");
	for (auto itr = document.MemberBegin(); itr != document.MemberEnd(); itr++) {
		//if (strcmp(itr->name.GetString(), "device name") == 0) continue;
		head_base_str.append(", `");
		head_base_str.append(itr->name.GetString());
		head_base_str.append("`");
		value_base_str.append(",");
		if (document[itr->name.GetString()].IsString()) {
			value_base_str.append("'");
			value_base_str.append(document[itr->name.GetString()].GetString());
			value_base_str.append("'");
		}
		if (document[itr->name.GetString()].IsInt()) {
			std::stringstream value_in;
			value_in << document[itr->name.GetString()].GetInt();
			value_base_str.append(value_in.str());
		}
	}
	head_base_str.append(") ");
	value_base_str.append(");");
	return head_base_str + value_base_str;
}

std::string JsonParseConsumer::CreateTableForMySQL(std::string json_str) {
	rapidjson::Document document;
	document.SetObject();
	document.Parse(json_str.c_str());

	// 生成SQL语句用于创建数据库中的表
	std::string sql_str = "CREATE TABLE IF NOT EXISTS `";
	for (auto itr = document.MemberBegin(); itr != document.MemberEnd(); itr++) {
		std::cout << itr->name.GetString() << "\t------>\t";
		if (strcmp(itr->name.GetString(), "device name") == 0) {
			sql_str.append(document[itr->name.GetString()].GetString());
			sql_str.append("` (");
			sql_str.append(" `ID` int NOT NULL AUTO_INCREMENT,");
			sql_str.append(" `device name` varchar(40) NOT NULL,");
			continue;
		}
		if (document[itr->name.GetString()].IsInt()) {
			sql_str.append(" `");
			sql_str.append(itr->name.GetString());
			sql_str.append("` int NOT NULL, ");
			std::cout << document[itr->name.GetString()].GetInt() << std::endl;
		}
		if (document[itr->name.GetString()].IsString()) {
			sql_str.append(" `");
			sql_str.append(itr->name.GetString());
			sql_str.append("` varchar(40) NOT NULL, ");
			std::cout << document[itr->name.GetString()].GetString() << std::endl;
		}
	}
	sql_str.append(" PRIMARY KEY(`ID`)");
	sql_str.append(" ) ENGINE = InnoDB DEFAULT CHARSET = utf8");

	//std::cout << "The SQL is: " << sql_str << std::endl;
	
	return sql_str;
}
