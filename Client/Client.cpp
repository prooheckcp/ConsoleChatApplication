#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <list>
#include <thread>

#define MAXRECVBUFFER 1024

using namespace std;

//Constants||
char END_OF_MESSAGE = '\20';
char MESSAGE_BREAK = '\22';
//_________||

void HandleServerConnection(SOCKET server) {

	char buffer[MAXRECVBUFFER];
	while (recv(server, buffer, MAXRECVBUFFER, 0) > 0)
	{
		string ip;
		string message;
		bool parsingID = true;

		for (int i = 0; i < MAXRECVBUFFER; i++) {
			char currentCharacter = buffer[i];
			if (parsingID) {
				if (currentCharacter == MESSAGE_BREAK) {
					parsingID = false;
					continue;
				}
				else {
					ip += currentCharacter;
				}
			}
			else {
				if (currentCharacter == END_OF_MESSAGE) 
					break;
				else
					message += currentCharacter;
			}
		}

		cout << "[" << ip << "]:" << message << endl;
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
		message += END_OF_MESSAGE;
		if (send(server, message.c_str(), message.length() + 1, 0) == SOCKET_ERROR)
		{
			cout << "send failed!" << endl;
			return 4;
		}

	}
	if (closesocket(server) == SOCKET_ERROR)
	{
		cout << "closesocket failed!" << endl;
		return 5;
	}
	WSACleanup();
	return 0;
}
