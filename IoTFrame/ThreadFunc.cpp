#include <iostream>
#include <document.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

#include "ThreadFunc.h"
#include "DataGet.h"
#include "JsonParse.h"
#include "Producer.h"

void ThreadFunc(SOCKET* client_socket, int id, sockaddr_in client_info) {
	DataGet data_get(client_socket, id, client_info);
	std::cout << "This is the id: " << data_get.GetId() << std::endl;
	std::cout << "The remote client address is: " << data_get.GetClientAddrInfo() << std::endl;
	std::cout << "The remote client port is: " << data_get.GetClientPortInfo() << std::endl;
	std::string msg_from_client;

	std::string broker_uri = "failover://(tcp://192.168.56.101:61616)";
	std::string des_uri = "TEST.FOO";
	//des_uri += data_get.GetClientPortInfo();

	activemq::library::ActiveMQCPP::initializeLibrary();
	Producer producer(broker_uri, des_uri);
	producer.Init();
	
	while (true) {
		// ��ȡ�ͻ��˷��͵��ַ�������
		msg_from_client = data_get.GetMsgFromClient();
		std::cout << msg_from_client << std::endl;

		if (msg_from_client.empty()) {
			// �������ַ����Ļ����ǿͻ�������
			fprintf(stdout, "The client id: %d. host: %s:%d closed.",
				data_get.GetId(),
				data_get.GetClientAddrInfo().c_str(),
				data_get.GetClientPortInfo());
			// �ر�����
			data_get.Shutdown();
			// ������
			// ����ѭ��
			break;
		}
		producer.SendMessage(msg_from_client);
		// ����ͻ��˷��͵�json����
		JsonParse json_parse(msg_from_client.c_str());
	}
	producer.Close();
}
