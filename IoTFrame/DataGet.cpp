#include <iostream>
#include <WinSock2.h>

#include "definations.h"
#include "DataGet.h"

#pragma comment(lib, "ws2_32.lib")

DataGet::DataGet(SOCKET* client_socket, int id, sockaddr_in client_info) {
	this->client_socket_ = client_socket;
	this->id_ = id;
	this->client_info_ = client_info;
}

DataGet::~DataGet() {
	if (this->client_socket_ != nullptr) {
		this->Shutdown();
	}
}

void DataGet::Shutdown() {
	closesocket(*(this->client_socket_));
	delete this->client_socket_;
	this->client_socket_ = nullptr;
}

int DataGet::GetId() const {
	return this->id_;
}

std::string DataGet::GetClientAddrInfo() const {
	char ip_addr_buffer[256] = { '\0' };
	sprintf_s(ip_addr_buffer, "%d.%d.%d.%d",
		this->client_info_.sin_addr.S_un.S_un_b.s_b1,
		this->client_info_.sin_addr.S_un.S_un_b.s_b2,
		this->client_info_.sin_addr.S_un.S_un_b.s_b3,
		this->client_info_.sin_addr.S_un.S_un_b.s_b4);
	return std::string(ip_addr_buffer);
}

unsigned short DataGet::GetClientPortInfo() const {
	return this->client_info_.sin_port;
}

std::string DataGet::GetMsgFromClient() {
	char buffer[MAX_BUFFER_LEN] = { '\0' };
	int recv_len = recv(*(this->client_socket_), buffer, MAX_BUFFER_LEN - 1, 0);
	// 如果读取到的长度为0，则为客户端下线
	if (recv_len == 0) {
		// 返回空字符串
		return (std::string)"";
	}
	return std::string(buffer);
}
