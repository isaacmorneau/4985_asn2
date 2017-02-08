#ifndef NETWORKINGTHREADER_H
#define NETWORKINGTHREADER_H
#include <winsock2.h>
#include <windows.h>
#include <string>

#define DATA_BUFSIZE 2048

//hack to return info to the gui thread
void resultAdd(std::string msg);
void resultClear();
//the shared pointer for the window
extern void *mainwindowptr;

//starts server
void serverTCP(int port);
void serverUDP(int port);
//starts client
void clientTCP(std::string dest, int  port, int size, int number);
void clientUDP(std::string dest, int  port, int size, int number);

//callback for async events
void CALLBACK workerRoutine_TCPserver(DWORD Error, DWORD BytesTransferred,
   LPWSAOVERLAPPED Overlapped, DWORD InFlags);

void CALLBACK workerRoutine_UDPserver(DWORD Error, DWORD BytesTransferred,
   LPWSAOVERLAPPED Overlapped, DWORD InFlags);
//thread for consuming connections
void workerThread_TCPserver(WSAEVENT event);

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
} SOCKET_INFO, *LPSOCKET_INFO;

#endif // NETWORKINGTHREADER_H
