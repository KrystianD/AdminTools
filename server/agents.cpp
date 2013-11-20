#include "agents.h"

#include <algorithm>

vector<TAgentData> agentsData;

bool cmp (const TAgentData& a1, const TAgentData& a2)
{
	return a1.packet.name < a2.packet.name;
}

void assignData (TPacketAgentData& packet)
{
	for (int i = 0; i < agentsData.size (); i++)
	{
		if (agentsData[i].packet.id == packet.id)
		{
			agentsData[i].time = time (0);
			agentsData[i].packet = packet;
			return;
		}
	}
	TAgentData data;	
	data.time = time (0);
	data.packet = packet;
	agentsData.push_back (data);
	sort (agentsData.begin (), agentsData.end (), cmp);
}
