#include <iostream>
#include <vector>

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
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int recvBytes = 0;
    int sendBytes = 0;
};

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

    // Select 모델
    vector<Session> sessions;
    sessions.reserve(100);

    fd_set reads;
    fd_set writes;

    while (true)
    {
        // 소켓 셋 초기화
        FD_ZERO(&reads);
        FD_ZERO(&writes);

        // ListenSocket 등록 -> accept는 read
        FD_SET(listenSocket, &reads);

        // 소켓 등록
        for (Session &s : sessions)
        {
            if (s.recvBytes <= s.sendBytes)
                FD_SET(s.socket, &reads);
            else
                FD_SET(s.socket, &writes);
        }

        // [옵션] 마지막 timeout 인자 설정 가능 -> 대기시간. nullptr => 무한대기
        int retVal = select(0, &reads, &writes, nullptr, nullptr);
        if (retVal == SOCKET_ERROR)
            break;

        // Listener 소켓 체크
        if (FD_ISSET(listenSocket, &reads))
        {
            SOCKADDR_IN clientAddr;
            int addrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, (SOCKADDR *)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Clent Connected!" << endl;
                sessions.push_back(Session{clientSocket});
            }
        }

        // 나머지 소켓 체크
        for (Session &s : sessions)
        {
            // Read
            if (FD_ISSET(s.socket, &reads))
            {
                int recvLen = recv(s.socket, s.recvBuffer, BUFSIZE, 0);
                if (recvLen <= 0)
                {
                    // TODO: session 제거
                    continue;
                }

                s.recvBytes = recvLen;
            }

            // Write
            if (FD_ISSET(s.socket, &writes))
            {
                /*
                    블로킹 -> 모든 데이터 다 보냄
                    논블로킹 -> 일부만 보낼 수 있음.(상대방 수신 버퍼 상황에 따라)
                */
                int sendLen = send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == SOCKET_ERROR)
                {
                    // TODO: session 제거
                    continue;
                }

                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes)
                {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }
            }
        }
    }

    WSACleanup();
}
