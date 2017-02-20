#ifndef NETWORKINGTHREADER_H
#define NETWORKINGTHREADER_H
#include <winsock2.h>
#include <windows.h>
#include <string>

//starts server
void serverTCP(int port, int buffsize);
void serverUDP(int port, int buffsize);
//starts client
void clientTCP(std::string dest, int  port, int size, int number);
void clientUDP(std::string dest, int  port, int size, int number);

extern bool running;
#endif // NETWORKINGTHREADER_H
