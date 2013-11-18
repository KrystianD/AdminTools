#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdint.h>

#include <string>
using namespace std;

#include "db.h"
#include "packets.h"
#include "kutils.h"

/**
 *	\class Client
 *	\brief Represents handled client-agent.
 */
class Client
{
public:
	//! Client id, mostly socket descriptor used for its communication.
	int fd;
	//! Client ip address.
	string ip;
	//! Client port.
	int port;
	//! If client is not used.
	bool toDelete;
	//! If client settings changed.
	bool settingsChanged;

	/**
	 *	\fn Client (int fd, const string& ip, int port)
	 *	\brief Constructor, initialize data with incoming connection returns.
	 *	\param fd Socket descriptor.
	 *	\param ip Client ip address.
	 *	\param port Client port.
	 */
	Client (int fd, const string& ip, int port);

	/**
	 *	\fn void readData ()
	 *	\brief Read data from client-agent.
	 *	\return None.
	 */
	void readData ();
	/**
	 *	\fn void process ()
	 *	\brief Process all waiting request.
	 *	\return None.
	 */
	void process ();
	/**
	 *	\fn void fetchConfig ()
	 *	\brief Fetch client-agent current configuration.
	 *	\return None.
	 */
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

	// old data
	uint32_t oldDataUpdateTime;
	vector<TSensorsData> oldData;

	void processPacket (int size);
	bool sendPacket (IPacket& packet, int type = -1);
	void kill ();
	void sendConfig ();
	void agentToConfig (const TDBAgent& dAgent, TPacketConfig& config);
	void configToAgent (const TPacketConfig& config, TDBAgent& dbAgent);
	bool generateAndSendStats (const TPacketStatsRequest& req);
};

#endif
