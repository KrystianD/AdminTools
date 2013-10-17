#include "common.h"

#include <stdio.h>

// net
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

int recvall (int sockfd, void* data, int size, int timeout)
{
	long arg = fcntl (sockfd, F_GETFL, NULL);
  fcntl (sockfd, F_SETFL, arg | O_NONBLOCK);

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
					fcntl (sockfd, F_SETFL, arg);
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

	fcntl (sockfd, F_SETFL, arg);
	return size;
}
int sendall (int sockfd, void* data, int size)
{
	long arg = fcntl (sockfd, F_GETFL, NULL);
  fcntl (sockfd, F_SETFL, arg | O_NONBLOCK);

	int toSend = size;
	char* pointer = (char*)data;
	while (toSend > 0)
	{
		fd_set clientWrite;
		while (true)
		{
			FD_ZERO (&clientWrite);
			FD_SET (sockfd, &clientWrite);

			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 1000 * 1000;
			select (sockfd + 1, NULL, &clientWrite, NULL, &tv);

			if (FD_ISSET (sockfd, &clientWrite))
			{
				int sent = send (sockfd, pointer, toSend, 0);
				printf ("Sent %d bytes\r\n", sent);
				if (sent <= 0)
				{
					fcntl (sockfd, F_SETFL, arg);
					return sent;
				}
				toSend -= sent;
				pointer += sent;

				break;
			}
			printf ("Waiting for write...\r\n");
		}
	}

	fcntl (sockfd, F_SETFL, arg);
	return size;
}
bool fileExists (const string& path)
{
	FILE *f = fopen (path.c_str (), "r");
	if (!f)
		return false;
	fclose (f);
	return true;
}
bool fileAccessible (const string& path)
{
	FILE *f = fopen (path.c_str (), "r+");
	if (!f)
		return false;
	fclose (f);
	return true;
}
