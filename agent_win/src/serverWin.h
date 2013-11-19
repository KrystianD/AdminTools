/*
Copyright (c) 2013, AGH University of Science and Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

/**
 *	\namespace WinAgent
 *	\brief Windows agent module.
 */
namespace WinAgent
{
/**
 *	\class Server
 *	\brief Represents server connected to.
 */
class Server
{
public:
	/**
	 *	\fn Server()
	 *	\brief Constructor, zeroes data.
	 */
	Server ();
	/**
	 *	\fn ~Server()
	 *	\brief Destructor, do nothing.
	 */
	~Server ();

	/**
	 *	\fn void setup(const string& host, int port, const string& key)
	 *	\brief Set server connection data and authentication key.
	 *	\param host Host address.
	 *	\param port Host port.
	 *	\param key Authentication key.
	 *	\return None.
	 */
	void setup (const string& host, int port, const string& key);
	/**
	 *	\fn	void process()
	 *	\brief Process connection events.
	 *	\return None.
	 */
	void process ();
	/**
	 *	\fn bool isValid()
	 *	\brief Check if connection is valid (agent connected to server).
	 *	\return If connected.
	 */
	bool isValid () { return m_state == Connected; }
	/**
	 *	\fn bool configChanged()
	 *	\brief Check if configuration change is needed.
	 *	\return If change needed.
	 */
	bool configChanged () { return m_configChanged; }
	/**
	 *	\fn void configApplied()
	 *	\brief Set configuration change flag if new config applied.
	 *	\return None.
	 */
	void configApplied () { m_configChanged = false; }
	/**
	 *	\fn bool sendPacket(IPacket& packet)
	 *	\brief Send given packet to server.
	 *	\param packet Packet to send.
	 *	\return If succeeded.
	 */
	bool sendPacket (IPacket& packet);

	/**
	 *	\fn void connectServer()
	 *	\brief Connect to server.
	 *	\return None.
	 */
	void connectServer ();
	/**
	 *	\fn TPacketConfig& getConfig()
	 *	\brief Get current agent-server configuration.
	 *	\return Current configuration.
	 */
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