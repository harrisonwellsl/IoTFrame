#pragma once

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

#pragma comment(lib, "libactivemq-cpp.lib")
#pragma comment(lib, "libapr-1.lib")

class Producer {
private:
	cms::Connection* connection;
	cms::Session* session;
	cms::Destination* destination;
	cms::MessageProducer* producer;
	bool use_topic_;
	bool client_ack_;
	std::string broker_uri_;
	std::string des_uri_;

	void cleanup();

private:
	Producer(const Producer&);
	Producer& operator= (const Producer&);

public:
	Producer(const std::string broker_uri, const std::string des_uri,
		     bool client_ack = false, bool use_topic = false);
	~Producer();
	void Init();
	void SendMessage(std::string& message_test);
	void Close();
};

