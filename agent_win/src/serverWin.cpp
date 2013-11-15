#include "serverWin.h"

#include <sstream>
#include <iostream>
#include "commonWin.h"

#pragma comment(lib, "Ws2_32.lib")


Server::Server() {
	m_state = NotConnected;
	m_lastConnect = 0;
	m_configChanged = false;

	ConnectSocket = INVALID_SOCKET;
}
Server::~Server() {}

void Server::setup (const string& host, int port, const string& key) {
	m_host = host;
	m_port = port;
	m_key = key;
}

void Server::process() {
	if (m_state == NotConnected) 
	{
		if (getTicks () - m_lastConnect >= 500)
		{
			connectServer ();
			m_lastConnect = getTicks ();
		}
	} 
	else if (m_state == WaitingForConfig) 
	{
		if (getTicks () > m_configTime)
		{
			printf ("No config within timeout\r\n");
			closesocket (ConnectSocket);
			WSACleanup();
			m_state = NotConnected;
		}
	}
	if (m_state == Connected || m_state == WaitingForConfig) 
	{
		timeval tv;
		fd_set fds;

		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		//o select na windzie
		//http://msdn.microsoft.com/en-us/library/windows/desktop/ms740141(v=vs.85).aspx
		FD_ZERO(&fds);
		FD_SET(ConnectSocket, &fds);
		int res = select (0,&fds,0,0,&tv); //deskryptor = 0, wg info z linka wy¿ej, pierwszy argument jest ignorowany
		if(res == -1)
		{
			std::cerr << "Select error\n" ;
			return;
		}
		else
		{
			if(FD_ISSET(ConnectSocket, &fds))
			{
				THeader h;
				int rd = recvallWin(ConnectSocket, &h, sizeof(h), 1000);
				if(rd == 0)
				{
					closesocket(ConnectSocket);
					WSACleanup();
					m_state = NotConnected;
					return;
				}

				if(h.size > 0)
				{
					buffer_t buf;
					buf.resize(h.size);
					rd = recvallWin(ConnectSocket, &buf[0], h.size, 1000);
					if(rd == 0)
					{
						closesocket(ConnectSocket);
						WSACleanup();
						m_state = NotConnected;
						return;
					}
					processPacket(h,buf);
				}
			}
		}
	}
}



void Server::connectServer() {
    
	int iResult;
	WSADATA wsaData;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	} else {
		printf("WSAStartup success.\n");
	}

	struct addrinfo *result, *ptr;
	struct addrinfo hints;

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::stringstream port;
	string portStr;
	port << m_port;
	port >> portStr;

	iResult = getaddrinfo(m_host.c_str(), portStr.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
	}	

	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}

	
	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	TPacketAuth p;
	p.sendConfig = 1;
	strncpy (p.key, m_key.c_str (), 16);

	sendPacket (p);

	TPacketReply r;
	if (!readPacket (PACKET_REPLY, r, 1000))
	{
		closesocket(ConnectSocket);
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

bool Server::sendHeader (int type) {
	THeader h;
	h.type = type;
	h.size = 0;
	THeader* tHPtr = &h;
	send (ConnectSocket,  reinterpret_cast<char*>(tHPtr), sizeof (h), 0);//uwaga, do ogarniêcia przes³anie struktury uint16 i uint32 (choc wydaje mi siê ze tak jest ok)
	return true;
}

bool Server::sendPacket (IPacket& packet) {
	buffer_t b;
	packet.toBuffer (b); 

	THeader h;

	h.type = packet.getType ();
	h.size = b.size ();
	THeader* tHPtr = &h;
	send (ConnectSocket, reinterpret_cast<char*>(tHPtr), sizeof (h), 0); 
	send (ConnectSocket, &b[0], b.size(), 0);
	return true;
}

bool Server::readPacket (int replyType, IPacket& p, int timeout) 
{
	THeader h;
	int rd = recvallWin(ConnectSocket, &h, sizeof(h), 1000);
	if(rd == 0)
	{
		closesocket(ConnectSocket);
		WSACleanup();
		m_state = NotConnected;
		return false;
	}

	if(h.size > 0)
	{
		buffer_t buf;
		buf.resize (h.size);
		rd = recvallWin (ConnectSocket, &buf[0], h.size, 1000);
		if(rd == 0)
		{
			closesocket(ConnectSocket);
			WSACleanup();
			m_state = NotConnected;
			return false;
		}

		if(h.type == replyType)
		{
			p.fromBuffer(buf);
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
		m_config.fromBuffer(buf);
		m_configChanged = true;
		if(m_state == WaitingForConfig)
			m_state = Connected;
		printf("Config received\n");
		break;
	}
}
