#include "networkthreader.h"
#include "wrapper.h"
#include "mainwindow.h"
#include "testset.h"
#include <thread>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <fstream>

using namespace std;
//starts server

sharedinfo sharedInfo{};

void serverTCP(int port, int buffsize, const string &outFile){
    //initialize the data
    resultClear();
    resultAdd("Starting TCP Server.");
    SOCKADDR_IN addr;
    SOCKET tempSock;
    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));
    sharedInfo.running = true;
    if((sharedInfo.usingFile = outFile.size() > 0)){
        sharedInfo.file = fstream(outFile.c_str(),ios_base::out | ios_base::binary);
        if(!sharedInfo.file.is_open()){
            resultAdd("Unable to upen file");
            return;
        }
    }

    //create the socket for listening
    if((tempSock = WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        return;
    }
    //bind to it
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if(bind(tempSock, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultError("bind Failed.");
        return;
    }
    //start listening
    if(listen(tempSock, 5)){
        resultError("listen Failed.");
        return;
    }

    //create a request for an async accept and wait for it to complete
    resultAdd("Waiting for connection...");
    if(!asyncAccept(tempSock, &sharedInfo.sharedSocket))
        return;
    resultAdd("Connected.");

    auto test = TestSet::getTestSets();
    test->newTest("TCP");

    if(sharedInfo.buffer == 0){
        sharedInfo.buffer = static_cast<char*>(malloc(buffsize * sizeof(char)));
        sharedInfo.size = buffsize;
        sharedInfo.wsabuff.buf = sharedInfo.buffer;
        sharedInfo.wsabuff.len = buffsize;
    }
    //register the completion routine
    if(!asyncRecv(&sharedInfo.sharedSocket, &sharedInfo.wsabuff, &sharedInfo.recvd, &sharedInfo.overlapped))
        return;

    //windows will not run the callback unless the thread is alertable
    //the thread can only be signaled as alertable with a SleepEx
    //its in a loop so that when a callback is triggered it will again wait
    while(sharedInfo.running)
        if(SleepEx(INFINITE,true) != WAIT_IO_COMPLETION)
            break;//something other than a callback woke us
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    if(sharedInfo.usingFile)
        sharedInfo.file.close();
    closesocket(tempSock);
    resultAdd("Stopping...");
}


void serverUDP(int port, int buffsize, const string &outFile){
    //initialize the data
    resultClear();
    resultAdd("Starting UDP Server.");
    SOCKADDR_IN addr;
    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));
    sharedInfo.running = true;
    if((sharedInfo.usingFile = outFile.size() > 0)){
        sharedInfo.file = fstream(outFile.c_str(),ios_base::out | ios_base::binary);
        if(!sharedInfo.file.is_open()){
            resultAdd("Unable to upen file");
            return;
        }
    }

    //create the socket
    if((sharedInfo.sharedSocket = WSASocket(AF_INET,SOCK_DGRAM,0,0,0,WSA_FLAG_OVERLAPPED))
            == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    //bind to the address
    if(bind(sharedInfo.sharedSocket, (PSOCKADDR)&addr,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
        resultError("bind Failed.");
        return;
    }

    if(sharedInfo.buffer == 0){
        sharedInfo.buffer = static_cast<char*>(malloc(buffsize * sizeof(char)));
        sharedInfo.size = buffsize;
        sharedInfo.wsabuff.buf = sharedInfo.buffer;
        sharedInfo.wsabuff.len = buffsize;
    }
    auto test = TestSet::getTestSets();
    test->newTest("UDP");
    //register the completion routine
    if(!asyncRecvFrom(&sharedInfo.sharedSocket, &sharedInfo.wsabuff,
                     &sharedInfo.recvd,&sharedInfo.overlapped))
            return;
    resultAdd("Waiting for datagram...");

    //windows will not run the callback unless the thread is alertable
    //the thread can only be signaled as alertable with a SleepEx
    //its in a loop so that when a callback is triggered it will again wait
    while(sharedInfo.running)
        if(SleepEx(INFINITE,true) != WAIT_IO_COMPLETION)
            break;//something other than a callback woke us
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    if(sharedInfo.usingFile)
        sharedInfo.file.close();
    resultAdd("Stopping...");
}

void CALLBACK workerRoutineTCP_server(DWORD error, DWORD bytesTrans,
                                      LPWSAOVERLAPPED, DWORD){
    if(error){
        if(error != WSA_OPERATION_ABORTED){
            resultError("Error in WSARecv.");
        }
        resultAdd("Quitting.");
        return;
    }

    //save it
    if (bytesTrans) {
        auto test = TestSet::getTestSets();
        test->addToTest(1,0,bytesTrans);
        resultAdd("read " + to_string(bytesTrans));
        if(sharedInfo.usingFile){
            sharedInfo.file.write(sharedInfo.buffer,bytesTrans);
            if(sharedInfo.file.bad()){
                resultAdd("Failed to write to file.");
                return;
            }
        }
    } else {
        resultAdd("Connection closed.");
        return;
    }

    if(sharedInfo.running)
        if(!asyncRecv(&sharedInfo.sharedSocket, &sharedInfo.wsabuff,
                     &sharedInfo.recvd, &sharedInfo.overlapped))
            return;
}

void CALLBACK workerRoutineUDP_server(DWORD error, DWORD bytesTrans,
                                      LPWSAOVERLAPPED, DWORD){
    char *temp;
    switch(error){
    case WSAEMSGSIZE:
        resultAdd("The buffer is not large enough!");
        resultAdd("Auto doubling...");
        sharedInfo.size *= 2;
        if((temp = reinterpret_cast<char*>(realloc(sharedInfo.buffer, sharedInfo.size * sizeof(char)))) != 0){
            sharedInfo.buffer = temp;
            sharedInfo.wsabuff.buf = temp;
            sharedInfo.wsabuff.len = sharedInfo.size;
        } else {
            free(sharedInfo.buffer);
            resultAdd("Failed to reallocate buffer, aborting.");
            sharedInfo.buffer = 0;
            closesocket(sharedInfo.sharedSocket);
            return;
        }
        //fall through and process the first part of the message first
    case 0://no error
        if (bytesTrans) {
            auto test = TestSet::getTestSets();
            test->addToTest(1,0,bytesTrans);
            resultAdd("read " + to_string(bytesTrans));
            //save it
            if(sharedInfo.usingFile){
                sharedInfo.file.write(sharedInfo.buffer,bytesTrans);
                if(sharedInfo.file.bad()){
                    resultAdd("Failed to write to file.");
                    return;
                }
            }
        } else {
            resultAdd("Nothing to read.");
            return;
        }

        //only reregister if its still running
        if(sharedInfo.running)
            if(!asyncRecvFrom(&sharedInfo.sharedSocket, &sharedInfo.wsabuff,
                              &sharedInfo.recvd, &sharedInfo.overlapped))
                    return;
        break;
    case WSA_OPERATION_ABORTED:
        //we closed it elsewhere
        break;
    default:
        closesocket(sharedInfo.sharedSocket);
        resultError("Error in WSARecvFrom");
        break;
    }
}

//starts client
void clientTCP(const string &dest, int  port, int size, int number, const string &inFile){
    //initialize the data
    resultClear();
    resultAdd("Starting TCP Client.");
    SOCKADDR_IN addr;
    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));
    sharedInfo.running = true;
    if((sharedInfo.usingFile = inFile.size() > 0)){
        sharedInfo.file = fstream(inFile.c_str(),ios_base::in | ios_base::binary);
        if(!sharedInfo.file.is_open()){
            resultAdd("Unable to upen file");
            return;
        }
        number = 1;
    }

    //create the socket
    if((sharedInfo.sharedSocket = WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED))
            == INVALID_SOCKET){
        resultError("WSASocket Failed.");
        return;
    }

    //get the ip
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    hostent *ent;
    if(!(ent = gethostbyname(dest.c_str()))){
        resultError("could not find hostname.");
        return;
    }
    memcpy((char *)&addr.sin_addr, ent->h_addr, ent->h_length);

    //connect with async and a wait
    resultAdd("Trying to connect...");
    if(!asyncConnect(&sharedInfo.sharedSocket,(PSOCKADDR)&addr))
        return;
    resultAdd("Connected.");

    resultAdd("Sending...");
    auto test = TestSet::getTestSets();
    test->newTest("TCP");

    if(sharedInfo.buffer != 0){
        free(sharedInfo.buffer);
    }
    sharedInfo.buffer = static_cast<char*>(malloc(size * sizeof(char)));
    sharedInfo.size = size;
    if(!sharedInfo.usingFile)
        memset(sharedInfo.buffer,'a', size);
    sharedInfo.wsabuff.buf = sharedInfo.buffer;
    sharedInfo.wsabuff.len = size;
    int total = number;
    while(sharedInfo.running && number){//keep sending
        if(sharedInfo.usingFile){
            sharedInfo.file.read(sharedInfo.buffer,sharedInfo.size);
            if (!(sharedInfo.wsabuff.len = sharedInfo.file.gcount())){
                break;
            }
        } else {
            --number;
        }
        //register sender completion routine
        if(!asyncSend(&sharedInfo.sharedSocket, &sharedInfo.wsabuff, &sharedInfo.recvd, &sharedInfo.overlapped))
            return;

        auto test = TestSet::getTestSets();
        test->addToTest(1, 0, size);
        //increment status bar
        resultSet((double(total - number) / total) * 100);
    }
    if(sharedInfo.running)
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    if(sharedInfo.usingFile)
        sharedInfo.file.close();
}

void clientUDP(const string &dest, int  port, int size, int number, const string &inFile){
    //initialize
    resultClear();
    resultAdd("Starting UDP Client.");
    SOCKADDR_IN addr;
    memset(&sharedInfo.overlapped,0,sizeof(OVERLAPPED));
    sharedInfo.running = true;
    if((sharedInfo.usingFile = inFile.size() > 0)){
        sharedInfo.file = fstream(inFile.c_str(),ios_base::in | ios_base::binary);
        if(!sharedInfo.file.is_open()){
            resultAdd("Unable to upen file");
            return;
        }
        number = 1;
    }
    //create the socket
    if((sharedInfo.sharedSocket = WSASocket(AF_INET,SOCK_DGRAM,0,0,0,WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        resultError("WSASocket Failed");
        return;
    }

    //get the ip
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    hostent *ent;
    if(!(ent = gethostbyname(dest.c_str()))){
        resultAdd("could not find hostname.");
        return;
    }
    memcpy((char *)&addr.sin_addr, ent->h_addr, ent->h_length);

    resultAdd("Sending...");
    auto test = TestSet::getTestSets();
    test->newTest("UDP");

    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    sharedInfo.buffer = static_cast<char*>(malloc(size * sizeof(char)));
    sharedInfo.size = size;
    if(!sharedInfo.usingFile)
        memset(sharedInfo.buffer,'a', size);
    sharedInfo.wsabuff.buf = sharedInfo.buffer;
    sharedInfo.wsabuff.len = size;
    int total = number;
    while(sharedInfo.running && number){//keep sending
        if(sharedInfo.usingFile){
            sharedInfo.file.read(sharedInfo.buffer,sharedInfo.size);
            if (!(sharedInfo.wsabuff.len = sharedInfo.file.gcount())){
                break;
            }
        } else {
            --number;
        }
        //register the sender completion routine
        if(!asyncSendTo(&sharedInfo.sharedSocket, &sharedInfo.wsabuff, &sharedInfo.recvd,
                        (PSOCKADDR)&addr, &sharedInfo.overlapped))
                return;

        auto test = TestSet::getTestSets();
        test->addToTest(1, 0, size);
        //increment status bar
        resultSet((double(total - number) / total) * 100);
    }
    if(sharedInfo.running)
        resultAdd("Finished.");
    else
        resultAdd("Stopped.");
    if(sharedInfo.usingFile)
        sharedInfo.file.close();
}

void CALLBACK workerRoutine_client(DWORD error, DWORD, LPWSAOVERLAPPED, DWORD){
    if (error) {
        if(sharedInfo.running){
            resultError("Error in WSASend.");
        }
        resultAdd("Quitting.");
        return;
    }
}
