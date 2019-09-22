#pragma once

#include <WinSock2.h>

void ThreadFunc(SOCKET* client_socket, int id, sockaddr_in client_info);