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

#include <string.h>
#include <stdint.h>

#include <vector>
using namespace std;

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
	
	virtual int getType () { return PACKET_AUTH; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, key);
	}
	virtual void fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		fetch (buf, key);
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
	virtual void fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		fetch (buf, value);
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
	virtual void fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		fetch (buf, interval);
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
	virtual void fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		fetch (buf, id);
		buffer_t tmp;
		tmp.insert (tmp.begin (), buf.begin () + m_pos, buf.end ());
		data.fromBuffer (tmp);
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
	virtual void fromBuffer (buffer_t& buf)
	{
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
	virtual void fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		fetch (buf, key);
	}
};

#endif
