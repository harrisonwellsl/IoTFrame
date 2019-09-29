#include <iostream>

#include "Consumer.h"

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libactivemq-cpp.lib")
#pragma comment(lib, "libapr-1.lib")
#pragma comment(lib, "libmysql.lib")

int main(int argc, char** argv) {
	// ��ʼ��ActiveMQ
	activemq::library::ActiveMQCPP::initializeLibrary();
	puts("Enter a uri like \"192.168.56.101:61616\" : ");
	char broker_uri_buffer[256] = { '\0' };
	fgets(broker_uri_buffer, 256, stdin);
	// ȥ����β���з�
	broker_uri_buffer[strlen(broker_uri_buffer) - 1] = '\0';
	// �м��URIĬ�Ͽ���������������
	std::string brokerURI = "failover:(tcp://";
	brokerURI += broker_uri_buffer;
	brokerURI += ")";
	std::cout << "Broker URI is: " << brokerURI << std::endl;
	// ����Ŀ��URI
	puts("Enter a destination uri: ");
	char dest_uri_buffer[256] = { '\0' };
	fgets(dest_uri_buffer, 256, stdin);
	std::string destURI;
	// ʹ��Ĭ��Ŀ��URI
	if (dest_uri_buffer[0] == '\n') {
		puts("Destination URI is default.");
		destURI = "TEST.FOO";
	}
	// ʹ���Զ����Ŀ��URI
	else {
		dest_uri_buffer[strlen(dest_uri_buffer) - 1] = '\0';
		destURI = dest_uri_buffer;
	}
	std::cout << "Destination URI is: " << destURI << std::endl;
	// �Ƿ���Ҫʹ��topic
	puts("Need to use the Topics (Enter 'y' or 'n') ? (default is false.): ");
	bool useTopics;
	char in_buffer[2] = { '\0' };
	fgets(in_buffer, 2, stdin);
	if (in_buffer[0] == 'y' || in_buffer[0] == 'Y') {
		puts("Use the Topics.");
		useTopics = true;
	}
	else if (in_buffer[0] == 'n' || in_buffer[0] == 'N') {
		puts("Don't use the Topics.");
		useTopics = false;
	}
	else if (in_buffer[0] == '\n') {
		puts("Use the default.");
		useTopics = false;
	}
	else {
		fprintf(stderr, "Enter an invalid argument!\n");
		return EXIT_FAILURE;
	}

	// �Ƿ���Ҫ�ͻ���Ӧ��
	puts("Need to use the Client ack (Enter 'y' or 'n') ? (default is false.): ");
	fgets(in_buffer, 2, stdin);
	bool clientAck;
	if (in_buffer[0] == 'y' || in_buffer[0] == 'Y') {
		puts("Use the client ack.");
		clientAck = true;
	}
	else if (in_buffer[0] == 'n' || in_buffer[0] == 'N') {
		puts("Don't use the client ack.");
		clientAck = false;
	}
	else if (in_buffer[0] == '\n') {
		puts("Use the default.");
		clientAck = false;
	}
	else {
		fprintf(stderr, "Enter an invalid argument!\n");
		return EXIT_FAILURE;
	}
	// ʵ���������߶���
	Consumer consumer(brokerURI, destURI, useTopics, clientAck);
	// ���������IP��ַ
	puts("Please enter the server ip address (default is localhost): ");
	char ip_addr_buffer[128] = { '\0' };
	fgets(ip_addr_buffer, 128, stdin);
	if (ip_addr_buffer[0] == '\n' && strlen(ip_addr_buffer) < 7) {
		fprintf(stdout, "Use the default.\n");
		sprintf_s(ip_addr_buffer, "localhost");
	}
	else {
		ip_addr_buffer[strlen(ip_addr_buffer) - 1] = '\0';
	}
	fprintf(stdout, "Server's ip address is %s\n", ip_addr_buffer);
	// �����û���
	puts("Please enter the MySQL user (default is root): ");
	char user_buffer[128] = { '\0' };
	fgets(user_buffer, 128, stdin);
	if (user_buffer[0] == '\n') {
		fprintf(stdout, "Use the default.\n");
		sprintf_s(user_buffer, "root");
	}
	else {
		user_buffer[strlen(user_buffer) - 1] = '\0';
	}
	fprintf(stdout, "MySQL user is %s\n", user_buffer);
	// ��������
	puts("Please enter the MySQL password (default is 123456): ");
	char password_buffer[128] = { '\0' };
	fgets(password_buffer, 128, stdin);
	if (password_buffer[0] == '\n') {
		fprintf(stdout, "Use the default.\n");
		sprintf_s(password_buffer, "123456");
	}
	else {
		password_buffer[strlen(password_buffer) - 1] = '\0';
	}
	// �������ݿ�����
	puts("Please enter the MySQL database name (default is test): ");
	char db_name_buffer[128] = { '\0' };
	fgets(db_name_buffer, 128, stdin);
	if (db_name_buffer[0] == '\n') {
		fprintf(stdout, "Use the default.\n");
		sprintf_s(db_name_buffer, "test");
	}
	else {
		db_name_buffer[strlen(db_name_buffer) - 1] = '\0';
	}
	fprintf(stdout, "MySQL database name is %s\n", db_name_buffer);
	// �������ݿ�˿ں�
	puts("Please enter the MySQL database port (default is 3306): ");
	char db_port_buffer[128] = { '\0' };
	fgets(db_port_buffer, 128, stdin);
	if (db_port_buffer[0] == '\n') {
		fprintf(stdout, "Use the default.\n");
		sprintf_s(db_port_buffer, "3306");
	}
	else {
		db_port_buffer[strlen(db_port_buffer) - 1] = '\0';
	}
	fprintf(stdout, "MySQL database port is %s\n", db_port_buffer);
	// ��������ʽ
	puts("Please enter the MySQL database encode (default is utf-8): ");
	char encode_buffer[128] = { '\0' };
	fgets(encode_buffer, 128, stdin);
	if (encode_buffer[0] == '\n') {
		fprintf(stdout, "Use the default.\n");
		sprintf_s(encode_buffer, "utf-8");
	}
	else {
		encode_buffer[strlen(encode_buffer) - 1] = '\0';
	}
	fprintf(stdout, "MySQL database name is %s\n", encode_buffer);
	system("pause");
	// ��ʼ��������
	consumer.Init(ip_addr_buffer, user_buffer,
		password_buffer, db_name_buffer,
		(int)atof(db_port_buffer), encode_buffer);

	// ����q���˳�����
	std::cout << "Press 'q' to quit" << std::endl;
	while (std::cin.get() != 'q') {
		continue;
	}
	consumer.close();
	activemq::library::ActiveMQCPP::shutdownLibrary();
	return 0;
}