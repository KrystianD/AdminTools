#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdint.h>

#include <string>
using namespace std;

#include "packets.h"
// #include "db.h"

class Server
{
public:
	Server ();
	~Server ();

	void setup (const string& host, int port, const string& key);
	void process ();

	bool isValid () { return m_state == Connected; }
	bool configChanged () { return m_configChanged; }
	void configApplied () { m_configChanged = false; }
	bool sendPacket (IPacket& packet);

	TPacketConfig& getConfig () { return m_config; }

private:
	enum EState { NotConnected, WaitingForConfig, Connected };

	string m_host, m_key;
	int m_port;

	int m_fd;
	uint32_t m_lastConnect;
	EState m_state;
	uint32_t m_configTime;

	TPacketConfig m_config;
	bool m_configChanged;

	void connect ();

	bool sendHeader (int type);
	bool readPacket (int replyType, IPacket& p, int timeout);

	void processPacket (THeader& h, buffer_t& buf);
};

#endif
