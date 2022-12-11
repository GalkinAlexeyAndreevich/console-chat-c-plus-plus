#include <general.h>


const string exit_command = "./exit|";
const string login_command = "./login|";
const string file_command = "./file|";

SOCKET Connection;

void sendFile(SOCKET socket, string file_name);
void recvFile(SOCKET socket);
void ClientHandler() {
	
	cout << "to exit, enter " << exit_command << endl;
	cout << "to change login enter " << login_command <<" + your new login" << endl;
	cout << "to send the file enter " << file_command << " + file name(no dir) and in this path(where .exe)" << endl;
	vector<char> msg(40000);
	while (true) {
		int received_count = recv(Connection, &msg[0], msg.size(), NULL);
		string str_message;
		if (received_count >= 0) {
			str_message.append(msg.cbegin(), msg.cend());
			str_message.resize(received_count);
		}
		if (received_count == -1) {
			cout << "you unconnect" << endl;
			closesocket(Connection);
			exit(EXIT_SUCCESS);
		}
		if (received_count == 0) {
			continue;
		}

		if (str_message.substr(0, 7) == file_command) {
			recvFile(Connection);
		}
		else {
			cout<< str_message << endl;
		}
		
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	const char SERVER_IP[] = "192.168.56.1";				
	const short SERVER_PORT_NUM = 1111;				

	int error;									

	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP, &ip_to_num);

	WSADATA wsData;
	error = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (error != 0) {
		cout << "Error WinSock version initializaion #" << WSAGetLastError() << endl;
		return 1;
	}

	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
	}

	sockaddr_in servInfo;

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERVER_PORT_NUM);



	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	error = connect(Connection, (SOCKADDR*)&servInfo, sizeof(servInfo));
	if (error != 0) {
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	cout << "Connected!" << endl;
	string login;
	cout << "your login:" << endl;
	getline(cin, login);
	login = login_command + login;
	send(Connection, login.c_str(), login.length(), NULL);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
	
	while (true) {
		string message;
		getline(cin,message);

		if (message.length() == 0) continue;

		send(Connection, message.c_str(), message.length(), NULL);

		if(message.substr(0, 7) == exit_command)exit(EXIT_SUCCESS);
		if(message.substr(0, 7) == file_command)sendFile(Connection, message.substr(7));
		
		Sleep(10);
	}
	closesocket(Connection);
	WSACleanup();
	system("pause");
	return 0;

}

void sendFile(SOCKET socket, string file_name) {
	fstream file;
	
	file.open(file_name, ios_base::in | ios_base::binary);

	if (file.is_open()) {

		int file_size = fs::file_size(file_name) + 1;

		char* bytes = new char[file_size];

		file.read(bytes, file_size);

		cout << "name " << file_name << endl;
		cout << "size " << file_size << endl;

		send(socket, file_name.c_str(), 40, 0);
		
		send(socket, to_string(file_size).c_str(), 16, 0);
		send(socket, bytes, file_size, 0);
		
		
	}
	else {
		cout << "Error file open" << endl;
	}
	file.close();
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

	fstream file;
	file.open(file_name, ios_base::out | ios_base::binary);


	if (file.is_open()) {
		file.write(bytes, file_size);
		cout << "ok save" << endl;
	}
	else {
		cout << "Error file open" << endl;
	}

	delete[] bytes;
	file.close();
}
