#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdint.h>

#include <string>
using namespace std;

#include "packets.h"

class Server
{
public:
	Server ();
	~Server ();

	void setup (const string& host, int port);
	void process ();

private:
	enum EState { NotConnected, Connected };

	string m_host;
	int m_port;

	int m_fd;
	uint32_t m_lastConnect;
	EState m_state;

	void connect ();

	bool sendPacket (IPacket& packet);
	bool readPacket (int replyType, IPacket& p, int timeout);

	void processPacket (THeader& h, buffer_t& buf);
};

#endif
