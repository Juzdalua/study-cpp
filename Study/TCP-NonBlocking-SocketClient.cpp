#include <iostream>
#include <thread>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	// Non-Blocking Setting
	u_long on = 1;
	if (ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = htons(7777);

	// Connect
	while (true)
	{
		if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			// 아직 연결 진행중이다.
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// 이미 연결된 상태
			if (WSAGetLastError() == WSAEISCONN)
				break;

			// Error
			break;
		}
	}

	cout << "Connected to Sever!" << endl;

	// Send
	char sendBuffer[100] = "Hello World";
	while (true)
	{
		if (send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			// 전송할 버퍼 공간이 없음
			// 상대방 recvBuffer 공간이 없음
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// Error
			break;
		}
		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;

		// Recv
		while (true)
		{
			char recvBuffer[1000];
			int recvLen = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				// recvBuffer가 비어있는 상태
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				// Error
				break;
			}
			else if (recvLen == 0)
			{
				// 연결 끊김
				break;
			}

			cout << "Recv Data Len = " << recvLen << endl;
			break;
		}

		this_thread::sleep_for(1s);
	}

	closesocket(clientSocket);
	WSACleanup();
}