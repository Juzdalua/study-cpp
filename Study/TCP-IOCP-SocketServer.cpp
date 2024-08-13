#include <iostream>
#include <vector>
#include <thread>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int BUFSIZE = 1'000;
struct Session
{
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int recvBytes = 0;
};

enum IO_TYPE
{
    READ,
    WRITE,
    ACCEPT,
    CONNECT,
};

struct OverlappedEx
{
    WSAOVERLAPPED overlapped = {};
    int type = 0; // IO_TYPE
};

// 5. 입출력 함수 완료후 소켓을 다룰 스레드 정의
void WorkerThreadMain(HANDLE iocpHandle)
{
    while (true)
    {
        DWORD bytesTransfered = 0;
        Session *session = nullptr;
        OverlappedEx *overlappedEx = nullptr;

        // INFINITE -> 일감이 있을 때까지 대기
        BOOL ret = GetQueuedCompletionStatus(iocpHandle, &bytesTransfered, (ULONG_PTR *)&session, (LPOVERLAPPED *)&overlappedEx, INFINITE);

        if (ret == FALSE || bytesTransfered == 0)
        {
            // TODO: 연결 끊김
            continue;
        }

        cout << "Recv Data IOCP = " << bytesTransfered << endl;

        // 6. 다시 입출력 함수를 사용
        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        DWORD recvLen = 0;
        DWORD flags = 0;
        WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
    }
}

int main()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
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

    vector<Session *> sessionManager;

    // 1. CP 생성
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    // 4. 입출력 함수 완료 후 소켓을 다룰 스레드. WorkerThreads
    thread t1(WorkerThreadMain, iocpHandle);

    // Main Thread = Accept 담당
    while (true)
    {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, (SOCKADDR *)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
            return 0;

        Session *session = new Session();
        session->socket = clientSocket;
        sessionManager.push_back(session);

        cout << "Client Connectec!" << endl;

        // 2. 소켓을 CP에 등록
        CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*Key*/ (ULONG_PTR)session, 0);

        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        DWORD recvLen = 0;
        DWORD flags = 0;

        OverlappedEx *overlappedEx = new OverlappedEx();
        overlappedEx->type = IO_TYPE::READ;

        // 3. 소켓 처리 등록 후 메인스레드 종료.
        WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
    }

    if (t1.joinable())
        t1.join();
    WSACleanup();
}
