#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdint.h>

#include <string>
using namespace std;

#include "packets.h"

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
	 *	\fn TPacketConfig& getConfig()
	 *	\brief Get current agent-server configuration.
	 *	\return Current configuration.
	 */
	TPacketConfig& getConfig () { return m_config; }

	bool readPacket (int replyType, IPacket& p, int timeout);

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

	void processPacket (THeader& h, buffer_t& buf);
};

#endif
