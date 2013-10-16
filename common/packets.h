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

class IPacket
{
public:
	virtual int getType () = 0;
	virtual void toBuffer (buffer_t& buf) = 0;
	virtual void fromBuffer (buffer_t& buf) = 0;

	template<typename T>
	bool append (buffer_t& buf, T& val)
	{
		buf.insert (buf.end (), (char*)&val, (char*)&val + sizeof (T));
	}
	template<typename T>
	bool fetch (buffer_t& buf, T& val)
	{
		if (m_pos + sizeof (T) > buf.size ())
			return false;
		memcpy (&val, &buf[m_pos], sizeof (T));
		m_pos += sizeof (T);
		return true;
	}

protected:
	int m_pos;
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
class TAgentData : public IPacket
{
public:
	uint16_t id;
	float temp;

	virtual int getType () { return PACKET_AGENTDATA; }
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, id);
		append (buf, temp);
	}
	virtual void fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		fetch (buf, id);
		fetch (buf, temp);
	}
};
class TAgentsData : public IPacket
{
public:
	vector<TAgentData> agents;

	virtual int getType () { return PACKET_AGENTSDATA; }
	virtual void toBuffer (buffer_t& buf)
	{
		uint16_t len = agents.size ();
		append (buf, len);
		for (int i = 0; i < agents.size (); i++)
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
class TKeyReply : public IPacket
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
