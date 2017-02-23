#ifndef NETWORKINGTHREADER_H
#define NETWORKINGTHREADER_H
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <fstream>

//starts server
    /**
    * Function:serverTCP
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void serverTCP(
    * 		int port,					- the port to listen to
    * 		int buffsize, 				- how large of a buffer to read into
    * 		const std::string &outFile	- [optional] the file to save data to
    * 	);
    *
    * Return:
    * 	void
    *
    * Notes: the function that is run in the thread for server TCP sessions taking in all data required to setup
    * thread and allow the ui to stay responsive. it registers asyncRecv.
    */
void serverTCP(int port, int buffsize, const std::string &outFile);
    /**
    * Function:serverUDP
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void serverUDP(
    * 		int port, 					- the port to listen to
    * 		int buffsize, 				- how large of a buffer to read into initially, can resize internally
    * 		const std::string &outFile	- [optional] the file to save data to
    * 	);
    *
    * Return:
    * 	void
    *
    * Notes: the function that is run in the thread for server UDP sessions taking in all data required to setup
    * thread and allow the ui to stay responsive. it registers asyncRecvFrom.
    */
void serverUDP(int port, int buffsize, const std::string &outFile);
//starts client
    /**
    * Function:clientTCP
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void clientTCP(
    * 		const std::string &dest, 	- the ip or hostname of who to connect to
    * 		int  port, 					- the port to connect to
    * 		int size, 					- the size of data to send at once
    * 		int number, 				- the number of packets to send
    * 		const std::string &inFile	- [optional] the file to send
    * 	);
    *
    * Return:
    * 	void
    *
    * Notes: the function that is run in the thread for client TCP sessions taking in all data required to setup
    * thread and allow the ui to stay responsive. it registers asyncSend.
    */
void clientTCP(const std::string &dest, int  port, int size, int number, const std::string &inFile);
    /**
    * Function:clientUDP
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void clientUDP(
    * 		const std::string &dest, 	- the ip or hostname of who to connect to
    * 		int  port, 					- the port to connect to
    * 		int size, 					- the size of data to send at once
    * 		int number, 				- the number of packets to send
    * 		const std::string &inFile	- [optional] the file to send
    * 	);
    *
    * Return:
    * 	void
    *
    * Notes: the function that is run in the thread for client UDP sessions taking in all data required to setup
    * thread and allow the ui to stay responsive. it registers asyncSendTo.
    */
void clientUDP(const std::string &dest, int  port, int size, int number, const std::string &inFile);

    /**
    * Function:workerRoutineTCP_server
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void CALLBACK workerRoutineTCP_server(
    * 		DWORD error,				- the error that occured if not zero
    * 		DWORD bytesTrans,			- the bytes read
    * 	   	LPWSAOVERLAPPED overlapped,	- the overlapped structure used
    * 		DWORD inFlags				- flags set by the caller
    * 	);
    *
    * Return:
    * 	void
    *
    * Notes: the completion routine registered in asyncRecv and saves the data received to a file and saves stats to the TestSet instance
    */
void CALLBACK workerRoutineTCP_server(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);
    /**
    * Function:workerRoutineUDP_server
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void CALLBACK workerRoutineUDP_server(
    * 		DWORD error,				- the error that occured if not zero
    * 		DWORD bytesTrans,			- the bytes read
    * 	   	LPWSAOVERLAPPED overlapped,	- the overlapped structure used
    * 		DWORD inFlags				- flags set by the caller
    * 	);
    *
    * Return:
    * 	void
    *
    * Notes: the completion routine registered in asyncRecvFrom and saves the data received to a file and saves stats to the TestSet instance
    */
void CALLBACK workerRoutineUDP_server(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);

    /**
    * Function:workerRoutine_client
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    * 	void CALLBACK workerRoutine_client(
    * 		DWORD error,				- the error that occured if not zero
    * 		DWORD bytesTrans,			- the bytes sent
    * 	   	LPWSAOVERLAPPED overlapped,	- the overlapped structure used
    * 		DWORD inFlags				- flags set by the caller
    * 	);
    *
    * Return:
    * 	void
    *
    * Notes: the completion routine for use inside of asyncRecv and asyncRecvFrom
    * it is never needed except for handling sending errors
    */
void CALLBACK workerRoutine_client(DWORD error, DWORD bytesTrans,
   LPWSAOVERLAPPED overlapped, DWORD inFlags);

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
