#ifndef __PACKETS_H__
#define __PACKETS_H__

#define PACKET_AUTH      0
#define PACKET_REPLY     1
#define PACKET_AGENTDATA 2
#define PACKET_START     3
#define PACKET_STOP      4

#include <string.h>

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
class TAgentData
{
	
};

#endif
