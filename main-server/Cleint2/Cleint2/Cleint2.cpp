#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <stdlib.h>

#pragma warning(disable: 4996)

SOCKET Connection;
bool close = false;

void ClientHandler() {
	char msg[256];
	while (!close) {
		recv(Connection, msg, sizeof(msg), NULL);
		std::cout << msg << std::endl;
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (close) {
		return 1;
	}
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	char msg1[256];
	char exit[5] = "exit";
	while (true) {
		std::cin.getline(msg1, sizeof(msg1));
		std::cout << msg1[0] << exit[0] << (msg1[0] == exit[0]) << std::endl;
		std::cout << (msg1[0] == exit[0]) << (msg1[1] == exit[1]) << (msg1[2] == exit[2]) << (msg1[3] == exit[3]) << (msg1[4] == exit[4]) << std::endl;

		send(Connection, msg1, sizeof(msg1), NULL);
		Sleep(10);
		if ((msg1[0] == exit[0]) && (msg1[1] == exit[1]) && (msg1[2] == exit[2]) && (msg1[3] == exit[3]) && (msg1[4] == exit[4])) {
			std::cout << "work" << std::endl;
			close = true;
			std::exit(EXIT_SUCCESS);
		}
	}
	closesocket(Connection);
	WSACleanup();
	system("pause");
	return 0;
}