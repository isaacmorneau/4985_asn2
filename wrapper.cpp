#include "wrapper.h"
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
    if(WSAEventSelect(tempsocket, *events, FD_ACCEPT) == SOCKET_ERROR){
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
    if(WSAWaitForMultipleEvents(1, events, 0, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
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
    if(WSAEventSelect(*socket, *events, FD_CONNECT) == SOCKET_ERROR){
        resultError("WSAEventSelect failed.");
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
        return 0;
    }
    //wait for it to not block anymore
    if(WSAWaitForMultipleEvents(1, events, 0, WSA_INFINITE, 1) == WSA_WAIT_FAILED){
        resultError("WSAWaitForMultipleEvents failed.");
        return 0;
    }
    //cool its done
    return 1;
}
