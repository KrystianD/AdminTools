#ifndef __PACKETS_H__
#define __PACKETS_H__

#define PACKET_AUTH      0
#define PACKET_REPLY     1
#define PACKET_AGENTDATA 2
#define PACKET_START     3
#define PACKET_STOP      4
#define PACKET_PING      5
#define PACKET_AGENTSDATA 6
#define PACKET_KEY_REQUEST 7
#define PACKET_KEY_REPLY   8
#define PACKET_CONFIG    9

#include <string.h>
#include <stdint.h>

#include <vector>
using namespace std;

#include "db.h"
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
	TSensorsData data;

	virtual int getType () { return PACKET_AGENTDATA; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, id);
		buffer_t tmp;
		data.toBuffer (tmp);
		buf.insert (buf.end (), tmp.begin (), tmp.end ());
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, id)) return false;
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
			if (!fetch (buf, s.port)) return false;
			services.push_back (s);
		}
		if (!fetch (buf, interval)) return false;
		return true;
	}
};

#endif
