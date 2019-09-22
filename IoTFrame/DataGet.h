#pragma once

#include <WinSock2.h>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

class DataGet {
public:
	DataGet(SOCKET* client_socket, int id, sockaddr_in client_info);
	~DataGet();
	void Shutdown();
	int GetId() const;
	std::string GetClientAddrInfo() const;
	unsigned short GetClientPortInfo() const;
	std::string GetMsgFromClient();

private:
	SOCKET* client_socket_;
	sockaddr_in client_info_;
	int id_;
};

