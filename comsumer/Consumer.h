#pragma once

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

#include "MySQLModule.h"

class Consumer : public cms::ExceptionListener,
	             public cms::MessageListener,
	             public activemq::transport::DefaultTransportListener {
private:
	cms::Connection* connection;
	cms::Session* session;
	cms::Destination* destination;
	cms::MessageConsumer* consumer;
	bool use_topic_;
	std::string broker_uri_;
	std::string des_uri_;
	bool client_ack_;
	MySQLModule* mysql_module;

private:
	Consumer(const Consumer&);
	Consumer& operator=(const Consumer&);
	void cleanup();

public:
	Consumer(const std::string& broker_uri, const std::string& des_uri,
		     bool use_topic = false, bool client_ack = false);
	void Init(const char* ip_addr, const char* user_name,
		const char* password, const char* db_name, const int port, const char* encode);
	void close();
	~Consumer();
	virtual void onMessage(const cms::Message* message);
	virtual void onException(const cms::CMSException& ex AMQCPP_UNUSED);
	virtual void onException(const decaf::lang::Exception& ex);
	virtual void transportInterrupted();
	virtual void transportResumed();
};

