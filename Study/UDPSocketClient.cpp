#include <iostream>
#include <thread>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void HandleError(const char *cause)
{
    int errCode = WSAGetLastError();
    cout << cause << " ErrorCode: " << errCode << endl;
}

int main()
{
    // 1. Socket 초기화
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        HandleError("Socket");
        return 0;
    }

    // 2. IP, PORT 설정
    char IP[] = "127.0.0.1";
    u_short PORT = 7777;

    SOCKADDR_IN serverAddr; // IPv4
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, IP, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(PORT);

    // Connected UDP
    connect(clientSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

    // 3. 데이터 송수신
    while (true)
    {
        char sendBuffer[100] = "Hello World!";

        // sendto -> 클라이언트의 IP주소, 포트번호 설정
        // Unconnected UDP
        // int resultCode = sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

        // Connected UDP
        int resultCode = send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
        if (resultCode == SOCKET_ERROR)
        {
            HandleError("SendTo");
            return 0;
        }

        cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;

        // Echo Receiver
        {
            SOCKADDR_IN recvAddr;
            memset(&recvAddr, 0, sizeof(recvAddr));
            int addrLen = sizeof(recvAddr);

            char recvBuffer[1000];

            // Unconnected UDP
            // int recvLen = recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&recvAddr, &addrLen);

            // Connected UDP
            int recvLen = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

            if (recvLen <= 0)
            {
                HandleError("RecvFrom");
                return 0;
            }

            cout << "Recv Data! Len = " << recvLen << endl;
            cout << "Recv Data! Data = " << recvBuffer << endl;
        }

        std::this_thread::sleep_for(1s);
    }

    // 4. Socket 종료
    closesocket(clientSocket);
    WSACleanup();
}