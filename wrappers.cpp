#include "wrappers.h"
#include <string>
#include <winsock2.h>

bool WSAWrap::run;
WSAWrap *WSAWrap::recentWrap = 0;

WSAWrap::WSAWrap(WrapType type, int bufferSize):
    type(type),
    socket(INVALID_SOCKET),//cant be initialized till after WSAStartup
    socketConn(INVALID_SOCKET),
    buffer(0),
    size(bufferSize),
    overlapped(),
    wsaData(){

    if(WSAStartup(0x0202,&wsaData)){
        resultError("WSAStartup Failed.");
        WSAWrap::stop();
    }

    if((socket = WSASocket(AF_INET, type == UDP ? SOCK_DGRAM : SOCK_STREAM,
                           0, 0, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        WSAWrap::stop();
    }
    buffer = static_cast<char*>(calloc(bufferSize, sizeof(char)));
    wsabuff.buf = buffer;
    wsabuff.len = size;


    memset(&overlapped,0,sizeof(OVERLAPPED));
    //for this instance set recentWrap to ourselves for use in static callbacks
    WSAWrap::recentWrap = this;
    WSAWrap::run = true;
}


WSAWrap::~WSAWrap(){
    WSAWrap::run = false;
    if(buffer!=0)
        free(buffer);
    if(closesocket(socket))
        resultError("closesocket primary failed.");
    if(socketConn != INVALID_SOCKET)
        if(closesocket(socketConn))
            resultError("closesocket conn failed.");
    if(WSACleanup()){
        resultError("WSACleanup Failed.");
    }
}

void WSAWrap::setupServer(int port){
    if(!WSAWrap::running())
        return;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if(bind(socket, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultError("bind Failed.");
        return;
    }

}

void WSAWrap::waitForConn(){
    if(!WSAWrap::running())
        return;
    if(type == UDP)
        return;
    if(listen(socket, 5)){
        resultError("listen Failed.");
        return;
    }
    //do async wait here
}

bool WSAWrap::acceptConn(){
    if(!WSAWrap::running())
        return false;
        if((socketConn = accept(socket,0,0)) == INVALID_SOCKET){
            if(WSAGetLastError() != WSAEINTR)
                resultError("accept Failed.");
            return false;
        }
        return true;
}

bool WSAWrap::recv(){
    if(!WSAWrap::running())
        return false;
    DWORD flags;
    switch(type){
    case TCP:
        flags = 0;
        if(WSARecv(socketConn, &wsabuff, 1, 0, &flags, &overlapped, WSAWrap::workerRoutineTCP_server)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                WSAWrap::stop();
                resultError("WSARecv Failed.");
                return false;
            }
        }
        break;
    case UDP:
        flags = MSG_PARTIAL;
        if(WSARecvFrom(socket, &wsabuff, 1, 0, &flags, 0, 0, &overlapped, WSAWrap::workerRoutineUDP_server)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                WSAWrap::stop();
                resultError("WSARecvFrom Failed");
                return false;
            }
        }
        break;
    }
    return (SleepEx(INFINITE,true) == WAIT_IO_COMPLETION);
}

void CALLBACK WSAWrap::workerRoutineTCP_server(DWORD error, DWORD bytesTrans,
                                               LPWSAOVERLAPPED, DWORD){
    if(!WSAWrap::running())
        return;
    auto wrap = *WSAWrap::recentWrap;
    if(error){
        if(error != WSA_OPERATION_ABORTED){
            resultError("Error in WSARecv.");
        }
        resultAdd("Quitting.");
        return;
    }

    if (bytesTrans) {
        resultAdd("read " + std::to_string(bytesTrans));
        //resultAdd(string(sharedInfo.buffer, bytesTrans));
        //save it
    } else {
        resultAdd("Nothing to read.");
    }
    if(WSARecv(wrap.socketConn, &wrap.wsabuff, 1, 0, 0, &wrap.overlapped,
               WSAWrap::workerRoutineTCP_server)){
        if(WSAGetLastError() != WSA_IO_PENDING){
            resultError("WSARecv Failed.");
            return;
        }
    }
}


void CALLBACK WSAWrap::workerRoutineUDP_server(DWORD error, DWORD bytesTrans,
                                               LPWSAOVERLAPPED, DWORD){
    if(!WSAWrap::running())
        return;
    auto wrap = *WSAWrap::recentWrap;
    char *temp;
    DWORD flags = MSG_PARTIAL;

    switch(error){
    case WSAEMSGSIZE:
        resultAdd("The buffer is not large enough!");
        resultAdd("Auto increasing...");
        wrap.size *= 2;
        if((temp = reinterpret_cast<char*>(realloc(wrap.buffer, wrap.size * sizeof(char)))) != 0){
            wrap.buffer = temp;
            wrap.wsabuff.buf = temp;
            wrap.wsabuff.len = wrap.size;
        } else {
            resultAdd("Failed to reallocate buffer, aborting.");
            return;
        }
        //fall through and process the first part of the message first
    case 0://no error
        if (bytesTrans) {
            resultAdd("read " + std::to_string(bytesTrans));
            //resultAdd(string(sharedInfo.buffer, bytesTrans));
            //save it
        } else {
            resultAdd("Nothing to read.");
        }

        if(WSARecvFrom(wrap.socket, &wrap.wsabuff, 1, 0, &flags, 0, 0,
                       &wrap.overlapped, WSAWrap::workerRoutineUDP_server)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultError("WSARecvFrom Failed");
                return;
            }
        }
        break;
    case WSA_OPERATION_ABORTED:
        //we closed it elsewhere, thats fine
        break;
    default:
        resultError("Error in WSARecvFrom");
        break;
    }
}

void WSAWrap::setupClient(const std::string &dest, int port){
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    hostent *ent;
    if(!(ent = gethostbyname(dest.c_str()))){
        resultError("could not find hostname.");
        return;
    }

    memcpy((char *)&addr.sin_addr, ent->h_addr_list[0], ent->h_length);

    memset(buffer,'a', size);
}

bool WSAWrap::conn(){
    if(type==UDP)
        return true;
    if(connect(socket, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultError("connect Failed.");
        return false;
    }
    return true;
}

bool WSAWrap::send(){
    switch(type){
    case TCP:
        if(WSASend(socket, &wsabuff, 1, 0, 0, &overlapped, WSAWrap::workerRoutine_client)){
            if(WSAGetLastError() != WSA_IO_PENDING){
                resultError("WSASend Failed.");
                return false;
            }
            return (SleepEx(INFINITE,true) == WAIT_IO_COMPLETION);
        }
        break;
    case UDP:
        if(WSASendTo(socket, &wsabuff, 1, 0, 0, (PSOCKADDR)&addr,
                     sizeof(SOCKADDR_IN), &overlapped, WSAWrap::workerRoutine_client)){
            int err = WSAGetLastError();
            if(err != WSA_IO_PENDING && err != ERROR_SUCCESS){
                resultError("WSASendTo Failed");
                return false;
            }
        }
        break;
    }
    return true;
}


void CALLBACK WSAWrap::workerRoutine_client(DWORD error, DWORD, LPWSAOVERLAPPED, DWORD){
    if(!WSAWrap::running())
        return;
    if (error && error != WSA_OPERATION_ABORTED) {
        resultError("Error in WSASend.");
        return;
    }
}

void *mainwindowptr;

//thread to ui interfaces
void resultError(std::string msg) {
    int last = WSAGetLastError();
    resultAdd((msg + " Error Code " + std::to_string(last)).c_str());
}

void resultAdd(std::string msg) {
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
    qtmo.invokeMethod(static_cast<MainWindow*>(mainwindowptr),
                      "messageClear_slot");
}
