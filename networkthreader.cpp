#include <thread>
#include <QMetaObject>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <sstream>

#include "networkthreader.h"
#include "mainwindow.h"

void *mainwindowptr;

using namespace std;
//starts server


sharedinfo sharedInfo = {};

void serverTCP(int port, int buffsize){
    resultClear();
    resultAdd("Starting TCP Server.");
    WSADATA wsaData;
    SOCKADDR_IN addr;
    SOCKET tempSock;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

    if(WSAStartup(0x0202,&wsaData) != 0){
        resultAdd("WSAStartup Failed.");
        return;
    }
    if((tempSock = WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if(bind(tempSock, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultError("bind Failed.");
        return;
    }

    if(listen(tempSock, 5)){
        resultError("listen Failed.");
        return;
    }

    //add checkbox for whether or not to loop for new connections
    while(sharedInfo.running){//accept connections one after another
        resultAdd("Waiting for connection...");
        if((sharedInfo.sharedSocket = accept(tempSock,0,0)) == INVALID_SOCKET){
            resultError("accept Failed.");
            return;
        }
        resultAdd("Connected.");
        DWORD flags = 0;
        if(sharedInfo.buffer == 0){
            sharedInfo.buffer = static_cast<char*>(malloc(buffsize * sizeof(char)));
            sharedInfo.wsabuff.buf = sharedInfo.buffer;
            sharedInfo.wsabuff.len = buffsize;
        }
        if(WSARecv(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   &flags, &sharedInfo.overlapped, workerRoutine_server)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                closesocket(sharedInfo.sharedSocket);
                resultError("WSARecv Failed.");
                return;
            }
        }
    }
    resultAdd("Stopping...");
}


void serverUDP(int port, int buffsize){
    resultClear();
    resultAdd("Starting UDP Server.");
    WSADATA wsaData;
    SOCKADDR_IN addr;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

    if(WSAStartup(0x0202,&wsaData) != 0){
        resultError("WSAStartup Failed.");
        return;
    }
    if((sharedInfo.sharedSocket = WSASocket(AF_INET,SOCK_DGRAM,0,0,0,WSA_FLAG_OVERLAPPED))
            == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if(bind(sharedInfo.sharedSocket, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultError("bind Failed.");
        return;
    }

    //add checkbox for whether or not to loop for new connections
    DWORD flags = 0;
    if(sharedInfo.buffer == 0){
        sharedInfo.buffer = static_cast<char*>(malloc(buffsize * sizeof(char)));
        sharedInfo.wsabuff.buf = sharedInfo.buffer;
        sharedInfo.wsabuff.len = buffsize;
    }
    //sharedinfo temp = sharedInfo;
    if(WSARecvFrom(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   &flags, 0, 0, &sharedInfo.overlapped, workerRoutine_server)){
        if(WSAGetLastError() != WSA_IO_PENDING){
            resultError("WSARecv Failed.");
            closesocket(sharedInfo.sharedSocket);
            return;
        }
    }
    resultAdd("Waiting for datagram...");
}

void CALLBACK workerRoutine_server(DWORD error, DWORD bytesTrans,
                                   LPWSAOVERLAPPED, DWORD){
    if(error){
        if(sharedInfo.running){
            resultError("Error in WSARecv.");
        }
        resultAdd("Quitting.");
        return;
    }
    if (bytesTrans) {
        resultAdd("== read>");
        resultAdd(string(sharedInfo.buffer, bytesTrans));
        resultAdd("== endr>");
        //save it
    } else {
        resultAdd("Nothing to read.");
    }

    if(sharedInfo.running) {
        DWORD flags = 0;
        if(WSARecv(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   &flags, &sharedInfo.overlapped, workerRoutine_server)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultError("WSARecv Failed.");
                return;
            }
        }
    }
}

//starts client
void clientTCP(string dest, int  port, int size, int number){
    resultClear();
    resultAdd("Starting TCP Client.");
    WSADATA wsaData;
    SOCKADDR_IN addr;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

    if(WSAStartup(0x0202,&wsaData) != 0){
        resultError("WSAStartup Failed.");
        return;
    }
    if((sharedInfo.sharedSocket = WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED))
            == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    hostent *ent;
    if(!(ent = gethostbyname(dest.c_str()))){
        resultError("could not find hostname.");
        return;
    }

    memcpy((char *)&addr.sin_addr, ent->h_addr, ent->h_length);

    resultAdd("Trying to connect...");
    if(connect(sharedInfo.sharedSocket, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultError("connect Failed.");
        return;
    }
    resultAdd("Connected.");
    resultAdd("Sending...");
    if(sharedInfo.buffer != 0){
        free(sharedInfo.buffer);
    }
    sharedInfo.buffer = static_cast<char*>(malloc(size * sizeof(char)));
    memset(sharedInfo.buffer,'a', size);
    int total = number;
    while(sharedInfo.running && number--){//keep sending
        //increment status bar
        resultSet((total-number)/total*100);
        DWORD flags = 0;
        sharedInfo.wsabuff.buf = sharedInfo.buffer;
        sharedInfo.wsabuff.len = size;
        if(WSASend(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   flags, &sharedInfo.overlapped, workerRoutine_client)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultError("WSASend Failed.");
                return;
            }
        }
    }
    if(sharedInfo.running)
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
}

void clientUDP(string dest, int  port, int size, int number){
    resultClear();
    resultAdd("Starting UDP Client.");
    WSADATA wsaData;
    SOCKADDR_IN addr;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

    if(WSAStartup(0x0202,&wsaData) != 0){
        resultError("WSAStartup Failed.");
        return;
    }
    if((sharedInfo.sharedSocket = WSASocket(AF_INET,SOCK_DGRAM,0,0,0,WSA_FLAG_OVERLAPPED))
            == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    hostent *ent;
    if(!(ent = gethostbyname(dest.c_str()))){
        resultAdd("could not find hostname.");
        return;
    }

    memcpy((char *)&addr.sin_addr, ent->h_addr, ent->h_length);

    resultAdd("Sending...");
    if(sharedInfo.buffer != 0){
        free(sharedInfo.buffer);
    }
    sharedInfo.buffer = static_cast<char*>(malloc(size * sizeof(char)));
    memset(sharedInfo.buffer,'a', size);
    int total = number;
    while(sharedInfo.running && number--){//keep sending
        //increment status bar
        resultSet((total-number)/total*100);
        DWORD flags = 0;
        sharedInfo.wsabuff.buf = sharedInfo.buffer;
        sharedInfo.wsabuff.len = size;
        if(WSASendTo(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                     flags, reinterpret_cast<SOCKADDR*>(&addr),sizeof(SOCKADDR_IN),
                     &sharedInfo.overlapped, workerRoutine_client)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultError("WSASend Failed.");
                return;
            }
        }
    }
    if(sharedInfo.running)
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
}

void CALLBACK workerRoutine_client(DWORD error, DWORD bytesTrans,
                                   LPWSAOVERLAPPED, DWORD){
    if (error) {
        if(sharedInfo.running){
            resultError("Error in WSASend.");
        }
        resultAdd("Quitting.");
        return;
    }
    if(bytesTrans) {
        resultAdd("Data Sent.");
    }
}


void resultError(string msg) {
    std::stringstream ss;
    ss << msg << " Error Code " << WSAGetLastError();
    resultAdd(ss.str());
}

void resultAdd(string msg) {
    QMetaObject qtmo;
    qtmo.invokeMethod(reinterpret_cast<MainWindow*>(mainwindowptr),
                      "messageAdd_slot", Q_ARG(std::string, msg));
}

void resultSet(int percent) {
    QMetaObject qtmo;
    qtmo.invokeMethod(reinterpret_cast<MainWindow*>(mainwindowptr),
                      "messageSet_slot", Q_ARG(int, percent));
}

void resultClear() {
    QMetaObject qtmo;
    qtmo.invokeMethod(reinterpret_cast<MainWindow*>(mainwindowptr), "messageClear_slot");
}
