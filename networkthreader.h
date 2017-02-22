#ifndef NETWORKINGTHREADER_H
#define NETWORKINGTHREADER_H
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <fstream>

//starts server
void serverTCP(int port, int buffsize, const std::string &outFile);
void serverUDP(int port, int buffsize, const std::string &outFile);
//starts client
void clientTCP(const std::string &dest, int  port, int size, int number, const std::string &inFile);
void clientUDP(const std::string &dest, int  port, int size, int number, const std::string &inFile);

//callback for async events
void CALLBACK workerRoutineTCP_server(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);
void CALLBACK workerRoutineUDP_server(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);

void CALLBACK workerRoutine_client(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);
//thread for consuming connections
void workerThread_server(WSAEVENT event);

typedef struct _sharedinfo {
    SOCKET sharedSocket;
    char *buffer;
    int size;
    WSABUF wsabuff;
    OVERLAPPED overlapped;
    DWORD recvd;

    bool running;

    bool usingFile;
    std::fstream file;
} sharedinfo;

extern sharedinfo sharedInfo;

#endif // NETWORKINGTHREADER_H
