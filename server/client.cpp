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

#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cmath>

#include <errno.h>
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

#define CLIENT_DEBUG(x,...) printf ("[Client %2d# (%-10s:%2d)] " x "\r\n", fd, dbAgent.name.c_str (), dbAgent.id, ##__VA_ARGS__)

string ftm (uint32_t tm, uint32_t base)
{
	char buf[100];
	sprintf (buf, "%02d:%02d", ((tm - base) / 3600) % 24, ((tm - base) / 60) % 60);
	return buf;
}

Client::Client (int fd, const string& ip, int port)
	: fd (fd), ip (ip), port (port)
{
	dbAgent.id = -1;

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
}
void Client::process ()
{
	if (dataToSend.size () > 0)
	{
		int len = dataToSend.size ();

		CLIENT_DEBUG ("[SEND] Sending %d bytes", len);
		int sent = send (fd, &dataToSend[0], len, MSG_DONTWAIT);
		if (sent <= 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				fprintf (stderr, "skip\n");
			}
			else
			{
				CLIENT_DEBUG("connection error, disconnecting");
				toDelete = true;
				close (fd);
				return;
			}
		}

		if (sent > 0)
		{
			dataToSend.erase (dataToSend.begin (), dataToSend.begin () + sent);
			CLIENT_DEBUG ("[SEND] %d bytes sent, %d bytes left", sent, dataToSend.size ());
		}
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
		CLIENT_DEBUG("Inserting %d records of old data", oldData.size ());
		DB::insertRecords (dbAgent, oldData);
		CLIENT_DEBUG("Done.");
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
	buf.rewind ();

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
			CLIENT_DEBUG("Invalid key %.*s", 16, p.key);
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
			fixPacket (p);
			p.id = dbAgent.id;
			if (dbAgent.name.size () == 0)
				p.name = ip;
			else
				p.name = dbAgent.name + " (" + ip + ")";
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
		if (agent.interval < AGENT_UPDATE_INTERVAL_MIN)
			agent.interval = AGENT_UPDATE_INTERVAL_MIN;
		if (agent.interval > AGENT_UPDATE_INTERVAL_MAX)
			agent.interval = AGENT_UPDATE_INTERVAL_MAX;
		DB::updateAgent (agent);

		settingsChanged = 1;

		TPacketReply pr;
		pr.value = 1;
		sendPacket (pr, PACKET_CONFIG_CHANGE_REPLY);
	}
	break;
	case PACKET_STATS_REQUEST:
	{
		if (!authorized) { kill (); return; }
		TPacketStatsRequest p;
		p.fromBuffer (buf);

		generateAndSendStats (p);
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
	config.name = dbAgent.name;
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
	dbAgent.name = config.name;
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
	TPacketStatsReply r;

	uint32_t start = req.startDate;
	uint32_t end = req.endDate;

	fprintf (stderr, "ag: %d s: %d e: %d\n", req.agentId, req.startDate, req.endDate);
	fprintf (stderr, "req pt: %d\n", req.points);

	vector<TSensorsRecord> rec;
	if (!DB::getRecords (req.agentId, start, end - 1, rec))
	{
		r.points.clear ();
		sendPacket (r);
		return false;
	}

	int pointsCount = req.points;
	if (pointsCount == 0)
	{
		r.points.clear ();
		sendPacket (r);
		return false;
	}
	vector<int16_t> points;

	uint32_t cur = start;
	uint32_t step = (end - start) / pointsCount;
	int idx = 0;
	int pointIdx = 0;
	string disk = req.diskName;
	while (cur < end)
	{
		uint32_t rangeBegin = cur;
		uint32_t rangeEnd = cur + step;

		while (idx < rec.size () && rec[idx].timestamp < rangeBegin)
			idx++;
		int valid = 0, validDisks = 0;
		int startIdx = idx;
		while (idx < rec.size () && rec[idx].timestamp < rangeEnd)
		{
			TSensorsRecord& r = rec[idx];
			valid++;

			for (int j = 0; j < r.disks.size (); j++)
			{
				if (r.disks[j].name == disk)
				{
					validDisks++;
					break;
				}
			}

			idx++;
		}
		int endIdx = idx;

		double tempAvg = 0, ramAvg = 0, cpuAvg = 0, diskAvg = 0;
		for (int i = startIdx; i < endIdx; i++)
		{
			TSensorsRecord& r = rec[i];
			tempAvg += r.temp / valid;
			ramAvg += r.ramUsage / valid;
			cpuAvg += r.cpuUsage / valid;

			for (int j = 0; j < r.disks.size (); j++)
			{
				if (r.disks[j].name == disk)
				{
					diskAvg += r.disks[j].usage / validDisks;
					break;
				}
			}
		}

		cur += step;
		// printf ("range (%s..%s) st: %4d en: %4d cnt: %3d  temp: %5.2f  ram: %2d%% cpu: %3d%% disk: %3d%%\n",
				// ftm (rangeBegin, base).c_str (), ftm (rangeEnd, base).c_str (), startIdx, endIdx, valid,
				// tempAvg, (int)round (ramAvg * 100.0), (int)round (cpuAvg * 100.0), (int)round (diskAvg * 100.0));

		if (valid)
		{
			switch (req.type)
			{
			case TPacketStatsRequest::CPU:
				r.points.push_back (cpuAvg);
				break;
			case TPacketStatsRequest::TEMP:
				r.points.push_back (tempAvg);
				break;
			case TPacketStatsRequest::RAM:
				r.points.push_back (ramAvg);
				break;
			case TPacketStatsRequest::DISK:
				r.points.push_back (diskAvg);
				break;
			}
		}
		else
		{
			r.points.push_back (-1);
		}

		pointIdx++;
	}
	
	sendPacket (r);
	lastPingTime = getTicks ();

	return true;
}
