#ifndef WRAPPER_H
#define WRAPPER_H
#include <winsock2.h>
#include <windows.h>
#include <string>

//hack to return info to the gui thread
void resultAdd(std::string msg);
//wapper for resultAdd that adds WSAGetLastError code to message
void resultError(std::string msg);
//set progress bars
void resultSet(int percent);
//clear output window
void resultClear();
//the pointer for the window
extern void *mainwindowptr;

int asyncAccept(SOCKET tempsocket, SOCKET *toAccept);
int asyncConnect(SOCKET *socket, SOCKADDR *addr);

#endif // WRAPPER_H
