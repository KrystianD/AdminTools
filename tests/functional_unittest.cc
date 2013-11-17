#include "gtest/gtest.h"
#include "server.h"

TEST(FunctionalTest, Test) {
	Server serv;

	serv.setup ("127.0.0.1", 1234, "xxxxxxxxxxxxxxx");
	serv.process();

	TPacketAuth p;
	strncpy (p.key, "xxxxxxxxxxxxxxx", 16);
	p.sendConfig = 100;

	serv.sendPacket (p);
}