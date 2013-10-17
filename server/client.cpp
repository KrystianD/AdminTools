#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "agents.h"
#include "kutils.h"
#include "common.h"
#include "packets.h"
#include "settings.h"
#include "db.h"

#define CLIENT_DEBUG(x,...) printf ("[Client #%2d id: %2d] " x "\r\n", fd, agentId, ##__VA_ARGS__)

Client::Client (int fd, const string& ip, int port)
	: fd (fd), ip (ip), port (port)
{
	CLIENT_DEBUG("New client fd: %d addr: %s:%d", fd, ip.c_str (), port);

	state = WAITING_FOR_HEADER;
	bufferPointer = 0;
	dataToReceive = sizeof (THeader);
	toDelete = false;
	lastPingTime = getTicks ();
	agentId = -1;

	sendingActive = 0;
	authorized = false;
}

void Client::readData ()
{
	// new data to be received
	int rd = recv (fd, buffer + bufferPointer, sizeof (buffer) - bufferPointer, 0);
	if (rd <= 0)
	{
		toDelete = true;
		close (fd);
		return;
	}

	lastPingTime = getTicks ();
	bufferPointer += rd;
	CLIENT_DEBUG("[READ] New data: %d", rd);

	while (bufferPointer >= dataToReceive)
	{
		int newDataLen;

		switch (state)
		{
		case WAITING_FOR_HEADER:
			currentHeader = *((THeader*)buffer);
			CLIENT_DEBUG("[READ] New header - type: %d size: %d",
					currentHeader.type, currentHeader.size);

			if (currentHeader.size > 0)
			{
				CLIENT_DEBUG("[READ] Waiting for a packet");
				state = WAITING_FOR_PACKET;
				packetStartTime = getTicks ();
				newDataLen = currentHeader.size;
			}
			else
			{
				state = WAITING_FOR_HEADER;
				processPacket (0);
				CLIENT_DEBUG("[READ] Waiting for new header");
				newDataLen = sizeof (THeader);
			}
			break;
		case WAITING_FOR_PACKET:
			state = WAITING_FOR_HEADER;
			processPacket (dataToReceive);
			CLIENT_DEBUG("[READ] Waiting for new header");
			newDataLen = sizeof (THeader);
			break;
		}

		CLIENT_DEBUG("[READ] Shifting buffer by: %d", dataToReceive);
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
		int len = dataToSend.size ();

		int sent = send (fd, &dataToSend[0], len, 0);
		if (sent <= 0)
		{
			CLIENT_DEBUG("connection error, disconnecting");
			toDelete = true;
			close (fd);
			return;
		}

		dataToSend.erase (dataToSend.begin (), dataToSend.begin () + sent);
		CLIENT_DEBUG ("[SEND] %d bytes sent, %d bytes left", sent, dataToSend.size ());
	}

	if (state == WAITING_FOR_PACKET && getTicks () - packetStartTime >= SERVER_PACKET_TIMEOUT)
	{
		CLIENT_DEBUG("[SEND] packet read timeout");
		toDelete = true;
		close (fd);
		return;
	}

	if (getTicks () - lastPingTime >= SERVER_PING_TIME)
	{
		CLIENT_DEBUG("[SEND] ping timeout\r\n");
		toDelete = true;
		close (fd);
		return;
	}

	if (sendingActive && sendDataTimer.process ())
	{
		TPacketAgentsData d;
		d.agents = agentsData;
		sendPacket (d);
	}
}

void Client::processPacket (int size)
{
	const THeader& h = currentHeader;
	CLIENT_DEBUG("[READ] Processing packet of type: %d size: %d", h.type, h.size);
	
	buffer_t buf;
	buf.insert (buf.begin (), (char*)buffer, (char*)buffer + size);


	switch (h.type)
	{
	case PACKET_AUTH:
	{
		if (size == 0) { kill (); return; }
		TPacketAuth p;
		p.fromBuffer (buf);

		TDBAgent agent;
		TPacketReply pr;
		int clientId;
		if (DB::findAgent (p.key, &agent))
		{
			CLIENT_DEBUG("Authorized");
			pr.value = 1;
			agentId = agent.id;
			authorized = true;
		}
		else
		{
			CLIENT_DEBUG("Invalid key");
			pr.value = 0;
		}
		sendPacket (pr);
	}
	break;
	case PACKET_AGENTDATA:
	{
		if (size == 0) { kill (); return; }
		if (!authorized) { kill (); return; }
		TPacketAgentData p;
		p.fromBuffer (buf);
		p.id = agentId;

		assignData (p);

		// printf ("temp: %f\r\n", p.temp);
	}
	break;
	case PACKET_START:
	{
		if (size == 0) { kill (); return; }
		if (!authorized) { kill (); return; }
		TPacketStart p;
		p.fromBuffer (buf);

		CLIENT_DEBUG("START");
		sendingActive = true;

		if (p.interval <= SERVER_INTERVAL_MIN)
			p.interval = SERVER_INTERVAL_MIN;
		if (p.interval >= SERVER_INTERVAL_MAX)
			p.interval = SERVER_INTERVAL_MAX;

		sendDataTimer.setInterval (p.interval);		
	}
	break;
	case PACKET_STOP:
	{
		if (!authorized) { kill (); return; }
		CLIENT_DEBUG("STOP");
		sendingActive = false;
	}
	break;
	case PACKET_KEY_REQUEST:
	{
		if (!authorized) { kill (); return; }
		TPacketKeyReply pr;
		DB::generateNewKey (pr.key);
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

	CLIENT_DEBUG("[SEND] New data to send: %d", dataToSend.size ());
}
void Client::kill ()
{
	toDelete = true;
	close (fd);
}
