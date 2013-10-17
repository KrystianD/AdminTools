#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <stdint.h>

#include <string>
#include <vector>
using namespace std;

#include "common.h"

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
	virtual void fromBuffer (buffer_t& buf)
	{
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
	virtual void fromBuffer (buffer_t& buf)
	{
	}
};
class TSensorsData : public ISerializable
{
public:
	float temp;
	bool tempValid;
	float cpuUsage;
	uint64_t totalRam, freeRam;
	uint32_t uptime;
	vector<TDiskUsage> disksUsage;
	vector<TService> services;

	virtual void toBuffer (buffer_t& buf)
	{
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
	virtual void fromBuffer (buffer_t& buf)
	{
		m_pos = 0;
		fetch (buf, temp);
		fetch (buf, tempValid);
		fetch (buf, cpuUsage);
		fetch (buf, totalRam);
		fetch (buf, freeRam);
		fetch (buf, uptime);

		uint16_t len;
		fetch (buf, len);
		disksUsage.clear ();
		while (len--)
		{
			TDiskUsage d;
			fetch (buf, d.name);
			fetch (buf, d.totalSpace);
			fetch (buf, d.usedSpace);
			disksUsage.push_back (d);
		}

		fetch (buf, len);
		services.clear ();
		while (len--)
		{
			TService s;
			fetch (buf, s.name);
			fetch (buf, s.available);
			services.push_back (s);
		}
	}
};

void getSensorsData (TSensorsData& data);

#endif
