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

const int BUFSIZE = 1'000;
struct Session
{
    WSAOVERLAPPED overlapped = {};
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
    cout << "Data Recv Len Callback = " << recvLen << endl;

    // TODO: Echo Server를 만든다면, WSASend()

    Session *session = (Session *)overlapped;
}

int main()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        return 0;

    // Non-Blocking Setting
    u_long on = 1;
    if (ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(7777);

    if (::bind(listenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        return 0;

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Accept" << endl;

    // Overlapped IO
    while (true)
    {
        SOCKET clientSocket;
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);

        while (true)
        {
            clientSocket = accept(listenSocket, (SOCKADDR *)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
                break;

            if (WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            // Error
            return 0;
        }

        Session session = Session{clientSocket};

        cout << "Client Connectec!" << endl;

        while (true)
        {
            WSABUF wsaBuf;
            wsaBuf.buf = session.recvBuffer;
            wsaBuf.len = BUFSIZE;

            DWORD recvLen = 0;
            DWORD flags = 0;

            // 비동기 함수 호출
            if (WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
            {
                if (WSAGetLastError() == WSA_IO_PENDING)
                {
                    // Alertable Wait
                    SleepEx(INFINITE, TRUE);
                    // WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);
                }
                else
                {
                    // TODO: Error
                    break;
                }
            }
            else
            {
                cout << "Data Recv Len = " << recvLen << endl;
            }
        }

        closesocket(session.socket);
    }
    WSACleanup();
}
