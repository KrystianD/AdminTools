#include "agents.h"

vector<TAgentData> agentsData;

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
}
