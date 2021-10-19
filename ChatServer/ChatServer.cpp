#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include <string>

#define MAXPENDINGCONNECTIONS 10
#define MAXRECVBUFFER 1024

using namespace std;

//Constants||
char END_OF_MESSAGE = '\20';
char MESSAGE_BREAK = '\22';
//_________||

struct user {
	char* ip;
	SOCKET client;
};

vector<user> connectedUsers;

void HandleClientConnection(user newUser)
{
	cout << "[" << newUser.ip << "]: Connected to the chat!" << endl;

	char buffer[MAXRECVBUFFER];
	while (recv(newUser.client, buffer, MAXRECVBUFFER, 0) > 0)
	{
		string message = "";

		for (int i = 0; i < MAXRECVBUFFER; i++) {
			if (buffer[i] == END_OF_MESSAGE)
				break;
			else
				message += buffer[i];
		}

		string newResponse(newUser.ip);

		newResponse.push_back(MESSAGE_BREAK);
		newResponse.append(message);
		newResponse.push_back(END_OF_MESSAGE);

		for (int i = 0; i < connectedUsers.size(); i++) {

			user currentUser = connectedUsers.at(i);

			if (currentUser.client == newUser.client)
				continue;

			if (send(currentUser.client, newResponse.c_str(), newResponse.length() , 0) == SOCKET_ERROR) {
				cout << "Message failed to replicate!" << endl;
			}
		}


	}
}

int main()
{
	SOCKET server;
	SOCKADDR_IN serverAddr;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != NO_ERROR)
	{
		cout << "WSAStartup failed!" << endl;
		return 1;
	}
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == SOCKET_ERROR)
	{
		cout << "socket failed!" << endl;
		return 2;
	}

	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(3434);

	if (bind(server, (struct sockaddr*)&serverAddr,
		sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cout << "bind failed!" << endl;
		return 3;
	}

	if (listen(server, MAXPENDINGCONNECTIONS) == SOCKET_ERROR)
	{
		cout << "listen failed!" << endl;
		return 4;
	}
	cout << "Server Started!" << endl;

	while (true)
	{
		SOCKET client;
		SOCKADDR_IN clientAddr;
		int clientLength = sizeof(clientAddr);
		client = accept(server, (struct sockaddr*)&clientAddr, &clientLength);
		if (client == INVALID_SOCKET)
		{
			cout << "accept failed!" << endl;
		}
		else
		{
			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, INET_ADDRSTRLEN);
				
			user newUser;
			newUser.client = client;
			newUser.ip = ip;

			connectedUsers.push_back(newUser);

			thread* clientThread = new thread(HandleClientConnection, newUser);
		}
	}
	WSACleanup();
	return 0;
}

