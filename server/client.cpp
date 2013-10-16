#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"
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
		close (fd);
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
			printf ("New header - type: %d size: %d\r\n",
					currentHeader.type, currentHeader.size);

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

		printf ("shifting buffer by: %d\r\n", dataToReceive);
		memcpy (buffer, buffer + dataToReceive, bufferPointer - dataToReceive);
		bufferPointer -= dataToReceive;
		dataToReceive = newDataLen;
	}

	// printf ("rd: %d\r\n", rd);
	// for (int i=0;i<rd;i++)
	// {
		// putchar (buffer[i]);
	// }
}
void Client::process ()
{
	if (dataToSend.size () > 0)
	{
		printf ("sending data...\r\n");

		int len = dataToSend.size ();

		int sent = send (fd, &dataToSend[0], len, 0);
		if (sent == 0)
		{
			printf ("connection error, disconnecting\r\n");
			toDelete = true;
			close (fd);
			return;
		}

		printf ("sent %d bytes, shifting... ", sent);
		dataToSend.erase (dataToSend.begin (), dataToSend.begin () + sent);
		printf ("%d bytes left to send\r\n", dataToSend.size ());
	}
}

// template<typename T>
// bool Client::getVal (T& val)
// {
	// const THeader& h = currentHeader;
	// printf ("dp: %d, sz: %d, h.size: %d\r\n", dataPointer, sizeof (T), h.size);
	// if (dataPointer + sizeof (T) > h.size)
		// return false;
	// val = *((T*)(buffer + dataPointer));
	// dataPointer += sizeof (T);
	// return true;
// }

void Client::processPacket (int size)
{
	const THeader& h = currentHeader;
	// dataPointer = 0;
	printf ("Processing packet of type: %d size: %d\r\n", h.type, h.size);
	
	buffer_t buf;
	buf.insert (buf.begin (), (char*)buffer, (char*)buffer + size);

	switch (h.type)
	{
	case PACKET_AUTH:
		{
			TPacketAuth p;
			p.fromBuffer (buf);

			// printf ("key: %s\r\n", p.key);

			TPacketReply pr;
			pr.value = 1;		
			sendPacket (pr);
		}		
		break;
	}
}
bool Client::sendPacket (IPacket& packet)
{
	buffer_t b;
	packet.toBuffer (b);
	THeader h;
	h.type = packet.getType ();
	h.size = b.size ();

	dataToSend.insert (dataToSend.end (), (char*)&h, (char*)&h + sizeof (h));
	dataToSend.insert (dataToSend.end (), &b[0], &b[0] + b.size ());

	printf ("dataToSend len: %d\r\n", dataToSend.size ());
}
