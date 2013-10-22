#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdint.h>

#include <string>
using namespace std;

#include "db.h"
#include "packets.h"
#include "kutils.h"

class Client
{
public:
	int fd;
	string ip;
	int port;
	bool toDelete;
	bool settingsChanged;

	Client (int fd, const string& ip, int port);

	void readData ();
	void process ();
	void fetchConfig ();

private:
	enum EState { WAITING_FOR_HEADER, WAITING_FOR_PACKET };

	THeader currentHeader;
	uint8_t buffer[1024 * 1024];
	int bufferPointer;
	// int dataPointer;
	int dataToReceive;
	EState state;
	buffer_t dataToSend;

	uint32_t packetStartTime;
	uint32_t lastPingTime;

	bool authorized;

	// client
	bool sendingActive;
	Timer sendDataTimer;

	TDBAgent dbAgent;

	void processPacket (int size);
	bool sendPacket (IPacket& packet);
	void kill ();
	void sendConfig ();
};

#endif
