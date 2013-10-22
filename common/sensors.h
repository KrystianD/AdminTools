#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <stdint.h>

#include <string>
#include <vector>
using namespace std;

#include "common.h"

class TPacketConfig;

class TDiskUsage : public ISerializable
{
public:
	string name;
	uint64_t totalSpace, usedSpace;

	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, name);
		append (buf, totalSpace);
		append (buf, usedSpace);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		return true;
	}
};
class TService : public ISerializable
{
public:
	string name;
	bool available;

	virtual void toBuffer (buffer_t& buf)
	{
		append (buf, name);
		append (buf, available);
	}
	virtual bool fromBuffer (buffer_t& buf)
	{
		return true;
	}
};
class TSensorsData : public ISerializable
{
public:
	uint32_t timestamp;
	float temp;
	bool tempValid;
	float cpuUsage;
	uint64_t totalRam, freeRam;
	uint32_t uptime;
	vector<TDiskUsage> disksUsage;
	vector<TService> services;

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
	virtual bool fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		if (!fetch (buf, timestamp)) return false;
		if (!fetch (buf, temp)) return false;
		if (!fetch (buf, tempValid)) return false;
		if (!fetch (buf, cpuUsage)) return false;
		if (!fetch (buf, totalRam)) return false;
		if (!fetch (buf, freeRam)) return false;
		if (!fetch (buf, uptime)) return false;

		uint16_t len;
		if (!fetch (buf, len)) return false;
		disksUsage.clear ();
		while (len--)
		{
			TDiskUsage d;
			if (!fetch (buf, d.name)) return false;
			if (!fetch (buf, d.totalSpace)) return false;
			if (!fetch (buf, d.usedSpace)) return false;
			disksUsage.push_back (d);
		}

		if (!fetch (buf, len)) return false;
		services.clear ();
		while (len--)
		{
			TService s;
			if (!fetch (buf, s.name)) return false;
			if (!fetch (buf, s.available)) return false;
			services.push_back (s);
		}
		return true;
	}
};

void getSensorsData (TSensorsData& data, const TPacketConfig& config);

#endif
