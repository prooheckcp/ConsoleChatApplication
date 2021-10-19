#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <list>
#include <string>

#define MAXPENDINGCONNECTIONS 10
#define MAXRECVBUFFER 1024

using namespace std;

/*CLIENT-SERVER MESSAGE PROTOCOL

CLIENT TO SERVER:
MOVE_TYPE | ID |
EXAMPLES: U|1, D|1, L|1, R|1

SERVER TO CLIENT:
CONNECTION CONFIRMATION:C|ID|AVATAR|posX|posY
UPDATE PLAYERS: U|ID|AVATAR|posX|posY


*/

struct PlayerInfo {
	SOCKET client;
	int id;
	char avatar;
	int positionx;
	int positiony;
};

list<PlayerInfo> players;
int playerCount = 0;

void HandleClientConnection(PlayerInfo player)
{
	cout << "User connect: " << player.id;

	char buffer[MAXRECVBUFFER];
	while (recv(player.client, buffer, MAXRECVBUFFER, 0) > 0)
	{

	}
	cout << "Player " << player.avatar << " disconnected!" << endl;
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
			cout << "Client " << ip << " connected!" << endl;

			PlayerInfo player;
			player.client = client;
			player.positionx = 1;
			player.positiony = 1;
			player.id = playerCount++;

			players.push_back(player);
			thread* clientThread = new thread(HandleClientConnection, player);
		}
	}
	WSACleanup();
	return 0;
}

