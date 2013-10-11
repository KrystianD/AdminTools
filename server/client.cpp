#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "packets.h"

Client::Client (int fd, const string& ip, int port)
	: fd (fd), ip (ip), port (port)
{
	printf ("New client fd: %d addr: %s:%d\r\n", fd, ip.c_str (), port);

	state = WAITING_FOR_HEADER;
	bufferPointer = 0;
	dataToReceive = sizeof (THeader);
	toDelete = false;
}

void Client::readData ()
{
	// new data to be received
	int rd = recv (fd, buffer + bufferPointer, sizeof (buffer) - bufferPointer, 0);
	if (rd == 0)
	{
		toDelete = true;
		return;
	}

	bufferPointer += rd;
	printf ("new data: %d\r\n", rd);

	while (bufferPointer >= dataToReceive)
	{
		int newDataLen;

		switch (state)
		{
		case WAITING_FOR_HEADER:
			currentHeader = *((THeader*)buffer);
			printf ("New header - key: %.*s type: %d size: %d\r\n",
					sizeof (currentHeader.key), currentHeader.key, currentHeader.type, currentHeader.size);

			if (currentHeader.size > 0)
			{
				printf ("Waiting for a packet\r\n");
				state = WAITING_FOR_PACKET;
				newDataLen = currentHeader.size;
			}
			else
			{
				printf ("Waiting for new header\r\n");
				newDataLen = sizeof (THeader);
			}
			break;
		case WAITING_FOR_PACKET:
			processPacket (dataToReceive);
			printf ("Waiting for new header\r\n");
			newDataLen = sizeof (THeader);
			break;
		}

		memcpy (buffer, buffer + dataToReceive, dataToReceive);
		bufferPointer -= dataToReceive;
		dataToReceive = newDataLen;
	}

	printf ("rd: %d\r\n", rd);
	for (int i=0;i<rd;i++)
	{
		putchar (buffer[i]);
	}
}
void Client::process ()
{
}

template<typename T>
bool Client::getVal (T& val)
{
	const THeader& h = currentHeader;
	printf ("dp: %d, sz: %d, h.size: %d\r\n", dataPointer, sizeof (T), h.size);
	if (dataPointer + sizeof (T) > h.size)
		return false;
	val = *((T*)(buffer + dataPointer));
	dataPointer += sizeof (T);
	return true;
}

void Client::processPacket (int size)
{
	const THeader& h = currentHeader;
	dataPointer = 0;
	printf ("Processing packet of type: %d size: %d\r\n", h.type, h.size);

	uint8_t val;
	if (!getVal (val)) return;

	printf ("val: %d \r\n", val);
}

