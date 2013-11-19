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
