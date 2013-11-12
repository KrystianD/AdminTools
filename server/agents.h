#ifndef __AGENTS_H__
#define __AGENTS_H__

#include <vector>
using namespace std;

#include "packets.h"

/**
 *	\class TAgentData
 *	\brief Agent data from one moment.
 */
class TAgentData
{
public:
	//! Data time.
	uint32_t time;
	//! Agent data packet.
	TPacketAgentData packet;
};

//! Global list of agents data.
extern vector<TAgentData> agentsData;

/**
 *	\fn	void assignData (TPacketAgentData& data)
 *	\brief Update or assign new agent data to global list.
 *	\param[in] data Data to assign.
 *	\return None.
 */
void assignData (TPacketAgentData& data);

#endif
