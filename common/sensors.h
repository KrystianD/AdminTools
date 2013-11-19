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

#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <stdint.h>

#include <string>
#include <vector>
using namespace std;

#include "common.h"

class TPacketConfig;

/**
 *	\class TDiskUsage
 *	\brief Information about disc usage.
 */
class TDiskUsage : public ISerializable
{
public:
	//! Disc name.
	string name;
	//! Total available space on disc.
	uint64_t totalSpace;
	//! Used space on disc.
	uint64_t usedSpace;

	/**
	 *	\fn virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, name);
		append (buf, totalSpace);
		append (buf, usedSpace);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		return true;
	}
};

/**
 *	\class TService
 *	\brief Information about service.
 */
class TService : public ISerializable
{
public:
	//! Service name.
	string name;
	//! Service availability.
	bool available;

	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, name);
		append (buf, available);
	}
	/**
	 *	\fn virtual bool fromBuffer(buffer_t& buf)
	 *	\brief Fill object with buffer data.
	 *	\param[in] buf Buffer data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf)
	{
		return true;
	}
};

/**
 *	\class TSensorsData
 *	\brief Data from agent sensors.
 */
class TSensorsData : public ISerializable
{
public:
	//! Time of data acquisition.
	uint32_t timestamp;
	//! Cpu temperature.
	float temp;
	//! Cpu temperature correctness.
	bool tempValid;
	//! Cpu percent usage.
	float cpuUsage;
	//! Available RAM.
	uint64_t totalRam;
	//! Not used RAM.
	uint64_t freeRam;
	//! System uptime.
	uint32_t uptime;
	//! List of discs usage.
	vector<TDiskUsage> disksUsage;
	//! List of running services.
	vector<TService> services;

	/**
	 *	\fn	virtual void toBuffer(buffer_t& buf)
	 *	\brief Get byte buffer representation.
	 *	\param[out] buf Target buffer.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, timestamp);
		append (buf, temp);
		append (buf, tempValid);
		append (buf, cpuUsage);
		append (buf, totalRam);
		append (buf, freeRam);
		append (buf, uptime);

		uint16_t len = disksUsage.size ();
		append (buf, len);
		for (int i = 0; i < len; i++)
		{
			buffer_t tmp;
			disksUsage[i].toBuffer (tmp);
			buf.insert (buf.end (), tmp.begin (), tmp.end ());
		}

		len = services.size ();
		append (buf, len);
		for (int i = 0; i < len; i++)
		{
			buffer_t tmp;
			services[i].toBuffer (tmp);
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
		if (!buf.fetch (timestamp)) return false;
		if (!buf.fetch (temp)) return false;
		if (!buf.fetch (tempValid)) return false;
		if (!buf.fetch (cpuUsage)) return false;
		if (!buf.fetch (totalRam)) return false;
		if (!buf.fetch (freeRam)) return false;
		if (!buf.fetch (uptime)) return false;

		uint16_t len;
		if (!buf.fetch (len)) return false;
		disksUsage.clear ();
		while (len--)
		{
			TDiskUsage d;
			if (!buf.fetch (d.name)) return false;
			if (!buf.fetch (d.totalSpace)) return false;
			if (!buf.fetch (d.usedSpace)) return false;
			disksUsage.push_back (d);
		}

		if (!buf.fetch (len)) return false;
		services.clear ();
		while (len--)
		{
			TService s;
			if (!buf.fetch (s.name)) return false;
			if (!buf.fetch (s.available)) return false;
			services.push_back (s);
		}
		return true;
	}
};

/**
 *	\fn void getSensorsData (TSensorsData& data, const TPacketConfig& config)
 *	\brief Reads data from sensors.
 *	\param[out] data Data from sensors.
 *	\param[in] config Current configuration.
 *	\return None.
 */
void getSensorsData (TSensorsData& data, const TPacketConfig& config);

#endif
