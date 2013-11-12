#include "serverWin.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sstream>

#include "kutils.h"
#include "packets.h"
#include "settings.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

Server::Server ()
{
	m_state = NotConnected;
	m_lastConnect = 0;
	m_configChanged = false;
}
Server::~Server ()
{
}


void Server::setup (const string& host, int port, const string& key)
{
	m_host = host;
	m_port = port;
	m_key = key;
}
void Server::process ()
{

}

void Server::connect ()
{
     WSADATA wsaData;

}

bool Server::sendHeader (int type)
{
	THeader h;
	h.type = type;
	h.size = 0;
	//send (m_fd, &h, sizeof (h), 0);
}
bool Server::sendPacket (IPacket& packet)
{
	buffer_t b;
	packet.toBuffer (b);
	THeader h;
	h.type = packet.getType ();
	h.size = b.size ();
	//send (m_fd, &h, sizeof (h), 0);
	//send (m_fd, &b[0], b.size (), 0);
}
bool Server::readPacket (int replyType, IPacket& p, int timeout)
{

}

void Server::processPacket (THeader& h, buffer_t& buf)
{

}
