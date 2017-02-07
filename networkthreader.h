#ifndef NETWORKINGTHREADER_H
#define NETWORKINGTHREADER_H
#include <winsock2.h>
#include <windows.h>
#include <string>

#define DATA_BUFSIZE 2048

//hack to return info to the gui thread
void resultAdd(void* object_ptr, std::string msg);
void resultClear(void* object_ptr);
//starts server
void serverTCP(void* object_ptr, int port);
void serverUDP(void* object_ptr, int port);
//starts client
void clientTCP(void* object_ptr, std::string dest, int  port, int size, int number);
void clientUDP(void* object_ptr, std::string dest, int  port, int size, int number);

//callback for async events
void CALLBACK workerRoutine(DWORD Error, DWORD BytesTransferred,
   LPWSAOVERLAPPED Overlapped, DWORD InFlags);
//thread for consuming connections
void workerThread(WSAEVENT event);


typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;
#endif // NETWORKINGTHREADER_H
