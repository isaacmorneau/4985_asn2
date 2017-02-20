#include <thread>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <sstream>

#include "networkthreader.h"
#include "wrappers.h"


using namespace std;
//starts server

void serverTCP(int port, int buffsize){
    resultClear();
    WSAWrap wrap(TCP, buffsize);

    resultAdd("Starting TCP Server.");
    wrap.setupServer(port);

    //add checkbox for whether or not to loop for new connections
    while(WSAWrap::running()){//accept connections one after another
        resultAdd("Waiting for connection...");
        wrap.waitForConn();
        if(!wrap.acceptConn())
            break;
        resultAdd("Connected.");
        if(!wrap.recv())
            break;
    }
    resultAdd("Stopping...");
}


void serverUDP(int port, int buffsize){
    resultClear();
    resultAdd("Starting UDP Server.");
    WSAWrap wrap(UDP, buffsize);

    wrap.setupServer(port);

    //add checkbox for whether or not to loop for new connections

    //windows will not run the callback unless the thread is alertable
    //the thread can only be signaled as alertable with a SleepEx
    //its in a loop so that when a callback is triggered it will again wait
    resultAdd("Waiting for datagrams...");
    if(!wrap.recv())
        return;
    while(WSAWrap::running())
        if(SleepEx(INFINITE,true) != WAIT_IO_COMPLETION)
            break;//something other than a callback woke us
}



//starts client
void clientTCP(string dest, int  port, int size, int number){
    resultClear();
    resultAdd("Starting TCP Client.");
    WSAWrap wrap(TCP, size);

    wrap.setupClient(dest, port);
    resultAdd("Trying to connect...");
    if(!wrap.conn())
        return;
    resultAdd("Connected.");

    resultAdd("Sending...");
    int total = number;
    while(WSAWrap::running() && number--){//keep sending
        if(!wrap.send())
            break;
        //increment status bar
        resultSet(double(total - number) / total * 100);
    }
    if(WSAWrap::running())
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
}

void clientUDP(string dest, int  port, int size, int number){
    resultClear();
    resultAdd("Starting UDP Client.");
    WSAWrap wrap(UDP, size);

    wrap.setupClient(dest, port);

    resultAdd("Sending...");
    int total = number;
    while(WSAWrap::running() && number--){//keep sending
        if(!wrap.send())
            break;
        //increment status bar
        resultSet(double(total - number) / 100 * total);
    }
    if(WSAWrap::running())
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
}
