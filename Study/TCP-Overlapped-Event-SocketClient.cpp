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
    this_thread::sleep_for(1s);

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
        if (connect(clientSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
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
    WSAEVENT wsaEvent = WSACreateEvent();
    WSAOVERLAPPED overlapped = {};
    overlapped.hEvent = wsaEvent;

    while (true)
    {
        WSABUF wsaBuf;
        wsaBuf.buf = sendBuffer;
        wsaBuf.len = 100;

        DWORD sendLen = 0;
        DWORD flags = 0;

        if (WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSA_IO_PENDING)
            {
                WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
                WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
            }
            else
            {
                // Error
                break;
            }
        }

        cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
        // this_thread::sleep_for(1s);
    }

    closesocket(clientSocket);
    WSACleanup();
}