#include "serverWin.h"

#include <sstream>

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
	if (m_state == NotConnected) {
		
	} else if (m_state == WaitingForConfig) {
		
	}
	if (m_state == Connected || m_state == WaitingForConfig) {
		
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

}

bool Server::sendHeader (int type) {
	THeader h;
	h.type = type;
	h.size = 0;
	//send (m_fd, &h, sizeof (h), 0);
	return true;
}

bool Server::sendPacket (IPacket& packet) {
	buffer_t b;
	packet.toBuffer (b);
	THeader h;
	h.type = packet.getType ();
	h.size = b.size ();
	//send (m_fd, &h, sizeof (h), 0);
	//send (m_fd, &b[0], b.size (), 0);
	return true;
}

bool Server::readPacket (int replyType, IPacket& p, int timeout) {
	return true;
}

void Server::processPacket (THeader& h, buffer_t& buf) {

}
