#include "general.h"

vector<SOCKET> users;
map<SOCKET, string> user_names;

bool IsCommand(string message);
string ExtractCommand(string command);
string ExtractCommandParam(string command);
void ExitUser(SOCKET socket);
void recvFile(SOCKET socket);

void ClientHandler(SOCKET socket) {
	while (true) {
		vector<char> msg(40000);
 		const size_t code = recv(socket, &msg[0], msg.size(), NULL);

		if (code == 0) {
			continue;
		}
		if (code == -1) {
			ExitUser(socket);
			continue;
		}

		string str_message;
		if (code >= 0) {
			str_message.append(msg.cbegin(), msg.cend());
			str_message.resize(code);
		}
		cout << str_message << endl;



		if (IsCommand(str_message)) {
			const string command = ExtractCommand(str_message);
			cout << command << endl;
			if (command == "exit") {
				ExitUser(socket);
			}
			else if (command == "login") {
				const string login = ExtractCommandParam(str_message);
				user_names[socket] = login;
			}
			else if (command == "file") {
				recvFile(socket);
			}
			continue;
		}


		for (int i = 0; i < users.size(); i++) {
			if (users[i] == socket || users[i] == 0) {
				continue;
			}
			const string msg = user_names[socket] + ": " + str_message;
			send(users[i], (char*)msg.c_str(), msg.length() + 1, NULL);
		}

	}
}

int main(int argc, char* argv[]){
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	int error;

	const char IP_SERV[] = "192.168.56.1";
	const int PORT_NUM = 1111;

	WSADATA wsData;
	error = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (error != 0) {
		cout << "Error WinSock version initializaion #" << WSAGetLastError() << endl;
		return 1;
	}

	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, NULL);
	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}

	in_addr ip_to_num;
	error = inet_pton(AF_INET, IP_SERV, &ip_to_num);

	if (error <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	sockaddr_in servInfo;
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(PORT_NUM);

	error = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
	if (error != 0) {
		cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}

	error = listen(ServSock, SOMAXCONN);
	if (error != 0) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		cout << "Listening..." << endl;
	}
	
	SOCKET newConnection;
	for (int i = 0; i < 100; i++) {
		sockaddr_in clientInfo;
		int clientInfo_size = sizeof(clientInfo);
		newConnection = accept(ServSock, (SOCKADDR*)&clientInfo, &clientInfo_size);
		if (newConnection == INVALID_SOCKET) {
			cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
			closesocket(ServSock);
			closesocket(newConnection);
			WSACleanup();
			return 1;
		}
		else {
			cout << "Client Connected!" << endl;
			string msg = "Connection is true";
			send(newConnection, msg.c_str(), msg.length(), NULL);
			users.push_back(newConnection);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(newConnection), NULL, NULL);
		}
	}


	system("pause");
	return 0;
}


bool IsCommand(string message) {
	return message.find("./") == 0;
}

string ExtractCommand(string command) {
	const int end_command_exit = command.find("|");
	return command.substr(2, end_command_exit - 2);
}
string ExtractCommandParam(string command) {
	const int start_command_param = command.find("|");
	return command.substr(start_command_param + 1);
}

void ExitUser(SOCKET socket) {
	const auto login = user_names[socket];
	string fullMsg = (login == "" ? "anonim" : login) + " unconnected";
	for (auto i = 0; i < users.size(); i++) {
		if (users[i] == socket)  continue;
		send(users[i], fullMsg.c_str(), fullMsg.length(), NULL);
	}
	cout << fullMsg << endl;
	closesocket(socket);
	user_names.erase(socket);
	auto item = find(users.begin(), users.end(), socket);
	if (item != users.end()) {
		users.erase(item);
	}
	ExitThread(0);
}
void recvFile(SOCKET socket) {

	char file_name[40];
	char file_size_str[16];
	
	recv(socket, file_name, 40, 0);

	recv(socket, file_size_str, 16, 0);
	int file_size = atoi(file_size_str);
	char* bytes = new char[file_size];
	
	recv(socket, bytes, file_size, 0);

	cout << "name " << file_name << endl;
	cout << "size " << file_size << endl;
	
	for (int i = 0; i < users.size(); i++) {
		if (users[i] == socket || users[i] == 0) {
			continue;
		}
		char file_command[8] = "./file|";
		send(users[i], file_command, 8, 0);
		send(users[i], file_name, 40, 0);
		send(users[i], to_string(file_size).c_str(), 16, 0);
		send(users[i], bytes, file_size, 0);
	}
	cout << "server send file" << endl;
}
