#include "commonWin.h"

#include <stdio.h>
#include "winsock2.h"
#include "ws2tcpip.h"
#include "windows.h"

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