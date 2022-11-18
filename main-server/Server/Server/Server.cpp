#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

SOCKET Connections[100];
int Counter = 0;

void ClientHandler(int index) {
	setlocale(LC_ALL, "ru");
	int lenExit = 0;
	int exitArr[100];
	char exit[7] = "./exit";
	
	char msg[256];
	while (true) {
		recv(Connections[index], msg, sizeof(msg), NULL);
		bool cont = false;
		char myLogin[50] = "";
		char fullMsg[100]{};
		for (int i = 0; i < Counter; i++) {
			if ((msg[0] == exit[0]) && (msg[1] == exit[1]) && (msg[2] == exit[2]) && (msg[3] == exit[3]) && (msg[4] == exit[4]) && (msg[5] == exit[5])) {
				exitArr[lenExit] = index;
				lenExit += 1;

				for (int i = 0; i < strlen(msg); i++) {
					if (i < 6) continue;
					strncpy(myLogin, (msg + 6), sizeof(myLogin));
				}
				strcat(fullMsg, myLogin);
				strcat(fullMsg, " unconnected");
				for (int i = 0; i < Counter; i++) {
					if (i == index)continue;
					send(Connections[i], fullMsg, sizeof(fullMsg), NULL);
				}
				std::cout << "Client " << myLogin << " unconnect" << std::endl;
				ExitThread(0);
				
			}
			
			for (int j = 0; j < lenExit; j++) {
				if (exitArr[j] == i) {
					cont=true;
				}
			}
			if (i == index || cont==true) {
				continue;
			}

			send(Connections[i], msg, sizeof(msg), NULL);
			
		}

	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		std::exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("192.168.226.12");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);


	SOCKET newConnection;
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";
			char msg[256] = "Connection is true";
			send(newConnection, msg, sizeof(msg), NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}


	system("pause");
	return 0;
}