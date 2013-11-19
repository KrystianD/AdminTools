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

/**
 *	\class IPacket
 *	\brief Packet data base class.
 */
class IPacket : public ISerializable
{
public:
	/**
	 *	\fn virtual int getType() = 0
	 *	\brief Get packet type.
	 *	\return Packet type.
	 */
	virtual int getType () = 0;
};

#pragma pack(1)
/**
 *	\struct THeader
 *	\brief Packet header.
 */
struct THeader
{
	//! Packet type.
	uint8_t type;
	//! Packet size.
	uint16_t size;
};
#pragma pack()

/**
 *	\class TPacketAuth
 *	\brief Authentication packet.
 */
class TPacketAuth : public IPacket
{
public:
	//! Key.
	char key[16];
	//! Config data.
	uint8_t sendConfig;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_AUTH.
	 */
	virtual int getType () { return PACKET_AUTH; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, key);
		append (buf, sendConfig);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, key)) return false;
		if (!fetch (buf, sendConfig)) return false;
		return true;
	}
};

/**
 *	\class TPacketReply
 *	\brief Reply packet.
 */
class TPacketReply : public IPacket
{
public:
	//! Reply value.
	int value;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_REPLY.
	 */
	virtual int getType () { return PACKET_REPLY; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, value);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, value)) return false;
		return true;
	}
};

/**
 *	\class TPacketStart
 *	\brief Starting packet.
 */
class TPacketStart : public IPacket
{
public:
	//! Time interval.
	uint16_t interval;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_START.
	 */
	virtual int getType () { return PACKET_START; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, interval);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, interval)) return false;
		return true;
	}
};

/**
 *	\class TPacketAgentData
 *	\brief Data from agent.
 */
class TPacketAgentData : public IPacket
{
public:
	//! Packet id.
	uint16_t id;
	//! Old data.
	uint8_t oldData;
	//! Agent name.
	string name;
	//! Sensors data.
	TSensorsData data;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_AGENTDATA.
	 */
	virtual int getType () { return PACKET_AGENTDATA; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, id);
		append (buf, oldData);
		append (buf, name);
		buffer_t tmp;
		data.toBuffer (tmp);
		buf.insert (buf.end (), tmp.begin (), tmp.end ());
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!buf.fetch (id)) return false;
		if (!buf.fetch (oldData)) return false;
		if (!buf.fetch (name)) return false;
		data.fromBuffer (buf);
		return true;
	}
};

/**
 *	\class TPacketAgentsData
 *	\brief Combined agents data.
 */
class TPacketAgentsData : public IPacket
{
public:
	//! Agents data.
	vector<TPacketAgentData> agents;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_AGENTSDATA.
	 */
	virtual int getType () { return PACKET_AGENTSDATA; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
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
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		uint16_t len;
		if (!buf.fetch (len)) return false;
		for (int i = 0; i < len; i++)
		{
			TPacketAgentData d;
			if (!d.fromBuffer (buf)) return false;
			agents.push_back (d);
		}
		return true;
	}
};

/**
 *	\class TPacketKeyReply
 *	\brief Packet with authentication key.
 */
class TPacketKeyReply : public IPacket
{
public:
	//! Key.
	char key[16];

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_KEY_REPLY.
	 */
	virtual int getType () { return PACKET_KEY_REPLY; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, key);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, key)) return false;
		return true;
	}
};

/**
 *	\class TPacketConfig
 *	\brief Agent configuration.
 */
class TPacketConfig : public IPacket
{
public:
	/**
	 *	\struct TService
	 *	\brief Networ service data.
	 */
	struct TService
	{
		//! Service name.
		string name;
		//! Service transport type (TCP/UDP)
		bool tcp;
		//! Service port.
		uint16_t port;
	};
	//! Agent id.
	uint16_t agentId;
	//! Temperature path.
	string tempPath;
	//! Temperature divider.
	uint16_t tempDivider;
	//! Agent services.
	vector<TPacketConfig::TService> services;
	//! Time interval.
	uint16_t interval;
	//! Agent name.
	string name;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_CONFIG.
	 */
	virtual int getType () { return PACKET_CONFIG; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
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
		append (buf, name);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
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
		if (!fetch (buf, name)) return false;
		return true;
	}
};

/**
 *	\class TPacketConfigRequest
 *	\brief Requet for agent configuration.
 */
class TPacketConfigRequest : public IPacket
{
public:
	//! Requested agent id.
	uint16_t agentId;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_CONFIG_REQUEST.
	 */
	virtual int getType () { return PACKET_CONFIG_REQUEST; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, agentId);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, agentId)) return false;
		return true;
	}
};

/**
 *	\class TPacketStatsRequest
 *	\brief Stats request from agent.
 */
class TPacketStatsRequest : public IPacket
{
public:
	/**
	 *	\enum EType
	 *	\brief Statistic types based on origin.
	 */
	enum EType { CPU = 0, RAM, TEMP, DISK };
	//! Agent id.
	uint16_t agentId;
	//! Start date.
	uint32_t startDate;
	//! End date.
	uint32_t endDate;
	//! Number of points.
	uint16_t points;
	//! Stats type.
	EType type; // 1 byte
	//! Disc name.
	string diskName;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_STATS_REQUEST.
	 */
	virtual int getType () { return PACKET_STATS_REQUEST; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		// append (buf, agentId);
		// append (buf, startDate);
		// append (buf, endDate);
		// append (buf, points);
		// uint8_t t = (uint8_t)type;
		// append (buf, t);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
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

/**
 *	\class TPacketStatsReply
 *	\brief Reply for stats request.
 */
class TPacketStatsReply : public IPacket
{
public:
	//! Given points.
	vector<float> points;

	/**
	 *	\fn virtual int getType()
	 *	\brief Get packet type.
	 *	\return PACKET_STATS_REPLY.
	 */
	virtual int getType () { return PACKET_STATS_REPLY; }
	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		buf.append (points);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		return true;
	}
};

#endif
