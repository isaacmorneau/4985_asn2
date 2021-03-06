#include "wrapper.h"
#include "networkthreader.h"
#include "mainwindow.h"
#include <winsock2.h>
#include <windows.h>
#include <QMetaObject>
#include <string>

void *mainwindowptr;

using namespace std;

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


int asyncAccept(SOCKET tempsocket, SOCKET *toAccept){
    WSAEVENT events[1];
    events[0] = WSACreateEvent();
    //register event
    if(WSAEventSelect(tempsocket, *events, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR){
        resultError("WSAEventSelect failed.");
        return 0;
    }
    if((*toAccept = WSAAccept(tempsocket,0,0,0,0)) != INVALID_SOCKET)
        return 1;
    if(WSAGetLastError() != WSAEWOULDBLOCK){
        resultError("WSAAccept failed.");
        return 0;
    }
    //wait for event to trigger
    if(WSAWaitForMultipleEvents(1, events, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        return 0;
    }
    //complete event
    if ((*toAccept = accept(tempsocket, 0, 0)) == INVALID_SOCKET)
    {
        resultError("accept failed.");
        return 0;
    }
    return 1;
}

int asyncConnect(SOCKET *socket, SOCKADDR *addr){
    WSAEVENT events[1];
    events[0] = WSACreateEvent();
    //register event
    if(WSAEventSelect(*socket, *events, FD_CONNECT | FD_CLOSE) == SOCKET_ERROR){
        resultError("WSAEventSelect failed.");
        WSACloseEvent(events[0]);
        return 0;
    }
    //check for imediate completion
    if(!WSAConnect(*socket, addr, sizeof(SOCKADDR_IN),0,0,0,0)){
        //worked first try, we're good to go
        return 1;
    }
    //didnt work, why?
    if(WSAGetLastError() != WSAEWOULDBLOCK){
        resultError("WSAConnect failed.");
        WSACloseEvent(events[0]);
        return 0;
    }
    //wait for it to not block anymore
    if(WSAWaitForMultipleEvents(1, events, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        WSACloseEvent(events[0]);
        return 0;
    }
    return 1;
}


int asyncSend(SOCKET *socket, WSABUF *buff, DWORD *recvd){
    memset(&sharedInfo.overlap,0,sizeof(OVERLAPPED));
    sharedInfo.overlap.hEvent = WSACreateEvent();

    WSAEVENT events[1];
    events[0] = WSACreateEvent();
    //register event
    if(WSAEventSelect(*socket, *events, FD_WRITE | FD_CLOSE) == SOCKET_ERROR){
        resultError("WSAEventSelect failed.");
        WSACloseEvent(events[0]);
        return 0;
    }
    //wait for it to not block anymore
    if(WSAWaitForMultipleEvents(1, events, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        WSACloseEvent(events[0]);
        return 0;
    }
    //check for imediate completion
    if(!WSASend(*socket, buff, 1, recvd, 0, &sharedInfo.overlap, workerRoutine_client)){
        //worked first try
        return 1;
    }
    //wait for it to finish
    if(WSAWaitForMultipleEvents(1, &sharedInfo.overlap.hEvent, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        WSACloseEvent(sharedInfo.overlap.hEvent);
        return 0;
    }
    return 1;
}


int asyncSendTo(SOCKET *socket, WSABUF *buff, DWORD *recvd,SOCKADDR *addr){
    memset(&sharedInfo.overlap,0,sizeof(OVERLAPPED));
    sharedInfo.overlap.hEvent = WSACreateEvent();

    //check for imediate completion
    if(!WSASendTo(*socket, buff, 1, recvd, 0, addr, sizeof(SOCKADDR_IN), &sharedInfo.overlap,
                  workerRoutine_client)){
        //worked first try
        return 1;
    }
    if(WSAWaitForMultipleEvents(1, &sharedInfo.overlap.hEvent, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        WSACloseEvent(sharedInfo.overlap.hEvent);
        return 0;
    }
    return 1;
}


int asyncRecv(SOCKET *socket, WSABUF *buff, DWORD *recvd){
    memset(&sharedInfo.overlap,0,sizeof(OVERLAPPED));
    sharedInfo.overlap.hEvent = WSACreateEvent();

    WSAEVENT events[1];
    events[0] = WSACreateEvent();
    //register event
    if(WSAEventSelect(*socket, *events, FD_READ | FD_CLOSE) == SOCKET_ERROR){
        resultError("WSAEventSelect failed.");
        WSACloseEvent(events[0]);
        return 0;
    }
    //wait for it to not block anymore
    if(WSAWaitForMultipleEvents(1, events, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        WSACloseEvent(events[0]);
        return 0;
    }
    //check for imediate completion
    DWORD flags = 0;
    if(!WSARecv(*socket, buff, 1, recvd, &flags, &sharedInfo.overlap, workerRoutineTCP_server)){
        //worked first try
        return 1;
    }
    //wait for it to finish
    if(WSAWaitForMultipleEvents(1, &sharedInfo.overlap.hEvent, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        WSACloseEvent(sharedInfo.overlap.hEvent);
        return 0;
    }
    return 1;
}


int asyncRecvFrom(SOCKET *socket, WSABUF *buff, DWORD *recvd){
    memset(&sharedInfo.overlap,0,sizeof(OVERLAPPED));
    sharedInfo.overlap.hEvent = WSACreateEvent();
    //wait for any reads in progress to finish
    DWORD tra=0,flg=0;
    if(WSAGetOverlappedResult(*socket,&sharedInfo.overlap,&tra,0,&flg) == 0){
        if(WSAGetLastError() == WSA_IO_INCOMPLETE){
            if(WSAWaitForMultipleEvents(1, &sharedInfo.overlap.hEvent, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
                resultError("WSAWaitForMultipleEvents failed.");
                WSACloseEvent(sharedInfo.overlap.hEvent);
                return 0;
            }
        }
    }
    //check for imediate completion
    DWORD flags = MSG_PARTIAL;
    if(WSARecvFrom(*socket, buff, 1, recvd, &flags, 0, 0, &sharedInfo.overlap, workerRoutineUDP_server)){
        //worked first try
        return 1;
    }
    //wait for it to finish
    if(WSAWaitForMultipleEvents(1, &sharedInfo.overlap.hEvent, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        WSACloseEvent(sharedInfo.overlap.hEvent);
        return 0;
    }
    return 1;
}
