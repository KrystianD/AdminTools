#ifndef __PACKETS_H__
#define __PACKETS_H__

#define PACKET_AUTH        0
#define PACKET_REPLY       1
#define PACKET_AGENTDATA   2
#define PACKET_START       3
#define PACKET_STOP        4
#define PACKET_PING        5
#define PACKET_AGENTSDATA  6
#define PACKET_KEY_REQUEST 7
#define PACKET_KEY_REPLY   8
#define PACKET_CONFIG      9
#define PACKET_CONFIG_REQUEST 10
#define PACKET_CONFIG_REPLY   11
#define PACKET_CONFIG_CHANGE_REQUEST 12
#define PACKET_CONFIG_CHANGE_REPLY   13
#define PACKET_STATS_REQUEST  14
#define PACKET_STATS_REPLY    15
#define PACKET_TYPES_COUNT 16

static const char* packetNames[] = {
/* 0 */ "PACKET_AUTH",
/* 1 */ "PACKET_REPLY",
/* 2 */ "PACKET_AGENTDATA",
/* 3 */ "PACKET_START",
/* 4 */ "PACKET_STOP",
/* 5 */ "PACKET_PING",
/* 6 */ "PACKET_AGENTSDATA",
/* 7 */ "PACKET_KEY_REQUEST",
/* 8 */ "PACKET_KEY_REPLY",
/* 9 */ "PACKET_CONFIG",
/* 10 */ "PACKET_CONFIG_REQUEST",
/* 11 */ "PACKET_CONFIG_REPLY",
/* 12 */ "PACKET_CONFIG_CHANGE_REQUEST",
/* 13 */ "PACKET_CONFIG_CHANGE_REPLY",
/* 14 */ "PACKET_STATS_REQUEST",
/* 15 */ "PACKET_STATS_REPLY",
};

#include <string.h>
#include <stdint.h>

#include <vector>
using namespace std;

// #include "db.h"
#include "common.h"
#include "sensors.h"

class IPacket : public ISerializable
{
public:
	virtual int getType () = 0;
};

#pragma pack(1)
struct THeader
{
	uint8_t type;
	uint16_t size;
};
#pragma pack()

class TPacketAuth : public IPacket
{
public:
	char key[16];
	uint8_t sendConfig;
	
	virtual int getType () { return PACKET_AUTH; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, key);
		append (buf, sendConfig);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, key)) return false;
		if (!fetch (buf, sendConfig)) return false;
		return true;
	}
};
class TPacketReply : public IPacket
{
public:
	int value;
	
	virtual int getType () { return PACKET_REPLY; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, value);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, value)) return false;
		return true;
	}
};
class TPacketStart : public IPacket
{
public:
	uint16_t interval;

	virtual int getType () { return PACKET_START; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, interval);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, interval)) return false;
		return true;
	}
};
class TPacketAgentData : public IPacket
{
public:
	uint16_t id;
	uint8_t oldData;
	string name;
	TSensorsData data;

	virtual int getType () { return PACKET_AGENTDATA; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, id);
		append (buf, oldData);
		append (buf, name);
		buffer_t tmp;
		data.toBuffer (tmp);
		buf.insert (buf.end (), tmp.begin (), tmp.end ());
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, id)) return false;
		if (!fetch (buf, oldData)) return false;
		if (!fetch (buf, name)) return false;
		buffer_t tmp;
		tmp.insert (tmp.begin (), buf.begin () + m_pos, buf.end ());
		data.fromBuffer (tmp);
		return true;
	}
};
class TPacketAgentsData : public IPacket
{
public:
	vector<TPacketAgentData> agents;

	virtual int getType () { return PACKET_AGENTSDATA; }
	virtual void toBuffer (buffer_t& buf)
	{
		uint16_t len = agents.size ();
		append (buf, len);
		for (int i = 0; i < len; i++)
		{
			buffer_t tmp;
			agents[i].toBuffer (tmp);
			buf.insert (buf.end (), tmp.begin (), tmp.end ());
		}
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		return true;
	}
};
class TPacketKeyReply : public IPacket
{
public:
	char key[16];
	
	virtual int getType () { return PACKET_KEY_REPLY; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, key);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, key)) return false;
		return true;
	}
};
class TPacketConfig : public IPacket
{
public:
	struct TService
	{
		string name;
		bool tcp;
		uint16_t port;
	};
	uint16_t agentId;
	string tempPath;
	uint16_t tempDivider;
	vector<TPacketConfig::TService> services;
	uint16_t interval;

	virtual int getType () { return PACKET_CONFIG; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, agentId);
		append (buf, tempPath);
		append (buf, tempDivider);
		uint16_t len = services.size ();
		append (buf, len);
		for (int i = 0; i < len; i++)
		{
			append (buf, services[i].name);
			append (buf, services[i].tcp);
			append (buf, services[i].port);
		}
		append (buf, interval);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, agentId)) return false;
		if (!fetch (buf, tempPath)) return false;
		if (!fetch (buf, tempDivider)) return false;
		uint16_t len;
		if (!fetch (buf, len)) return false;
		services.clear ();
		while (len--)
		{
			TPacketConfig::TService s;
			if (!fetch (buf, s.name)) return false;
			if (!fetch (buf, s.tcp)) return false;
			if (!fetch (buf, s.port)) return false;
			services.push_back (s);
		}
		if (!fetch (buf, interval)) return false;
		return true;
	}
};
class TPacketConfigRequest : public IPacket
{
public:
	uint16_t agentId;
	
	virtual int getType () { return PACKET_CONFIG_REQUEST; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, agentId);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, agentId)) return false;
		return true;
	}
};
class TPacketStatsRequest : public IPacket
{
public:
	enum EType { CPU = 0, RAM, TEMP, DISK };
	uint16_t agentId;
	uint32_t startDate, endDate;
	uint16_t points;
	EType type; // 1 byte
	string diskName;

	virtual int getType () { return PACKET_STATS_REQUEST; }
	virtual void toBuffer (buffer_t& buf)
	{
		// append (buf, agentId);
		// append (buf, startDate);
		// append (buf, endDate);
		// append (buf, points);
		// uint8_t t = (uint8_t)type;
		// append (buf, t);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, agentId)) return false;
		if (!fetch (buf, startDate)) return false;
		if (!fetch (buf, endDate)) return false;
		if (!fetch (buf, points)) return false;
		uint8_t t;
		if (!fetch (buf, t)) return false;
		type = (EType)t;
		if (!fetch (buf, diskName)) return false;
		return true;
	}
};
class TPacketStatsReply : public IPacket
{
public:
	vector<float> points;

	virtual int getType () { return PACKET_STATS_REPLY; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, points);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		return true;
	}
};

#endif
