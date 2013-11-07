#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// net
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sstream>

#include "kutils.h"
#include "packets.h"
#include "settings.h"

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
	if (m_state == NotConnected)
	{
		if (getTicks () - m_lastConnect >= 500)
		{
			connect ();
			m_lastConnect = getTicks ();
		}
	}
	else if (m_state == WaitingForConfig)
	{
		if (getTicks () > m_configTime)
		{
			printf ("No config within timeout\r\n");
			close (m_fd);
			m_state = NotConnected;
		}
	}

	if (m_state == Connected || m_state == WaitingForConfig)
	{
		timeval tv;
		fd_set fds;

		tv.tv_sec = 0;
		tv.tv_usec = 10000;

		FD_ZERO(&fds);
		FD_SET(m_fd, &fds);

		int res = select (m_fd + 1, &fds, 0, 0, &tv);
		if (res == -1)
		{
			perror ("select");
			return;
		}
		else
		{
			if (FD_ISSET(m_fd, &fds))
			{
				THeader h;
				int rd = recvall (m_fd, &h, sizeof (h), 1000);
				if (rd == 0)
				{
					close (m_fd);
					m_state = NotConnected;
					return;
				}
	
				if (h.size > 0)
				{
					buffer_t buf;
					buf.resize (h.size);
					rd = recvall (m_fd, &buf[0], h.size, 1000);
					if (rd == 0)
					{
						close (m_fd);
						m_state = NotConnected;
						return;
					}

					processPacket (h, buf);
				}
			}
		}
	}
}

void Server::connect ()
{
	sockaddr_in servaddr;

	m_fd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_fd == -1)
	{
		perror ("socket");
		return;
	}

	int yes = 1;
	setsockopt (m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int));

	memset (&servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (1234);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	stringstream port;
	string portStr;
	port << m_port;
	port >> portStr;
	if (status = getaddrinfo (m_host.c_str (), portStr.c_str (), &hints, &servinfo))
	{
		close (m_fd);
		fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (status));
		return;
	}

	char str[100];
	inet_ntop (servinfo->ai_family, &((sockaddr_in*)servinfo->ai_addr)->sin_addr, str, 100);
	printf ("%s\r\n", str);

	freeaddrinfo (servinfo);

	if (::connect (m_fd, servinfo->ai_addr, servinfo->ai_addrlen))
	{
		close (m_fd);
		perror ("connect");
		return;
	}

	TPacketAuth p;
	p.sendConfig = 1;
	strncpy (p.key, m_key.c_str (), 16);

	sendPacket (p);

	TPacketReply r;
	if (!readPacket (PACKET_REPLY, r, 1000))
	{
		close (m_fd);
		printf ("unable to read reply packet\r\n");
		return;
	}

	printf ("Auth reply: %s\r\n", r.value == 1 ? "Access granted" : "Access denied");

	if (r.value == 1)
	{
		m_state = WaitingForConfig;
		m_configTime = getTicks () + CONFIG_TIMEOUT;
	}
}

bool Server::sendHeader (int type)
{
	THeader h;
	h.type = type;
	h.size = 0;
	send (m_fd, &h, sizeof (h), 0);
}
bool Server::sendPacket (IPacket& packet)
{
	buffer_t b;
	packet.toBuffer (b);
	THeader h;
	h.type = packet.getType ();
	h.size = b.size ();
	send (m_fd, &h, sizeof (h), 0);
	send (m_fd, &b[0], b.size (), 0);
}
bool Server::readPacket (int replyType, IPacket& p, int timeout)
{
	THeader h;
	int rd = recvall (m_fd, &h, sizeof (h), 1000);
	if (rd == 0)
	{
		close (m_fd);
		m_state = NotConnected;
		return false;
	}

	if (h.size > 0)
	{
		buffer_t buf;
		buf.resize (h.size);
		rd = recvall (m_fd, &buf[0], h.size, 1000);
		if (rd == 0)
		{
			close (m_fd);
			m_state = NotConnected;
			return false;
		}

		if (h.type == replyType)
		{
			p.fromBuffer (buf);
			return true;
		}
	}

	return false;
}

void Server::processPacket (THeader& h, buffer_t& buf)
{
	switch (h.type)
	{
	case PACKET_CONFIG:
		{
			m_config.fromBuffer (buf);
			m_configChanged = true;
			if (m_state == WaitingForConfig)
				m_state = Connected;

			printf ("Config received\r\n");
		}
		break;
	}
}
