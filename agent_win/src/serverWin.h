#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
using namespace std;

#include "../../common/packets.h"
#include "commonWin.h"
#include "kutilsWin.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "windows.h"
#include "settingsWin.h"

using namespace WinAgent;

namespace WinAgent
{

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

	
	void connectServer ();
	TPacketConfig& getConfig () { return m_config; }

private:
	enum EState { NotConnected, WaitingForConfig, Connected };

	string m_host, m_key;
	int m_port;

	SOCKET ConnectSocket;

	uint32_t m_lastConnect;
	EState m_state;
	uint32_t m_configTime;

	TPacketConfig m_config;
	bool m_configChanged;

	bool sendHeader (int type);
	bool readPacket (int replyType, IPacket& p, int timeout);

	void processPacket (THeader& h, buffer_t& buf);
};

}
#endif