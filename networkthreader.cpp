#include <thread>
#include <QMetaObject>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <sstream>

#include "networkthreader.h"
#include "mainwindow.h"
#include "testset.h"

void *mainwindowptr;

using namespace std;
//starts server

sharedinfo sharedInfo{};

void serverTCP(int port, int buffsize){
    resultClear();
    resultAdd("Starting TCP Server.");
    SOCKADDR_IN addr;
    SOCKET tempSock;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

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
            if(WSAGetLastError() == WSAEINTR)
                return;
            resultError("accept Failed.");
            return;
        }
        resultAdd("Connected.");
    auto test = TestSet::getTestSets();
    test->newTest();
        DWORD flags = 0;
        if(sharedInfo.buffer == 0){
            sharedInfo.buffer = static_cast<char*>(malloc(buffsize * sizeof(char)));
            sharedInfo.size = buffsize;
            sharedInfo.wsabuff.buf = sharedInfo.buffer;
            sharedInfo.wsabuff.len = buffsize;
        }
        if(WSARecv(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   &flags, &sharedInfo.overlapped, workerRoutineTCP_server)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                closesocket(sharedInfo.sharedSocket);
                resultError("WSARecv Failed.");
                return;
            }
        }
    }
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    resultAdd("Stopping...");
}


void serverUDP(int port, int buffsize){
    resultClear();
    resultAdd("Starting UDP Server.");
    SOCKADDR_IN addr;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

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
    DWORD flags = MSG_PARTIAL;
    if(sharedInfo.buffer == 0){
        sharedInfo.buffer = static_cast<char*>(malloc(buffsize * sizeof(char)));
        sharedInfo.size = buffsize;
        sharedInfo.wsabuff.buf = sharedInfo.buffer;
        sharedInfo.wsabuff.len = buffsize;
    }
    auto test = TestSet::getTestSets();
    test->newTest();
    if(WSARecvFrom(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   &flags, 0, 0, &sharedInfo.overlapped, workerRoutineUDP_server)){
        if(WSAGetLastError() != WSA_IO_PENDING){
            resultError("WSARecv Failed.");
            closesocket(sharedInfo.sharedSocket);
            resultAdd("WSARecvFrom Failed");
            return;
        }
    }
    resultAdd("Waiting for datagram...");
    //windows will not run the callback unless the thread is alertable
    //the thread can only be signaled as alertable with a SleepEx
    //its in a loop so that when a callback is triggered it will again wait
    while(sharedInfo.running)
        if(SleepEx(INFINITE,true) != WAIT_IO_COMPLETION)
            break;//something other than a callback woke us
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
}

void CALLBACK workerRoutineTCP_server(DWORD error, DWORD bytesTrans,
                                      LPWSAOVERLAPPED, DWORD){
    if(error){
        if(error != WSA_OPERATION_ABORTED){
            resultError("Error in WSARecv.");
        }
        resultAdd("Quitting.");
        return;
    }

    if (bytesTrans) {
        auto test = TestSet::getTestSets();
        test->addToTest(1,0,bytesTrans);
        resultAdd("read " + to_string(bytesTrans));
        //resultAdd(string(sharedInfo.buffer, bytesTrans));
        //save it
    } else {
        resultAdd("Connection closed.");
        return;
    }

    if(sharedInfo.running) {
        DWORD flags = 0;
        if(WSARecv(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                   &flags, &sharedInfo.overlapped, workerRoutineTCP_server)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultError("WSARecv Failed.");
                return;
            }
        }
    }
}

void CALLBACK workerRoutineUDP_server(DWORD error, DWORD bytesTrans,
                                      LPWSAOVERLAPPED, DWORD){
    char *temp;
    switch(error){
    case WSAEMSGSIZE:
        resultAdd("The buffer is not large enough!");
        resultAdd("Auto increasing...");
        sharedInfo.size *= 2;
        if((temp = reinterpret_cast<char*>(realloc(sharedInfo.buffer, sharedInfo.size * sizeof(char)))) != 0){
            sharedInfo.buffer = temp;
            sharedInfo.wsabuff.buf = temp;
            sharedInfo.wsabuff.len = sharedInfo.size;
        } else {
            free(sharedInfo.buffer);
            resultAdd("Failed to reallocate buffer, aborting.");
            sharedInfo.buffer = 0;
            closesocket(sharedInfo.sharedSocket);
            return;
        }
        //fall through and process the first part of the message first
    case 0://no error
        if (bytesTrans) {
            auto test = TestSet::getTestSets();
            test->addToTest(1,0,bytesTrans);
            resultAdd("read " + to_string(bytesTrans));
            //resultAdd(string(sharedInfo.buffer, bytesTrans));
            //save it
        } else {
            resultAdd("Nothing to read.");
            return;
        }

        if(sharedInfo.running) {
            DWORD flags = MSG_PARTIAL;
            if(WSARecvFrom(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                           &flags, 0, 0, &sharedInfo.overlapped, workerRoutineUDP_server)){
                if(WSAGetLastError() != WSA_IO_PENDING){
                    resultAdd("WSARecvFrom Failed");
                    return;
                }
            }
        }
        break;
    case WSA_OPERATION_ABORTED:
        //we closed it elsewhere
        break;
    default:
        closesocket(sharedInfo.sharedSocket);
        resultError("Error in WSARecvFrom");
        break;
    }
}

//starts client
void clientTCP(string dest, int  port, int size, int number){
    resultClear();
    resultAdd("Starting TCP Client.");
    SOCKADDR_IN addr;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

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
    auto test = TestSet::getTestSets();
    test->newTest();
    if(sharedInfo.buffer != 0){
        free(sharedInfo.buffer);
    }
    sharedInfo.buffer = static_cast<char*>(malloc(size * sizeof(char)));
    sharedInfo.size = size;
    memset(sharedInfo.buffer,'a', size);
    int total = number;
    while(sharedInfo.running && number--){//keep sending
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

        auto test = TestSet::getTestSets();
        test->addToTest(1, 0, size);
        //increment status bar
        resultSet(double(total - number) / 100 * total);
    }
    if(sharedInfo.running)
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
}

void clientUDP(string dest, int  port, int size, int number){
    resultClear();
    resultAdd("Starting UDP Client.");
    SOCKADDR_IN addr;

    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));

    sharedInfo.running = true;

    if((sharedInfo.sharedSocket = WSASocket(AF_INET,SOCK_DGRAM,0,0,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultError("WSASocket Failed");
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
    auto test = TestSet::getTestSets();
    test->newTest();
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    sharedInfo.buffer = static_cast<char*>(malloc(size * sizeof(char)));
    sharedInfo.size = size;
    memset(sharedInfo.buffer,'a', size);
    sharedInfo.wsabuff.buf = sharedInfo.buffer;
    sharedInfo.wsabuff.len = size;
    int total = number;
    while(sharedInfo.running && number--){//keep sending
        if(WSASendTo(sharedInfo.sharedSocket, &sharedInfo.wsabuff, 1, &sharedInfo.recvd,
                     0, (PSOCKADDR)&addr, sizeof(SOCKADDR_IN), &sharedInfo.overlapped, workerRoutine_client)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultError("WSASendTo Failed");
                return;
            }
        }

        auto test = TestSet::getTestSets();
        test->addToTest(1, 0, size);
        //increment status bar
        int tmp = (double(total - number) / total) * 100;
        resultSet(tmp);
    }
    if(sharedInfo.running)
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
}

void CALLBACK workerRoutine_client(DWORD error, DWORD, LPWSAOVERLAPPED, DWORD){
    if (error) {
        if(sharedInfo.running){
            resultError("Error in WSASend.");
        }
        resultAdd("Quitting.");
        return;
    }
}


void resultError(string msg) {
    resultAdd((msg + " Error Code " + to_string(WSAGetLastError())).c_str());
}

void resultAdd(string msg) {
    QMetaObject qtmo;
    qtmo.invokeMethod(static_cast<MainWindow*>(mainwindowptr),
                      "messageAdd_slot", Q_ARG(std::string, msg));
}

void resultSet(int percent) {
    QMetaObject qtmo;
    qtmo.invokeMethod(static_cast<MainWindow*>(mainwindowptr),
                      "messageSet_slot", Q_ARG(int, percent));
}

void resultClear() {
    QMetaObject qtmo;
    qtmo.invokeMethod(static_cast<MainWindow*>(mainwindowptr), "messageClear_slot");
}
