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

#define CLIENT_DEBUG(x,...) printf ("[Client #%d id: %2d] " x "\r\n", fd, dbAgent.id, ##__VA_ARGS__)

Client::Client (int fd, const string& ip, int port)
	: fd (fd), ip (ip), port (port)
{
	CLIENT_DEBUG("New client fd: %d addr: %s:%d", fd, ip.c_str (), port);

	state = WAITING_FOR_HEADER;
	bufferPointer = 0;
	dataToReceive = sizeof (THeader);
	toDelete = false;
	lastPingTime = getTicks ();
	settingsChanged = false;

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
		CLIENT_DEBUG("[READ] packet read timeout");
		toDelete = true;
		close (fd);
		return;
	}

	if (getTicks () - lastPingTime >= SERVER_PING_TIME)
	{
		CLIENT_DEBUG("[READ] ping timeout\r\n");
		toDelete = true;
		close (fd);
		return;
	}

	if (sendingActive && sendDataTimer.process ())
	{
		TPacketAgentsData d;
		for (int i = 0; i < agentsData.size (); i++)
			if (time (0) - agentsData[i].time < 10)
				d.agents.push_back (agentsData[i].packet);
		sendPacket (d);
	}

	if (oldData.size () > 0 && getTicks () - oldDataUpdateTime >= 2000)
	{
		printf("\n\n\n\n\n\n\n\n");
		printf ("Inserting %d of old data\n", oldData.size ());
		DB::insertRecords (dbAgent, oldData);
		printf ("Done.\n");
		oldData.clear ();
		oldDataUpdateTime = getTicks ();
	}
}

void Client::fetchConfig ()
{
	DB::findAgentById (dbAgent.id, dbAgent);
	sendConfig ();
	CLIENT_DEBUG("[CONFIG] New config fetched and sent");
}

void Client::processPacket (int size)
{
	const THeader& h = currentHeader;

	const char* typeStr = "(unknown)";
	if (h.type < PACKET_TYPES_COUNT)
		typeStr = packetNames[h.type];

	CLIENT_DEBUG("[READ] Processing packet of type: %s size: %d", typeStr, h.size);
	
	buffer_t buf;
	buf.insert (buf.begin (), (char*)buffer, (char*)buffer + size);

	switch (h.type)
	{
	case PACKET_AUTH:
	{
		if (size == 0) { kill (); return; }
		TPacketAuth p;
		p.fromBuffer (buf);

		TPacketReply pr;
		if (DB::findAgentByKey (p.key, dbAgent))
		{
			CLIENT_DEBUG("Authorized id: %d", dbAgent.id);
			pr.value = 1;
			authorized = true;
		}
		else
		{
			CLIENT_DEBUG("Invalid key");
			pr.value = 0;
		}
		sendPacket (pr);

		if (authorized && p.sendConfig)
		{
			sendConfig ();
			CLIENT_DEBUG("Config sent");
		}
	}
	break;
	case PACKET_AGENTDATA:
	{
		if (size == 0) { kill (); return; }
		if (!authorized) { kill (); return; }
		TPacketAgentData p;
		if (p.fromBuffer (buf))
		{
			p.id = dbAgent.id;
			p.name = ip;
			if (p.oldData)
			{
				oldData.push_back (p.data);
			}
			else
			{
				assignData (p);
				DB::insertRecord (dbAgent, p.data);
			}
		}
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

		if (p.interval <= CLIENT_UPDATE_INTERVAL_MIN)
			p.interval = CLIENT_UPDATE_INTERVAL_MIN;
		if (p.interval >= CLIENT_UPDATE_INTERVAL_MAX)
			p.interval = CLIENT_UPDATE_INTERVAL_MAX;

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
	case PACKET_CONFIG_REQUEST:
	{
		if (!authorized) { kill (); return; }
		TPacketConfigRequest p;
		p.fromBuffer (buf);

		TDBAgent agent;
		TPacketConfig pc;
		if (DB::findAgentById (p.agentId, agent))
		{
			agentToConfig (agent, pc);
		}
		else
		{
			pc.agentId = 0xffff;
		}
		sendPacket (pc, PACKET_CONFIG_REPLY);
	}
	break;
	case PACKET_CONFIG_CHANGE_REQUEST:
	{
		if (!authorized) { kill (); return; }
		TPacketConfig p;
		p.fromBuffer (buf);

		TDBAgent agent;
		configToAgent (p, agent);
		DB::updateAgent (agent);

		settingsChanged = 1;

		TPacketReply pr;
		pr.value = 1;
		sendPacket (pr, PACKET_CONFIG_CHANGE_REPLY);
	}
	break;
	}
}
bool Client::sendPacket (IPacket& packet, int type)
{
	buffer_t b;
	packet.toBuffer (b);
	THeader h;
	if (type == -1)
		h.type = packet.getType ();
	else 
		h.type = type;
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
void Client::sendConfig ()
{
	TPacketConfig p;
	agentToConfig (dbAgent, p);
	sendPacket (p);
}

void Client::agentToConfig (const TDBAgent& dbAgent, TPacketConfig& config)
{
	config.agentId = dbAgent.id;
	config.services.clear ();
	for (int i = 0; i < dbAgent.services.size (); i++)
	{
		TPacketConfig::TService s;
		s.name = dbAgent.services[i].name;
		s.tcp = dbAgent.services[i].tcp;
		s.port = dbAgent.services[i].port;
		config.services.push_back (s);
	}
	config.tempPath = dbAgent.tempPath;
	config.tempDivider = dbAgent.tempDivider;
	config.interval = dbAgent.interval;
}
void Client::configToAgent (const TPacketConfig& config, TDBAgent& dbAgent)
{
	dbAgent.id = config.agentId;
	dbAgent.services.clear ();
	for (int i = 0; i < config.services.size (); i++)
	{
		TDBService s;
		s.name = config.services[i].name;
		s.tcp = config.services[i].tcp;
		s.port = config.services[i].port;
		dbAgent.services.push_back (s);
	}
	dbAgent.tempPath = config.tempPath;
	dbAgent.tempDivider = config.tempDivider;
	dbAgent.interval = config.interval;
}
bool Client::generateAndSendStats (const TPacketStatsRequest& req)
{
	vector<TSensorsRecord> records;
	if (!DB::getRecords (req.agentId, req.startDate, req.endDate, records))
		return false;



	return true;
}
