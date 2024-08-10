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

    // WSA Event Select 모델
    // 두 벡터의 인덱스에 따라 1:1 매칭을 해준다.
    vector<WSAEVENT> wsaEvents;
    vector<Session> sessions;
    sessions.reserve(100);

    // 1. 이벤트 생성
    WSAEVENT listenEvent = WSACreateEvent();
    wsaEvents.push_back(listenEvent);
    sessions.push_back(Session{listenSocket});

    // 2. 이벤트 연동
    if (WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
        return 0;

    while (true)
    {
        // 3. 이벤트 결과 확인
        int index = WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
        if (index == WSA_WAIT_FAILED)
        {
            // Error
            HandleError("3");
            continue;
        }
        index -= WSA_WAIT_EVENT_0;

        // Manual-Reset 수동리셋 -> WSAEnumNetworkEvents 함수를 사용한다면 생략 가능. 내장하고있음.
        WSAResetEvent(wsaEvents[index]);

        // 4. 완료된 이벤트 확인
        WSANETWORKEVENTS networkEvents;
        if (WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
        {
            // Error
            HandleError("4");
            continue;
        }

        // Listener Socket Check -> BIT FLAG
        if (networkEvents.lNetworkEvents & FD_ACCEPT)
        {
            // Error Check
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
            {
                // Error
                HandleError("Accept");
                continue;
            }

            // Accept 발생
            SOCKADDR_IN clientAddr;
            int addrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, (SOCKADDR *)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Client Connected" << endl;

                // 이벤트 변경
                WSAEVENT clientEvent = WSACreateEvent();
                wsaEvents.push_back(clientEvent);
                sessions.push_back(Session{clientSocket});
                if (WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
                    return 0;
            }
        }

        // Client Session Socket Check
        if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
        {
            // READ Error Check
            if ((networkEvents.lNetworkEvents & FD_READ) && networkEvents.iErrorCode[FD_READ_BIT] != 0)
            {
                // Error
                HandleError("READ");
                continue;
            }

            // WRITE Error Check
            if ((networkEvents.lNetworkEvents & FD_WRITE) && networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
            {
                // Error
                HandleError("WRITE");
                continue;
            }

            Session &s = sessions[index];

            // Read
            if (s.recvBytes == 0)
            {
                int recvLen = recv(s.socket, s.recvBuffer, BUFSIZE, 0);
                if (recvLen == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    // TODO: Error. Remove Session
                    HandleError("READ recv");
                    continue;
                }

                s.recvBytes = recvLen;
                cout << "Recv Data = " << recvLen << endl;
            }

            // Write
            if (s.recvBytes > s.sendBytes)
            {
                int sendLen = send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    // TODO: Error. Remove Session
                    HandleError("WRITE send");
                    continue;
                }

                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes)
                {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }

                cout << "Send Data = " << sendLen << endl;
            }

            // FD_CLOSE
            if (networkEvents.lNetworkEvents & FD_CLOSE)
            {
                // TODO: Remove Socket
                HandleError("CLOSE");
            }
        }
    }

    WSACleanup();
}
