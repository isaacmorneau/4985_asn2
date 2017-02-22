#ifndef WRAPPER_H
#define WRAPPER_H
#include <winsock2.h>
#include <windows.h>
#include <string>

    /**
    * Function:resultAdd
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void resultAdd(std::string msg);
    *
    * Return:
    *
    * Notes: sends the content of msg to the ui thread so it can be added to the messages box
    */
void resultAdd(std::string msg);
    /**
    * Function:resultError
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void resultError(std::string msg);
    *
    * Return:
    *
    * Notes: wraps WSAGetLastError into the message then passes it to resultAdd
    */
void resultError(std::string msg);
    /**
    * Function:resultSet
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void resultSet(int percent);
    *
    * Return:
    *
    * Notes: updates the progress bar with the integer of the percent to set it to
    */
void resultSet(int percent);
    /**
    * Function:resultClear
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void resultClear();
    *
    * Return:
    *
    * Notes: clears the messages from the result window
    */
void resultClear();
//the pointer for the window
extern void *mainwindowptr;

    /**
    * Function:asyncAccept
    *
    * Date: 2017/02/20
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	int asyncAccept(SOCKET tempsocket, SOCKET *toAccept);
    *
    * Return:
    *
    * Notes: Wraps the setup of a accept event, wait, and error checking into a function. on success toAccept will be the accepted socket.
    * blocks unti either success or failure of a call occurs
    */
int asyncAccept(SOCKET tempsocket, SOCKET *toAccept);
    /**
    * Function:asyncConnect
    *
    * Date: 2017/02/20
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	int asyncConnect(SOCKET *socket, SOCKADDR *addr);
    *
    * Return:
    *
    * Notes: Wraps the setup of a connection event that will wait until it can complete or the completion times out
    */
int asyncConnect(SOCKET *socket, SOCKADDR *addr);
    /**
    * Function:asyncSend
    *
    * Date: 2017/02/20
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	int asyncSend(SOCKET *socket, WSABUF *buff, DWORD *recvd, OVERLAPPED *overlapped);
    *
    * Return:
    *
    * Notes: Wraps sending on a tcp connection to check for pending IO as well as waiting for a call until it wont block
    * It will block until success or failure
    */
int asyncSend(SOCKET *socket, WSABUF *buff, DWORD *recvd, OVERLAPPED *overlapped);
    /**
    * Function:asyncSendTo
    *
    * Date: 2017/02/20
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	int asyncSendTo(SOCKET *socket, WSABUF *buff, DWORD *recvd,SOCKADDR *addr, OVERLAPPED *overlapped);
    *
    * Return:
    *
    * Notes: Wraps sending on a udp connection to check for pending IO as well as waiting until the call wont block
    * It will block until success or failure
    */
int asyncSendTo(SOCKET *socket, WSABUF *buff, DWORD *recvd,SOCKADDR *addr, OVERLAPPED *overlapped);
    /**
    * Function:asyncRecv
    *
    * Date: 2017/02/20
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	int asyncRecv(SOCKET *socket, WSABUF *buff, DWORD *recvd, OVERLAPPED *overlapped);
    *
    * Return:
    *
    * Notes: Wraps setup recv events for a tcp connection until there is data to be read
    */
int asyncRecv(SOCKET *socket, WSABUF *buff, DWORD *recvd, OVERLAPPED *overlapped);
    /**
    * Function:asyncRecvFrom
    *
    * Date: 2017/02/20
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	int asyncRecvFrom(SOCKET *socket, WSABUF *buff, DWORD *recvd, OVERLAPPED *overlapped);
    *
    * Return:
    *
    * Notes: Wraps setup recv events for a udp datagram until there is one to read
    */
int asyncRecvFrom(SOCKET *socket, WSABUF *buff, DWORD *recvd, OVERLAPPED *overlapped);

#endif // WRAPPER_H
