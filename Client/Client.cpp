#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <list>
#include <thread>

#define MAXRECVBUFFER 1024

using namespace std;

struct PlayerInfo {
	int id;
	char avatar;
	int positionx;
	int positiony;
};

list<PlayerInfo> players;
int playerID = -1;

void HandleServerConnection(SOCKET server) {

	char buffer[MAXRECVBUFFER];
	while (recv(server, buffer, MAXRECVBUFFER, 0) > 0)
	{
		cout << "Message: " << buffer << endl;
	}
	cout << "Server commited seppuku" << endl;
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

	InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr.s_addr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(3434);

	if (connect(server, (struct sockaddr*)&serverAddr,
		sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cout << "connect failed!" << endl;
		return 3;
	}

	cout << "Connected to Server!" << endl;

	thread* serverThread = new thread(HandleServerConnection, server);

	string message = "";
	while (message != "exit")
	{
		getline(cin, message);
		if (send(server, message.c_str(), message.length() + 1, 0) == SOCKET_ERROR)
		{
			cout << "send failed!" << endl;
			return 4;
		}
		cout << "Message sent!" << endl;
	}
	if (closesocket(server) == SOCKET_ERROR)
	{
		cout << "closesocket failed!" << endl;
		return 5;
	}
	WSACleanup();
	return 0;
}
