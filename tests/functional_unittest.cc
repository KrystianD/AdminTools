#include "gtest/gtest.h"
#include "server.h"
#include "key.h"

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
int m_fd;
string m_host = "127.0.0.1";
int m_port = 5555;
string m_key = KEY;
bool sendPacket (IPacket& packet)
{
	buffer_t b;
	packet.toBuffer (b);
	THeader h;
	h.type = packet.getType ();
	h.size = b.size ();
	send (m_fd, &h, sizeof (h), 0);
	send (m_fd, &b[0], b.size (), 0);
}
bool readPacket (int replyType, IPacket& p, int timeout)
{
	THeader h;
	int rd = recvall (m_fd, &h, sizeof (h), 1000);
	if (rd == 0)
	{
		close (m_fd);
		// m_state = NotConnected;
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
			// m_state = NotConnected;
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

TEST(FunctionalTest, TestVALIDKEY) {


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
		perror ("connect ");
		return;
	}

	TPacketAuth p;
	strncpy (p.key, "xxxxxxxxxxxxxxxxxx", 16);
	strncpy (p.key, KEY, 16);
	p.sendConfig = 0;

	sendPacket (p);

	TPacketReply r;

	if (!readPacket (PACKET_REPLY, r, 1000))
	{
		printf ("unable to read reply packet\r\n");
	}
	else
	{
		printf ("ok  reply:%d!!\n",r.value);
		ASSERT_EQ(r.value,1);
	}

	close (m_fd);
}

TEST(FunctionalTest, TestWRONGKEY) {


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
		perror ("connect ");
		return;
	}

	TPacketAuth p;
	strncpy (p.key, "xxxxxxxxxxxxxxxxxx", 16);
	p.sendConfig = 0;

	sendPacket (p);

	TPacketReply r;

	if (!readPacket (PACKET_REPLY, r, 1000))
	{
		printf ("unable to read reply packet\r\n");
	}
	else
	{
		printf ("ok  reply:%d!!\n",r.value);
		ASSERT_EQ(r.value,0);
	}

	close (m_fd);
}
