#include "serverWin.h"

#include <windows.h>

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
	return true;
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
	return true;
}
bool Server::readPacket (int replyType, IPacket& p, int timeout)
{
	return true;
}

void Server::processPacket (THeader& h, buffer_t& buf)
{

}
