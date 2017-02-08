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


sharedinfo sharedInfo = {0};

void serverTCP(int port){
    resultClear();
    resultAdd("Starting TCP Server.");
    WSADATA wsaData;
    SOCKADDR_IN addr;
    WSAEVENT event;
    SOCKET tempSock;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

    if(WSAStartup(0x0202,&wsaData) != 0){
        resultAdd("WSAStartup Failed");
        return;
    }
    if((tempSock = WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultAdd("WSASocket Failed");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if(bind(tempSock, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultAdd("bind Failed");
        return;
    }

    if(listen(tempSock, 5)){
        resultAdd("listen Failed");
        return;
    }

    if((event = WSACreateEvent()) == WSA_INVALID_EVENT){
        resultAdd("WSACreateEvent Failed");
        return;
    }

    //add checkbox for whether or not to loop for new connections
    while(sharedInfo.running){//accept connections one after another
        resultAdd("Waiting for connection...");
        if((sharedInfo.sharedSocket = accept(tempSock,0,0)) == INVALID_SOCKET){
            resultAdd("accept Failed");
            return;
        }
        resultAdd("Connected.");
        DWORD flags = 0;
        if(sharedInfo.buffer == 0){
            sharedInfo.buffer = static_cast<char*>(malloc(2048 * sizeof(char)));
            sharedInfo.wsabuff.buf = sharedInfo.buffer;
            sharedInfo.wsabuff.len = 2048;
        }
        if(WSARecv(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   &flags, &sharedInfo.overlapped, workerRoutine_TCPserver)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                closesocket(sharedInfo.sharedSocket);
                resultAdd("WSARecv Failed");
                return;
            }
        }
    }
    resultAdd("Stopping...");
}


void CALLBACK workerRoutine_TCPserver(DWORD error, DWORD bytesTrans,
                                      LPWSAOVERLAPPED overlapped, DWORD inFlags){
    if(error){
        closesocket(sharedInfo.sharedSocket);
        resultAdd("Error in WSARecv");
        return;
    }
    if (bytesTrans) {
        resultAdd("Got Something!");
        //save it
    } else {
        resultAdd("Nothing read");
    }

    DWORD flags = 0;
    if(WSARecv(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
               &flags, &sharedInfo.overlapped, workerRoutine_TCPserver)){
        if(WSAGetLastError() != WSA_IO_PENDING){
            resultAdd("WSARecv Failed");
            return;
        }
    }
}


void serverUDP(int port){
    resultClear();
    resultAdd("Starting UDP Server.");
}

void CALLBACK workerRoutine_UDPserver(DWORD error, DWORD bytesTrans,
                                      LPWSAOVERLAPPED overlapped, DWORD inFlags){
}












//starts client
void clientTCP(string dest, int  port, int size, int number){
    resultClear();
    resultAdd("Starting TCP Client.");
    WSADATA wsaData;
    SOCKADDR_IN addr;
    WSAEVENT event;
    SOCKET sendSock;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

    if(WSAStartup(0x0202,&wsaData) != 0){
        resultAdd("WSAStartup Failed");
        return;
    }
    if((sendSock = WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultAdd("WSASocket Failed");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    hostent *ent;
    if(!(ent = gethostbyname(dest.c_str()))){
        resultAdd("could not find hostname");
        return;
    }

    memcpy((char *)&addr.sin_addr, ent->h_addr, ent->h_length);

    resultAdd("Trying to connect...");
    if(connect(sendSock, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultAdd("connect Failed");
        return;
    }
    resultAdd("Connected.");
    sharedInfo.sharedSocket = sendSock;

    resultAdd("Sending...");

    if(sharedInfo.buffer != 0){
        free(sharedInfo.buffer);
    }
    sharedInfo.buffer = static_cast<char*>(malloc(size * sizeof(char)));
    memset(sharedInfo.buffer,'a', size);
    while(sharedInfo.running && number--){//keep sending
        DWORD flags = 0;
        sharedInfo.wsabuff.buf = sharedInfo.buffer;
        sharedInfo.wsabuff.len = size;
        if(WSASend(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   flags, &sharedInfo.overlapped, workerRoutine_TCPclient)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultAdd("WSASend Failed");
                return;
            }
        }
        //increment status bar
    }
    if(sharedInfo.running)
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
}

void CALLBACK workerRoutine_TCPclient(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags){
    if(bytesTrans) {
        resultAdd("Got Something!");
    } else {
        resultAdd("Nothing read");
    }
}

void clientUDP(string dest, int  port, int size, int number){
    resultClear();
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
