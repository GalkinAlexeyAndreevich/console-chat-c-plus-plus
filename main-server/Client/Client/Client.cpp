#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <stdlib.h>

#pragma warning(disable: 4996)

SOCKET Connection;
void ClientHandler() {
	char msg[256];
	std::cout << "для выхода введите ./exit" << std::endl;
	while (true) {
		recv(Connection, msg, sizeof(msg), NULL);
		std::cout << msg << std::endl;
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	setlocale(LC_ALL, "ru");
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("192.168.226.12");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";
	char login[70];
	std::cout << "your login:" << std::endl;
	std::cin.getline(login, sizeof(login));

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	char msg1[256];
	
	char exit[7] = "./exit";
	char newW[100] = "./exit";
	char dv[5] =":";
	
	while (true) {
		char fullMsg[256]{};
		bool close = false;
		std::cin.getline(msg1, sizeof(msg1));
		if ((msg1[0] == exit[0]) && (msg1[1] == exit[1]) && (msg1[2] == exit[2]) && (msg1[3] == exit[3]) && (msg1[4] == exit[4]) && (msg1[5] == exit[5])) {
			close = true;
			strcat(fullMsg, msg1);
			strcat(fullMsg, login);
		}
		else {
			strcat(fullMsg, login);
			strcat(fullMsg, dv);
			strcat(fullMsg, msg1);
		}

		send(Connection, fullMsg, sizeof(fullMsg), NULL);
		Sleep(10);
		if (close) {
			std::cout << "you unconnect" << std::endl;
			std::exit(EXIT_SUCCESS);
		}
	}
	closesocket(Connection);
	WSACleanup();
	system("pause");
	return 0;
}