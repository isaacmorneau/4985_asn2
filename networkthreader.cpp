#include <thread>
#include <QMetaObject>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#include "networkthreader.h"
#include "mainwindow.h"


using namespace std;
//starts server

SOCKET AcceptSocket;

void serverTCP(void* object_ptr, int port){
    resultAdd(object_ptr, "Starting TCP Server.");
    WSADATA wsaData;
    SOCKET ListenSocket;
    SOCKADDR_IN InternetAddr;
    WSAEVENT AcceptEvent;

    if ((WSAStartup(0x0202,&wsaData)) != 0){
        printf("WSAStartup failed with error \n");
        WSACleanup();
        return;
    }
    if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
                                  WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET){
        printf("Failed to get a socket %d\n", WSAGetLastError());
        return;
    }
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(port);

    if (bind(ListenSocket, (PSOCKADDR) &InternetAddr,
             sizeof(InternetAddr)) == SOCKET_ERROR){
        printf("bind() failed with error %d\n", WSAGetLastError());
        return;
    }

    if (listen(ListenSocket, 5)){
        printf("listen() failed with error %d\n", WSAGetLastError());
        return;
    }

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT){
        printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
        return;
    }

    thread worker(workerThread, AcceptEvent);
    worker.detach();
    while(1){
        AcceptSocket = accept(ListenSocket, NULL, NULL);

        if (!WSASetEvent(AcceptEvent)){
            printf("WSASetEvent failed with error %d\n", WSAGetLastError());
            return;
        }
    }
}
void workerThread(WSAEVENT event){
    DWORD Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;

    // Save the accept event in the event array.

    EventArray[0] = event;

    while(1){
        // Wait for accept() to signal an event and also process WorkerRoutine() returns.

        while(1){
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

            if (Index == WSA_WAIT_FAILED) {
                printf("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
                return;
            }

            if (Index != WAIT_IO_COMPLETION){
                // An accept() call event is ready - break the wait loop
                break;
            }
        }

        WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

        // Create a socket information structure to associate with the accepted socket.

        if ((SocketInfo = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
                                                             sizeof(SOCKET_INFORMATION))) == NULL){
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            return;
        }

        // Fill in the details of our accepted socket.

        SocketInfo->Socket = AcceptSocket;
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        Flags = 0;
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                    &(SocketInfo->Overlapped), workerRoutine) == SOCKET_ERROR){
            if (WSAGetLastError() != WSA_IO_PENDING){
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
        printf("Socket %d connected\n", AcceptSocket);
    }
}


void CALLBACK workerRoutine(DWORD Error, DWORD BytesTransferred,
   LPWSAOVERLAPPED Overlapped, DWORD InFlags){
   DWORD SendBytes, RecvBytes;
   DWORD Flags;

   // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
   LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

   if (Error != 0){
     printf("I/O operation failed with error %d\n", Error);
   }

   if (BytesTransferred == 0){
      printf("Closing socket %d\n", SI->Socket);
   }

   if (Error != 0 || BytesTransferred == 0){
      closesocket(SI->Socket);
      GlobalFree(SI);
      return;
   }

   // Check to see if the BytesRECV field equals zero. If this is so, then
   // this means a WSARecv call just completed so update the BytesRECV field
   // with the BytesTransferred value from the completed WSARecv() call.

   if (SI->BytesRECV == 0){
      SI->BytesRECV = BytesTransferred;
      SI->BytesSEND = 0;
   } else {
      SI->BytesSEND += BytesTransferred;
   }

   if (SI->BytesRECV > SI->BytesSEND) {

      // Post another WSASend() request.
      // Since WSASend() is not gauranteed to send all of the bytes requested,
      // continue posting WSASend() calls until all received bytes are sent.

      ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

      SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
      SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

      if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0,
         &(SI->Overlapped), workerRoutine) == SOCKET_ERROR){
         if (WSAGetLastError() != WSA_IO_PENDING){
            printf("WSASend() failed with error %d\n", WSAGetLastError());
            return;
         }
      }
   } else {
      SI->BytesRECV = 0;

      // Now that there are no more bytes to send post another WSARecv() request.

      Flags = 0;
      ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

      SI->DataBuf.len = DATA_BUFSIZE;
      SI->DataBuf.buf = SI->Buffer;

      if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
         &(SI->Overlapped), workerRoutine) == SOCKET_ERROR){
         if (WSAGetLastError() != WSA_IO_PENDING ){
            printf("WSARecv() failed with error %d\n", WSAGetLastError());
            return;
         }
      }
   }
}

void serverUDP(void* object_ptr, int port){
    resultAdd(object_ptr, "Starting UDP Server.");
}

//starts client
void clientTCP(void* object_ptr, string dest, int  port, int size, int number){
    resultAdd(object_ptr, "Starting TCP Client.");
}

void clientUDP(void* object_ptr, string dest, int  port, int size, int number){
    resultAdd(object_ptr, "Starting UDP Client.");
}

void resultAdd(void* object_ptr, string msg) {
    QMetaObject qtmo;
    qtmo.invokeMethod((MainWindow*)object_ptr, "messageAdd_slot", Q_ARG(std::string, msg));
}

void resultClear(void* object_ptr) {
    QMetaObject qtmo;
    qtmo.invokeMethod((MainWindow*)object_ptr, "messageClear_slot");
}
