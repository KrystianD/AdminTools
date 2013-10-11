#ifndef __PACKETS_H__
#define __PACKETS_H__

#define PACKET_AGENTDATA 0
#define PACKET_START     1
#define PACKET_STOP      2

#pragma pack(1)
struct THeader
{
	char key[0];
	uint8_t type, size;
};

class TAgentData
{
	
};
#pragma pack()

#endif
