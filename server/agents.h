#ifndef __AGENTS_H__
#define __AGENTS_H__

#include <vector>
using namespace std;

#include "packets.h"

class TAgentData
{
public:
	uint32_t time;
	TPacketAgentData packet;
};

extern vector<TAgentData> agentsData;

void assignData (TPacketAgentData& data);

#endif
