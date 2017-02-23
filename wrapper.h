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
    * 	void resultAdd(
    * 		std::string msg 	- the string of the message to be printed to the message box
    * 	);
    *
    * Return:
    * 		void
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
    * 	void resultError(
    * 		std::string msg 	- the string of the message to be printed to the message box
    * 								the last WSA error code will be appended to it
    * 	);
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
    * 	void resultSet(
    * 		int percent		- the percent to set the progresss bar to
    * 	);
    *
    * Return:
    * 	void
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
    * 	void
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
    * 	int asyncAccept(
    * 		SOCKET tempsocket, 	- the socket that is is actively listening for connections
    * 		SOCKET *toAccept	- where to save the successful accepted socket
    * 	);
    *
    * Return:
    * 	int 	- 1 on success 0 on failure
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
    * 	int asyncConnect(
    * 		SOCKET *socket, 	- the socket that should be connected
    * 		SOCKADDR *addr		- the address to connect to
    * 	);
    *
    * Return:
    * 	int 	- 1 on success 0 on failure
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
    * 	int asyncSend(
    * 		SOCKET *socket, 		- the socket to send from
    * 		WSABUF *buff, 			- the buffer to read the data out of
    * 		DWORD *recvd, 			- where to store how much was sent
    * 		OVERLAPPED *overlapped	- the overlapped structure for the io
    * 	);
    *
    * Return:
    * 	int 	- 1 on success 0 on failure
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
    * 	int asyncSendTo(
    * 		SOCKET *socket, 		- the socket to send from
    * 		WSABUF *buff, 			- the buffer to read the data out of
    * 		DWORD *recvd, 			- where to store how much was sent
    * 		SOCKADDR *addr,			- the address to send the message to
    * 		OVERLAPPED *overlapped	- the overlapped structure for the io
    * 	);
    *
    * Return:
    * 	int 	- 1 on success 0 on failure
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
    * 	int asyncRecv(
    * 		SOCKET *socket, 		- the socket to read from
    * 		WSABUF *buff, 			- the buffer to write the data into
    * 		DWORD *recvd, 			- where to store how much was read
    * 		OVERLAPPED *overlapped  - the overlapped structure for the io
    * 	);
    *
    * Return:
    * 	int 	- 1 on success 0 on failure
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
    * 	int asyncRecvFrom(
    * 		SOCKET *socket, 		- the socket to read from
    * 		WSABUF *buff, 			- the buffer to write the data into
    * 		DWORD *recvd, 			- where to store how much was read
    * 		OVERLAPPED *overlapped  - the overlapped structure for the io
    * 	);
    *
    * Return:
    * 	int 	- 1 on success 0 on failure
    *
    * Notes: Wraps setup recv events for a udp datagram until there is one to read
    */
int asyncRecvFrom(SOCKET *socket, WSABUF *buff, DWORD *recvd, OVERLAPPED *overlapped);

#endif // WRAPPER_H
