#include "agents.h"

vector<TAgentData> agentsData;

void assignData (TAgentData& data)
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
