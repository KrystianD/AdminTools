#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdint.h>

#include <string>
using namespace std;

#include "packets.h"

class Client
{
public:
	int fd;
	string ip;
	int port;
	bool toDelete;

	Client (int fd, const string& ip, int port);

	void readData ();
	void process ();

private:
	enum EState { WAITING_FOR_HEADER, WAITING_FOR_PACKET };

	THeader currentHeader;
	uint8_t buffer[1024];
	int bufferPointer;
	int dataPointer;
	int dataToReceive;
	EState state;

	void processPacket (int size);
	
	// packet processing
	template<typename T>
	bool getVal (T& val);
};

#endif
