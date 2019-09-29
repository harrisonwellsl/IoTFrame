#include <thread>
#include "Consumer.h"
#include "JsonParseConsumer.h"
#include "MySQLModule.h"

void Consumer::cleanup() {
	// 删除
	try {
		if (this->destination != nullptr) delete this->destination;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->destination = nullptr;

	// 删除
	try {
		if (this->consumer != nullptr) delete this->consumer;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->consumer = nullptr;

	// 关闭资源
	try {
		if (this->session != nullptr) this->session->close();
		if (this->connection != nullptr) this->connection->close();
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}

	// 删除资源
	try {
		if (this->session != nullptr) delete this->session;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->session = nullptr;

	try {
		if (this->connection != nullptr) delete this->connection;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->connection = nullptr;
}

Consumer::Consumer(const std::string& broker_uri, const std::string& des_uri,
	               bool use_topic, bool client_ack) :
                   broker_uri_(broker_uri),
                   des_uri_(des_uri),
                   use_topic_(use_topic),
                   client_ack_(client_ack),
				   connection(nullptr),
				   session(nullptr),
				   destination(nullptr),
				   consumer(nullptr),
	               mysql_module(nullptr) {
}

void Consumer::Init(const char* ip_addr, const char* user_name,
	const char* password, const char* db_name, const int port, const char* encode) {
	try {
		// 创建链接工厂
		activemq::core::ActiveMQConnectionFactory* connection_factory = new activemq::core::ActiveMQConnectionFactory(this->broker_uri_);

		// 创建链接
		this->connection = connection_factory->createConnection();
		delete connection_factory;

		activemq::core::ActiveMQConnection* amq_connection = dynamic_cast<activemq::core::ActiveMQConnection*>(this->connection);
		if (amq_connection != nullptr) {
			amq_connection->addTransportListener(this);
		}

		// 开启链接
		this->connection->start();

		// 设置错误监听
		this->connection->setExceptionListener(this);

		// 设置客户端应答方式
		if (this->client_ack_) {
			this->session = this->connection->createSession(cms::Session::CLIENT_ACKNOWLEDGE);
		}
		else {
			this->session = this->connection->createSession(cms::Session::AUTO_ACKNOWLEDGE);
		}

		// 创建目标
		if (this->use_topic_) {
			this->destination = this->session->createTopic(this->des_uri_);
		}
		else {
			this->destination = this->session->createQueue(this->des_uri_);
		}

		// 创建消费者
		this->consumer = this->session->createConsumer(this->destination);
		this->consumer->setMessageListener(this);
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}

	this->mysql_module = new MySQLModule;
	this->mysql_module->Init(encode);
	this->mysql_module->Connect(ip_addr, user_name, password, db_name, port);

}

void Consumer::close() {
	this->cleanup();
}

Consumer::~Consumer() {
	this->cleanup();
}

// 实现onMessage方法，自动返回收到的消息
void Consumer::onMessage(const cms::Message* message) {

	static int count = 0;

	try {
		count++;
		const cms::TextMessage* textMessage = dynamic_cast<const cms::TextMessage*>(message);
		std::string text = "";

		if (textMessage != nullptr) {
			text = textMessage->getText();
		}
		else {
			text = "NOT A TEXTMESSAGE!";
		}

		if (this->client_ack_) {
			message->acknowledge();
		}

		std::cout << text << std::endl;
		// 静态成员函数枚举收到的消息内容
		JsonParseConsumer::EnumContent(text);

		// 获取生成的字符串
		this->mysql_module->ExcuteQuery(JsonParseConsumer::CreateTableForMySQL(text));
		this->mysql_module->ExcuteQuery(JsonParseConsumer::InsertMySQL(text));

		printf("Message #%d Received: %s\n", count, text.c_str());
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
}

void Consumer::onException(const cms::CMSException& ex AMQCPP_UNUSED) {
	printf("CMS Exception occurred.  Shutting down client.\n");
	exit(EXIT_FAILURE);
}

void Consumer::onException(const decaf::lang::Exception& ex) {
	printf("Transport Exception occurred: %s \n", ex.getMessage().c_str());
}

void Consumer::transportInterrupted() {
	std::cout << "The Connection's Transport has been Interrupted." << std::endl;
}

void Consumer::transportResumed() {
	std::cout << "The Connection's Transport has been Restored." << std::endl;
}
