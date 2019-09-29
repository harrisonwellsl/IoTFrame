#pragma once

#include <mysql.h>

// 该类为与MySQL数据库进行交互使用的数据库
// 包括初始化
// 连接
// 执行MySQL语句
class MySQLModule {
private:
	MYSQL* mysql_data;

public:
	MySQLModule();
	~MySQLModule();
	void cleanup();
	// 返回0初始化成功，其余失败
	int Init(const char* encode);
	// 返回0连接成功，其余失败
	int Connect(const char* ip_addr, const char* user_name,
		const char* password, const char* db_name, const int port);
	int ExcuteQuery(std::string query);
};

