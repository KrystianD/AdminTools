#ifndef __AGENTS_H__
#define __AGENTS_H__

#include <vector>
using namespace std;

#include "packets.h"

extern vector<TPacketAgentData> agentsData;

void assignData (TPacketAgentData& data);

#endif
