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
	// ���������
	//std::cout << "Init the network library...." << std::endl;
	WORD network_version = MAKEWORD(2, 2);
	WSADATA wsa_data{};

	// ���������
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Openning the WSA...." << std::endl;
	int wsa_error_code = WSAStartup(network_version, &wsa_data);

	if (0 != wsa_error_code) {
		// ���������ʧ��
		// ��ȡ��ʧ����
		int failed_code = WSAGetLastError();
		fprintf(stderr, "WSA open failed. Error code: %d\n", failed_code);
		// ����ֵ1
		return EXIT_FAILURE;
	}

	// ��鿪���汾�Ƿ���ȷ
	if (2 != HIBYTE(wsa_data.wVersion) || 2 != LOBYTE(wsa_data.wVersion)) {
		// �汾����
		// ���������
		fprintf(stderr, "Has a invalid network version.\n");
		WSACleanup();
		// ����ֵ1
		return EXIT_FAILURE;
	}

	// �������������׽���
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Creating the server socket....." << std::endl;
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == server_socket) {
		// �׽��ֿ���ʧ��
		// ��ȡ��ʧ����
		int failed_code = WSAGetLastError();
		fprintf(stderr, "Socket create failed. Error code: %d\n", failed_code);
		// �׽��ִ�ʧ�ܲ���Ҫ���йر�
		// ���������
		WSACleanup();
		// ����ֵ1
		return EXIT_FAILURE;
	}

	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Binding the socket address..." << std::endl;
	// ���ð󶨵�ַ�Ͷ˿ں�
	sockaddr_in server_bind = {};
	server_bind.sin_family = AF_INET;
	server_bind.sin_port = htons(IOT_FRAME_PORT);

	// �󶨵�ַ��inet_addr������
	in_addr addr;
	inet_pton(AF_INET, IOT_FRAME_IP_ADDR, (void*)& addr);
	server_bind.sin_addr = addr;

	// ��ӡ��Ҫ�󶨵ĵ�ַ�Ͷ˿�
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	printf("IP addr: %d.%d.%d.%d\n",
		server_bind.sin_addr.S_un.S_un_b.s_b1,
		server_bind.sin_addr.S_un.S_un_b.s_b2,
		server_bind.sin_addr.S_un.S_un_b.s_b3,
		server_bind.sin_addr.S_un.S_un_b.s_b4);

	printf("PORT: %d\n", IOT_FRAME_PORT);

	// �󶨵�ַ�Ͷ˿�
	int bind_error_code = bind(server_socket, (const sockaddr*)& server_bind, sizeof(server_bind));

	if (SOCKET_ERROR == bind_error_code) {
		// �󶨵�ַ����
		// ��ȡ��ʧ����
		int failed_code = WSAGetLastError();
		fprintf(stderr, "Bind failed. Error code: %d\n", failed_code);
		// �׽��ִ�ʧ�ܲ���Ҫ���йر�
		// ���������
		WSACleanup();
		// ����ֵ1
		return EXIT_FAILURE;
	}

	// �����˿�
	std::this_thread::sleep_for(std::chrono::microseconds(DELAY_TIME));
	std::cout << "Listening....." << std::endl;
	int listen_flag = listen(server_socket, SOMAXCONN);

	if (SOCKET_ERROR == listen_flag) {
		// ��ȡ��ʧ����
		int failed_code = WSAGetLastError();
		fprintf(stderr, "Listen failed. Error code: %d\n", failed_code);
		// �ر��׽���
		closesocket(server_socket);
		// ���������
		WSACleanup();
		// ����ֵ1
		return EXIT_FAILURE;
	}

	// �ͻ�������
	int id = 0;
	std::vector<SOCKET*> socket_vector;
	std::vector<std::thread*> thread_vector;
	sockaddr_in* client_info = nullptr;
	SOCKET* socket_client = nullptr;
	for (int i = 0; i < 1000; i++) {
		// ��̬�����ڴ����ڴ洢�ͻ�����Ϣ�ͽ����ͻ����׽���
		client_info = new sockaddr_in;
		socket_client = new SOCKET;

		// ��ȡ����
		int len = sizeof(sockaddr_in);

		std::cout << "Waiting for the connection....." << std::endl;

		// ��������
		*socket_client = accept(server_socket, (sockaddr*)client_info, &len);

		std::cout << "Connection built....." << std::endl;

		// �ͻ���IP��ַ
		printf("Client IP addr: %d.%d.%d.%d\n",
			client_info->sin_addr.S_un.S_un_b.s_b1,
			client_info->sin_addr.S_un.S_un_b.s_b2,
			client_info->sin_addr.S_un.S_un_b.s_b3,
			client_info->sin_addr.S_un.S_un_b.s_b4);

		// �ͻ��˶˿�
		printf("Client PORT: %d\n", client_info->sin_port);

		// ��ȡ�������ÿͻ����׽���������
		if (*socket_client == INVALID_SOCKET) {
			// ��ȡ��ʧ����
			int failed_code = WSAGetLastError();
			fprintf(stderr, "Create client socket failed. Error code: %d\n", failed_code);
			// �ر��׽���
			closesocket(*socket_client);
			// ɾ����̬������ڴ棬ָ��nullptr
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

	// ����ָ�룬ɾ����Ϊ�յĶ�̬�����ڴ�
	for (auto task_pt : thread_vector) {
		if (task_pt != nullptr) {
			delete task_pt;
			task_pt = nullptr;
			puts("Delete a task thread pointer.");
		}
		puts("Skip a task thread pointer.");
	}

	// ����ָ�룬ɾ����Ϊ�յĶ�̬�����ڴ�
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