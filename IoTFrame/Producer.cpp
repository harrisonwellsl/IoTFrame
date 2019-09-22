#include "Producer.h"

void Producer::cleanup() {
	// �ر���Դ
	try {
		if (this->session != nullptr) this->session->close();
		if (this->connection != nullptr) this->connection->close();
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}

	// �ͷ�����
	try {
		if (this->connection != nullptr) delete this->connection;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->connection = nullptr;

	// �ͷŻỰ
	try {
		if (this->session != nullptr) delete this->session;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->session = nullptr;

	// �ͷ�Ŀ��
	try {
		if (this->destination != nullptr) delete this->destination;
	}
	catch (cms::CMSException& e) {
		e.printStackTrace();
	}
	this->destination = nullptr;

	// �ͷ�������
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
		// ������������
		std::unique_ptr<activemq::core::ActiveMQConnectionFactory>
			connect_factory(new activemq::core::ActiveMQConnectionFactory(this->broker_uri_));

		// ��������
		try {
			this->connection = connect_factory->createConnection();
			this->connection->start();
		}
		catch (cms::CMSException& e) {
			fprintf(stderr, "Connection build failed.\n");
			e.printStackTrace();
			throw e;
		}

		// ���ÿͻ���Ӧ��ʽ
		if (this->client_ack_) {
			this->session = this->connection->createSession(cms::Session::CLIENT_ACKNOWLEDGE);
		}
		else {
			this->session = this->connection->createSession(cms::Session::AUTO_ACKNOWLEDGE);
		}

		// ʹ��topic����queue
		if (this->use_topic_) {
			this->destination = this->session->createTopic(this->des_uri_);
		}
		else {
			this->destination = this->session->createQueue(this->des_uri_);
		}

		// ����������
		this->producer = this->session->createProducer(this->destination);
		this->producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);
	}
	catch (cms::CMSException& e) {
		fprintf(stderr, "Init failed.\n");
		e.printStackTrace();
	}
}

void Producer::SendMessage(std::string& message_text) {
	// ������Ϣ�����
	cms::TextMessage* message = this->session->createTextMessage(message_text);
	// ������Ϣ
	this->producer->send(message);
	fprintf(stdout, "Send message: %s to the broker.\n", message_text.c_str());
	// ɾ����̬�����ڴ�
	delete message;
}

void Producer::Close() {
	this->cleanup();
}
