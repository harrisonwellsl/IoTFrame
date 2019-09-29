#include <iostream>
#include "MySQLModule.h"


MySQLModule::MySQLModule() {
	this->mysql_data = new MYSQL;
}

MySQLModule::~MySQLModule() {
	if (this->mysql_data != nullptr) {
		this->cleanup();
	}
}

// 清理动态分配的内存
void MySQLModule::cleanup() {
	mysql_close(this->mysql_data);
	if (this->mysql_data != nullptr) {
		//delete this->mysql_data;
		this->mysql_data = nullptr;
	}
}


 // 初始化数据库需要的连接
 // 参数为字符串编码格式
 // 类UNIX使用utf8，Windows使用gbk
int MySQLModule::Init(const char* encode) {
	// 初始化数据库
	if (0 == mysql_library_init(0, NULL, NULL)) {
		fprintf(stdout, "mysql_library_init() succeed.\n");
	}
	else {
		fprintf(stderr, "mysql_library_init() failed.\n");
		return EXIT_FAILURE;
	}

	// 初始化数据结构
	if (NULL != mysql_init(this->mysql_data)) {
		fprintf(stdout, "mysql_init() succeed.\n");
	}
	else {
		fprintf(stderr, "mysql_init() failed.\n");
		return EXIT_FAILURE;
	}

	// 在连接数据库之前，设置额外的连接选项
	// 可以设置的选项很多，这里设置字符集，否则无法处理中文
	// 失败返回1
	// 成功返回0
	if (0 == mysql_options(this->mysql_data, MYSQL_SET_CHARSET_NAME, encode)) {
		fprintf(stdout, "mysql_options() succeed.\n");
	}
	else {
		fprintf(stderr, "mysql_options() failed.\n");
		return EXIT_FAILURE;
	}
	
	return 0;
}


// 连接数据库
// 成功返回0
// 失败返回1
// 参数依次为
// IP地址，用户名，密码，数据库名，端口号
int MySQLModule::Connect(const char* ip_addr, const char* user_name,
	const char* password, const char* db_name, const int port) {

	// 连接数据库
	if (NULL != mysql_real_connect(this->mysql_data, ip_addr, user_name,
			password, db_name, port, NULL, 0)) {
		fprintf(stdout, "mysql_real_connect() succeed.\n");
	}
	else {
		fprintf(stderr, "mysql_real_connect() failed.\n");
		return EXIT_FAILURE;
	}

	return 0;
}


// 执行生成的MySQL语句
// 成功返回0
// 失败返回1
int MySQLModule::ExcuteQuery(std::string query) {
	if (0 == mysql_query(this->mysql_data, query.c_str())) {
		fprintf(stdout, "mysql_query() excute query succeed.\n");
	}
	else {
		fprintf(stderr, "mysql_query() excute query failed.\n");
		return EXIT_FAILURE;
	}
	return 0;
}
