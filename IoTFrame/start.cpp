#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>

#include "DataGet.h"
#include "definations.h"
#include "ThreadFunc.h"

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char** argv) {
	// 配置网络库
	//std::cout << "Init the network library...." << std::endl;
	WORD network_version = MAKEWORD(2, 2);
	WSADATA wsa_data{};

	// 开启网络库
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Openning the WSA...." << std::endl;
	int wsa_error_code = WSAStartup(network_version, &wsa_data);

	if (0 != wsa_error_code) {
		// 开启网络库失败
		// 获取到失败码
		int failed_code = WSAGetLastError();
		fprintf(stderr, "WSA open failed. Error code: %d\n", failed_code);
		// 返回值1
		return EXIT_FAILURE;
	}

	// 检查开启版本是否正确
	if (2 != HIBYTE(wsa_data.wVersion) || 2 != LOBYTE(wsa_data.wVersion)) {
		// 版本错误
		// 清理网络库
		fprintf(stderr, "Has a invalid network version.\n");
		WSACleanup();
		// 返回值1
		return EXIT_FAILURE;
	}

	// 建立服务器端套接字
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Creating the server socket....." << std::endl;
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == server_socket) {
		// 套接字开启失败
		// 获取到失败码
		int failed_code = WSAGetLastError();
		fprintf(stderr, "Socket create failed. Error code: %d\n", failed_code);
		// 套接字打开失败不需要进行关闭
		// 清理网络库
		WSACleanup();
		// 返回值1
		return EXIT_FAILURE;
	}

	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Binding the socket address..." << std::endl;
	// 设置绑定地址和端口号
	sockaddr_in server_bind = {};
	server_bind.sin_family = AF_INET;
	server_bind.sin_port = htons(IOT_FRAME_PORT);

	// 绑定地址，inet_addr被弃用
	in_addr addr;
	inet_pton(AF_INET, IOT_FRAME_IP_ADDR, (void*)& addr);
	server_bind.sin_addr = addr;

	// 打印需要绑定的地址和端口
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	printf("IP addr: %d.%d.%d.%d\n",
		server_bind.sin_addr.S_un.S_un_b.s_b1,
		server_bind.sin_addr.S_un.S_un_b.s_b2,
		server_bind.sin_addr.S_un.S_un_b.s_b3,
		server_bind.sin_addr.S_un.S_un_b.s_b4);

	printf("PORT: %d\n", IOT_FRAME_PORT);

	// 绑定地址和端口
	int bind_error_code = bind(server_socket, (const sockaddr*)& server_bind, sizeof(server_bind));

	if (SOCKET_ERROR == bind_error_code) {
		// 绑定地址出错
		// 获取到失败码
		int failed_code = WSAGetLastError();
		fprintf(stderr, "Bind failed. Error code: %d\n", failed_code);
		// 套接字打开失败不需要进行关闭
		// 清理网络库
		WSACleanup();
		// 返回值1
		return EXIT_FAILURE;
	}

	// 监听端口
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Listening....." << std::endl;
	int listen_flag = listen(server_socket, SOMAXCONN);

	if (SOCKET_ERROR == listen_flag) {
		// 获取到失败码
		int failed_code = WSAGetLastError();
		fprintf(stderr, "Listen failed. Error code: %d\n", failed_code);
		// 关闭套接字
		closesocket(server_socket);
		// 清理网络库
		WSACleanup();
		// 返回值1
		return EXIT_FAILURE;
	}

	// 客户端连接
	int id = 0;
	std::vector<SOCKET*> socket_vector;
	std::vector<std::thread*> thread_vector;
	sockaddr_in* client_info = nullptr;
	SOCKET* socket_client = nullptr;
	for (int i = 0; i < 1000; i++) {
		// 动态分配内存用于存储客户端信息和建立客户端套接字
		client_info = new sockaddr_in;
		socket_client = new SOCKET;

		// 获取长度
		int len = sizeof(sockaddr_in);

		std::cout << "Waiting for the connection....." << std::endl;

		// 阻塞连接
		*socket_client = accept(server_socket, (sockaddr*)client_info, &len);

		std::cout << "Connection built....." << std::endl;

		// 客户端IP地址
		printf("Client IP addr: %d.%d.%d.%d\n",
			client_info->sin_addr.S_un.S_un_b.s_b1,
			client_info->sin_addr.S_un.S_un_b.s_b2,
			client_info->sin_addr.S_un.S_un_b.s_b3,
			client_info->sin_addr.S_un.S_un_b.s_b4);

		// 客户端端口
		printf("Client PORT: %d\n", client_info->sin_port);

		// 获取到不可用客户端套接字做处理
		if (*socket_client == INVALID_SOCKET) {
			// 获取到失败码
			int failed_code = WSAGetLastError();
			fprintf(stderr, "Create client socket failed. Error code: %d\n", failed_code);
			// 关闭套接字
			closesocket(*socket_client);
			// 删除动态分配的内存，指向nullptr
			delete client_info;
			delete socket_client;
			client_info = nullptr;
			socket_client = nullptr;
			continue;
		}

		socket_vector.push_back(socket_client);

		std::thread* task = new std::thread(ThreadFunc, socket_client, id, *client_info);
		thread_vector.push_back(task);
		task->detach();

		id++;
		delete client_info;
		client_info = nullptr;
		socket_client = nullptr;
	}

	// 遍历指针，删除不为空的动态分配内存
	for (auto task_pt : thread_vector) {
		if (task_pt != nullptr) {
			delete task_pt;
			task_pt = nullptr;
			puts("Delete a task thread pointer.");
		}
		puts("Skip a task thread pointer.");
	}

	// 遍历指针，删除不为空的动态分配内存
	for (auto socket_pt : socket_vector) {
		if (socket_pt != nullptr) {
			closesocket(*socket_pt);
			delete socket_pt;
			socket_pt = nullptr;
			puts("Delete a client socket pointer.");
		}
		puts("Skip a client socket pointer.");
	}

	closesocket(server_socket);
	WSACleanup();

	return 0;
}