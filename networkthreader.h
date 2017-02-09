#ifndef NETWORKINGTHREADER_H
#define NETWORKINGTHREADER_H
#include <winsock2.h>
#include <windows.h>
#include <string>

//hack to return info to the gui thread
void resultAdd(std::string msg);
void resultSet(int percent);
void resultClear();
//the shared pointer for the window
extern void *mainwindowptr;

//starts server
void serverTCP(int port, int buffsize);
void serverUDP(int port, int buffsize);
//starts client
void clientTCP(std::string dest, int  port, int size, int number);
void clientUDP(std::string dest, int  port, int size, int number);

//callback for async events
void CALLBACK workerRoutine_server(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);

void CALLBACK workerRoutine_client(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);
//thread for consuming connections
void workerThread_server(WSAEVENT event);

typedef struct _sharedinfo {
    SOCKET sharedSocket;
    char *buffer;
    DWORD recvd;
    WSABUF wsabuff;
    bool running;
    OVERLAPPED overlapped;
} sharedinfo;

extern sharedinfo sharedInfo;

#endif // NETWORKINGTHREADER_H
