#include "Producer.h"

void Producer::cleanup() {
	// 关闭资源
	try {
		if (this->session != nullptr) this->session->close();
		if (this->connection != nullptr) this->connection->close();
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}

	// 释放连接
	try {
		if (this->connection != nullptr) delete this->connection;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->connection = nullptr;

	// 释放会话
	try {
		if (this->session != nullptr) delete this->session;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->session = nullptr;

	// 释放目标
	try {
		if (this->destination != nullptr) delete this->destination;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->destination = nullptr;

	// 释放生产者
	try {
		if (this->producer != nullptr) delete this->producer;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->producer = nullptr;
}

Producer::Producer(const std::string broker_uri, const std::string des_uri,
	               bool client_ack, bool use_topic) 
	: connection(nullptr),
      session(nullptr),
      destination(nullptr),
      producer(nullptr),
      use_topic_(use_topic),
      client_ack_(client_ack),
      broker_uri_(broker_uri),
      des_uri_(des_uri) {
}

Producer::~Producer() {
	this->cleanup();
}

void Producer::Init() {
	try {
		// 创建工厂函数
		std::unique_ptr<activemq::core::ActiveMQConnectionFactory>
			connect_factory(new activemq::core::ActiveMQConnectionFactory(this->broker_uri_));

		// 创建连接
		try {
			this->connection = connect_factory->createConnection();
			this->connection->start();
		}
		catch (cms::CMSException& e) {
			fprintf(stderr, "Connection build failed.\n");
			e.printStackTrace();
			throw e;
		}

		// 设置客户端应答方式
		if (this->client_ack_) {
			this->session = this->connection->createSession(cms::Session::CLIENT_ACKNOWLEDGE);
		}
		else {
			this->session = this->connection->createSession(cms::Session::AUTO_ACKNOWLEDGE);
		}

		// 使用topic还是queue
		if (this->use_topic_) {
			this->destination = this->session->createTopic(this->des_uri_);
		}
		else {
			this->destination = this->session->createQueue(this->des_uri_);
		}

		// 创建生产者
		this->producer = this->session->createProducer(this->destination);
		this->producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
	}
	catch (cms::CMSException& e) {
		fprintf(stderr, "Init failed.\n");
		e.printStackTrace();
	}
}

void Producer::SendMessage(std::string& message_text) {
	// 创建消息类对象
	cms::TextMessage* message = this->session->createTextMessage(message_text);
	// 发送消息
	this->producer->send(message);
	fprintf(stdout, "Send message: %s to the broker.\n", message_text.c_str());
	// 删除动态分配内存
	delete message;
}

void Producer::Close() {
	this->cleanup();
}
