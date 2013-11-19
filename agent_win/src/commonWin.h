#ifndef __COMMONWIN_H__
#define __COMMONWIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "winsock2.h"
#include "ws2tcpip.h"
#include "windows.h"

#include <vector>
#include <string>
using namespace std;


namespace WinAgent
{
/**
 *	\fn int recvallWin (SOCKET sockfd, void* data, int size, int timeout)
 *	\brief Receive data from given socket.
 *	\param sockfd Socket descriptor.
 *	\param data Pointer to received data.
 *	\param size Number of bytes to be read.
 *	\param timeout Time to wait for data.
 *	\return Size of received data.
 */
int recvallWin (SOCKET sockfd, void* data, int size, int timeout);
}

#endif