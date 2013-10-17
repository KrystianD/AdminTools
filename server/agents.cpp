#include "agents.h"

vector<TPacketAgentData> agentsData;

void assignData (TPacketAgentData& data)
{
	for (int i = 0; i < agentsData.size (); i++)
	{
		if (agentsData[i].id == data.id)
		{
			agentsData[i] = data;
			return;
		}
	}
	agentsData.push_back (data);
}
