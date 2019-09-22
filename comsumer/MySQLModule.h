#pragma once

#include <mysql.h>

class MySQLModule {
private:
	MYSQL* mysql_data;

public:
	MySQLModule();
	~MySQLModule();
	void cleanup();
	// ����0��ʼ���ɹ�������ʧ��
	int Init(const char* encode);
	// ����0���ӳɹ�������ʧ��
	int Connect(const char* ip_addr, const char* user_name,
		const char* password, const char* db_name, const int port);
	int ExcuteQuery(std::string query);
};

