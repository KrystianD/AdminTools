#include "agents.h"

#include <algorithm>

vector<TAgentData> agentsData;

bool cmp (const TAgentData& a1, const TAgentData& a2)
{
	const char* orders[] = { "KD" };

	int o1 = -1, o2 = -1;
	for (int i = 0; i < sizeof (orders) / sizeof (orders[0]); i++)
	{
		if (a1.packet.name.substr (0, strlen (orders[i])) == string (orders[i]))
			o1 = i;
		if (a2.packet.name.substr (0, strlen (orders[i])) == string (orders[i]))
			o2 = i;
	}

	if (o1 == o2)
		return a1.packet.name < a2.packet.name;
	else if (o1 != -1 && o2 == -1)
		return true;
	else if (o1 == -1 && o2 != -1)
		return false;
	else
		return o1 < o2;
}

void fixPacket (TPacketAgentData& packet)
{
	vector<TDiskUsage>& d = packet.data.disksUsage;
	for (int i = d.size () - 1; i >= 0; i--)
	{
		for (int j = i - 1; j >= 0; j--)
		{
			if (d[i].name == d[j].name)
			{
				d.erase (d.begin () + i);
				break;
			}
		}
	}
}

void assignData (TPacketAgentData& packet)
{
	for (int i = 0; i < agentsData.size (); i++)
	{
		if (agentsData[i].packet.id == packet.id)
		{
			agentsData[i].time = time (0);
			agentsData[i].packet = packet;
			sort (agentsData.begin (), agentsData.end (), cmp);
			return;
		}
	}
	TAgentData data;	
	data.time = time (0);
	data.packet = packet;
	agentsData.push_back (data);
	sort (agentsData.begin (), agentsData.end (), cmp);
}
