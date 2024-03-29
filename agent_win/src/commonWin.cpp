/*
Copyright (c) 2013, AGH University of Science and Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "commonWin.h"

#include <stdio.h>
#include "winsock2.h"
#include "ws2tcpip.h"
#include "windows.h"

namespace WinAgent
{

int recvallWin (SOCKET sockfd, void* data, int size, int timeout)
{
	//long arg = fcntl (sockfd, F_GETFL, NULL); //znalesc zamiennik dla fcntl dla windowsa
	//fcntl (sockfd, F_SETFL, arg | O_NONBLOCK);

	int toRead = size;
	char* pointer = (char*)data;
	while (toRead > 0)
	{
		fd_set clientRead;
		while (true)
		{
			FD_ZERO (&clientRead);
			FD_SET (sockfd, &clientRead);

			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 10 * 1000;
			int res = select (sockfd + 1, &clientRead, NULL, NULL, &tv);

			if (res == -1)
				return 0;

			if (res > 0 && FD_ISSET (sockfd, &clientRead))
			{
				int read = recv (sockfd, pointer, toRead, 0);
				printf ("Received %d bytes\r\n", read);
				if (read <= 0)
				{
					//fcntl (sockfd, F_SETFL, arg);
					return read;
				}
				toRead -= read;
				pointer += read;

				break;
			}

			timeout -= 10;
			if (timeout <= 0)
				return 0;

			printf ("Waiting for read...\r\n");
		}
	}

	//fcntl (sockfd, F_SETFL, arg);
	return size;
}

}