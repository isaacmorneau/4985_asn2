#include <thread>
#include <QMetaObject>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#include "networkthreader.h"
#include "mainwindow.h"

void *mainwindowptr;

using namespace std;
//starts server

SOCKET AcceptSocket;

void serverTCP(int port){
    resultAdd("Starting TCP Server.");
    WSADATA wsaData;
    SOCKET ListenSocket;
    SOCKADDR_IN InternetAddr;
    WSAEVENT AcceptEvent;

    if ((WSAStartup(0x0202,&wsaData)) != 0){
        resultAdd("WSAStartup failed.");
        WSACleanup();
        return;
    }
    if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
                                  WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultAdd("Failed to get socket.");
        return;
    }
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(port);

    if (bind(ListenSocket, (PSOCKADDR) &InternetAddr,
             sizeof(InternetAddr)) == SOCKET_ERROR){
        resultAdd("Bind failed.");
        return;
    }

    if (listen(ListenSocket, 5)){
        resultAdd("Listen failed.");
        return;
    }

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT){
        resultAdd("WSACreateEvent failed.");
        return;
    }

    thread worker(workerThread_TCPserver, AcceptEvent);
    worker.detach();
    resultAdd("Worker thread started.");
    while(1){
        AcceptSocket = accept(ListenSocket, NULL, NULL);

        if (!WSASetEvent(AcceptEvent)){
            resultAdd("WSASetEvent failed.");
            return;
        }
    }
}

void workerThread_TCPserver(WSAEVENT event){
    DWORD Flags;
    WSAEVENT EventArray[1];
    DWORD Index;
    LPSOCKET_INFO SocketInfo;
    DWORD RecvBytes;

    // Save the accept event in the event array.
    EventArray[0] = event;

    while(1){
        // Wait for accept() to signal an event and also process WorkerRoutine() returns.

        while(1){
            resultAdd("Waiting for connection.");
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

            if (Index == WSA_WAIT_FAILED) {
                resultAdd("WSAWaitForMultipleEvents failed.");
                return;
            }

            if (Index != WAIT_IO_COMPLETION){
                resultAdd("Accept Call is ready.");
                // An accept() call event is ready - break the wait loop
                break;
            }
        }

        WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

        // Create a socket information structure to associate with the accepted socket.

        if ((SocketInfo = (LPSOCKET_INFO) GlobalAlloc(GPTR,
                                                      sizeof(SOCKET_INFO))) == NULL){
            resultAdd("GlobalAlloc.");
            return;
        }

        // Fill in the details of our accepted socket.

        SocketInfo->Socket = AcceptSocket;
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        Flags = 0;
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                    &(SocketInfo->Overlapped), workerRoutine_TCPserver) == SOCKET_ERROR){
            if (WSAGetLastError() != WSA_IO_PENDING){
                resultAdd("WSARecv failed.");
                return;
            }
        }
        resultAdd("Socket connected.");
        //printf("Socket %d connected\n", AcceptSocket);
    }
}


void CALLBACK workerRoutine_TCPserver(DWORD Error, DWORD BytesTransferred,
                                      LPWSAOVERLAPPED Overlapped, DWORD InFlags){
    DWORD RecvBytes;
    DWORD Flags;
    resultAdd("Routine triggered.");

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFO SI = (LPSOCKET_INFO) Overlapped;

    if (Error != 0){
        resultAdd("IO operation failed.");
    }

    if (BytesTransferred == 0){
        resultAdd("Closing Socket.");
    }

    if (Error != 0 || BytesTransferred == 0){
        closesocket(SI->Socket);
        GlobalFree(SI);
        return;
    }

    Flags = 0;
    ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

    SI->DataBuf.len = DATA_BUFSIZE;
    SI->DataBuf.buf = SI->Buffer;

    //lets show the user what was read

    resultAdd("== start read>");

    // TODO write to disk
    resultAdd(string(SI->Buffer,BytesTransferred));

    resultAdd("== end of read>");
    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
                &(SI->Overlapped), workerRoutine_TCPserver) == SOCKET_ERROR){
        if (WSAGetLastError() != WSA_IO_PENDING ){
            resultAdd("WSARecv failed.");
            return;
        }
    }
}



void serverUDP(int port){
    resultAdd("Starting UDP Server.");

    WSADATA wsaData;
    WSABUF DataBuf;
    WSAOVERLAPPED Overlapped;

    SOCKET RecvSocket = INVALID_SOCKET;
    struct sockaddr_in RecvAddr;
    struct sockaddr_in SenderAddr;

    int SenderAddrSize = sizeof (SenderAddr);
    char RecvBuf[1024];
    int BufLen = 1024;
    DWORD BytesRecv = 0;
    DWORD Flags = 0;

    int err = 0;
    int rc;
    int retval = 0;

    //-----------------------------------------------
    // Initialize Winsock
    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc != 0) {
        /* Could not find a usable Winsock DLL */
        resultAdd("WSAStartup failed.");
        return;
    }

    // Make sure the Overlapped struct is zeroed out
    SecureZeroMemory((PVOID) &Overlapped, sizeof(WSAOVERLAPPED) );

    // Create an event handle and setup the overlapped structure.
    Overlapped.hEvent = WSACreateEvent();
    if (Overlapped.hEvent == NULL) {
        resultAdd("WSACreateEvent failed.");
        WSACleanup();
        return;
    }
    //-----------------------------------------------
    // Create a receiver socket to receive datagrams
    RecvSocket = WSASocket(AF_INET,
                           SOCK_DGRAM,
                           IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (RecvSocket == INVALID_SOCKET) {
        /* Could not open a socket */
        resultAdd("WSASocket failed.");
        WSACloseEvent(Overlapped.hEvent);
        WSACleanup();
        return;
    }
    //-----------------------------------------------
    // Bind the socket to any address and the specified port.
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(port);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    rc = bind(RecvSocket, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr));
    if (rc != 0) {
        /* Bind to the socket failed */
        resultAdd("bind failed.");
        WSACloseEvent(Overlapped.hEvent);
        closesocket(RecvSocket);
        WSACleanup();
        return;
    }

    //-----------------------------------------------
    // Call the recvfrom function to receive datagrams
    // on the bound socket.
    DataBuf.len = BufLen;
    DataBuf.buf = RecvBuf;

    resultAdd("Registered recive.");
    rc = WSARecvFrom(RecvSocket, &DataBuf, 1, &BytesRecv, &Flags,(SOCKADDR *) & SenderAddr,
                     &SenderAddrSize, &Overlapped, workerRoutine_UDPserver);

    if (rc != 0) {
        err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            resultAdd("WSARecvFrom failed.");
            WSACloseEvent(Overlapped.hEvent);
            closesocket(RecvSocket);
            WSACleanup();
            return;
        } else {
            resultAdd("WSA IO Pending");
            rc = WSAWaitForMultipleEvents(1, &Overlapped.hEvent, TRUE, INFINITE, TRUE);
            if (rc == WSA_WAIT_FAILED) {
                resultAdd("WSAWaitForMultipleEvents failed.");
                retval = 1;
            }

            rc = WSAGetOverlappedResult(RecvSocket, &Overlapped, &BytesRecv,
                                        FALSE, &Flags);
            if (rc == FALSE) {
                resultAdd("WSARecvFrom failed.");
                retval = 1;
            } else {
                resultAdd("== start read>");
                resultAdd(string(DataBuf.buf,BytesRecv));
                resultAdd("== end of read>");
            }

    resultAdd("Finishing, closing socket.");
        }

    }
    //---------------------------------------------
    // When the application is finished receiving, close the socket.

    WSACloseEvent(Overlapped.hEvent);
    closesocket(RecvSocket);
    resultAdd("Finished.");

    //---------------------------------------------
    // Clean up and quit.
    WSACleanup();
    //return (retval);
}

void CALLBACK workerRoutine_UDPserver(DWORD Error, DWORD BytesTransferred,
                                      LPWSAOVERLAPPED Overlapped, DWORD InFlags){
    DWORD RecvBytes;
    DWORD Flags;
    resultAdd("Routine triggered.");

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    /*LPSOCKET_INFO SI = (LPSOCKET_INFO) Overlapped;

    if (Error != 0){
        resultAdd("IO operation failed.");
    }

    Flags = 0;
    ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

    SI->DataBuf.len = DATA_BUFSIZE;
    SI->DataBuf.buf = SI->Buffer;

    //lets show the user what was read

    resultAdd("== start read>");

    // TODO write to disk
    //resultAdd(string(SI->Buffer,BytesTransferred));

    resultAdd("== end of read>");
    if (WSARecvFrom(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
                &(SI->Overlapped), workerRoutine_UDPserver) == SOCKET_ERROR){
        if (WSAGetLastError() != WSA_IO_PENDING ){
            resultAdd("WSARecv failed.");
            return;
        }
    }*/
}












//starts client
void clientTCP(string dest, int  port, int size, int number){
    resultAdd("Starting TCP Client.");
}

void clientUDP(string dest, int  port, int size, int number){
    resultAdd("Starting UDP Client.");
}

void resultAdd(string msg) {
    QMetaObject qtmo;
    qtmo.invokeMethod(reinterpret_cast<MainWindow*>(mainwindowptr),
                      "messageAdd_slot", Q_ARG(std::string, msg));
}

void resultClear() {
    QMetaObject qtmo;
    qtmo.invokeMethod(reinterpret_cast<MainWindow*>(mainwindowptr), "messageClear_slot");
}
