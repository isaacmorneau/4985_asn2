#ifndef WRAPPERS_H
#define WRAPPERS_H
#include <winsock2.h>
#include <string>
#include <QMetaObject>
#include "mainwindow.h"

//hack to return info to the gui thread
void resultAdd(std::string msg);
//wapper for resultAdd that adds WSAGetLastError code to message
void resultError(std::string msg);
//set progress bars
void resultSet(int percent);
//clear output window
void resultClear();
//the shared pointer for the window
extern void *mainwindowptr;

//follows RAII model for WSA startup
enum WrapType{
    TCP,UDP
};
#define DEFAULTBUFFER 1024

class WSAWrap{
public:
    WSAWrap(WrapType type, int bufferSize = DEFAULTBUFFER);
    ~WSAWrap();

    void setupServer(int port);
    void waitForConn();
    bool acceptConn();
    bool recv();

    void setupClient(const std::string &dest, int port);
    bool conn();
    bool send();

    //for external termination
    static bool running(){return WSAWrap::run;}
    static void stop(){WSAWrap::run = false;}

    //callback for async events
    static void CALLBACK workerRoutineTCP_server(DWORD error, DWORD bytesTrans,
                                                 LPWSAOVERLAPPED overlapped, DWORD inFlags);
    static void CALLBACK workerRoutineUDP_server(DWORD error, DWORD bytesTrans,
                                                 LPWSAOVERLAPPED overlapped, DWORD inFlags);
    static void CALLBACK workerRoutine_client(DWORD error, DWORD bytesTrans,
                                              LPWSAOVERLAPPED overlapped, DWORD inFlags);
    //so that static memebers can function on the most recent wrap instance
    static WSAWrap *recentWrap;

protected:
    static bool run;
    WrapType type;
    SOCKET socket;
    SOCKET socketConn;
    char *buffer;
    int size;
    OVERLAPPED overlapped;
    WSADATA wsaData;

    SOCKADDR_IN addr;
    WSABUF wsabuff;
};

#endif // WRAPPERS_H
