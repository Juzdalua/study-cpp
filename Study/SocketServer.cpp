#include <iostream>

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

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int errCode = WSAGetLastError();
		cout << "Socket ErrorCode: " << errCode << endl;
		return 0;
	}

	u_short PORT = 7777;
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(PORT);

	// bind
	if (::bind(listenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		cout << "Bind ErrorCode: " << errCode << endl;
		return 0;
	}

	// listen
	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		cout << "Listen ErrorCode: " << errCode << endl;
		return 0;
	}

	while (true)
	{
		SOCKADDR_IN clientAddr;
		memset(&clientAddr, 0, sizeof(clientAddr));
		int addrLen = sizeof(clientAddr);

		SOCKET clientSocket = accept(listenSocket, (SOCKADDR *)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int errCode = WSAGetLastError();
			cout << "Accept ErrorCode: " << errCode << endl;
			return 0;
		}

		// 연결 성공
		char ipAddress[16];
		inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "Client Connected! IP: " << ipAddress << endl;

		char recvBuffer[1000];
		int recvLen = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen <= 0)
		{
			int errCode = WSAGetLastError();
			cout << "Recv ErrorCode: " << errCode << endl;
			return 0;
		}

		cout << "Recv Data! Len = " << recvLen << endl;
		cout << "Recv Data! Data = " << recvBuffer << endl;

		// Echo Server
		int resultCode = send(clientSocket, recvBuffer, recvLen, 0);
		if (resultCode == SOCKET_ERROR)
		{
			int errCode = WSAGetLastError();
			cout << "Send ErrorCode: " << errCode << endl;
			return 0;
		}
	}

	WSACleanup();
}
