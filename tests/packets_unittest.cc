#include "gtest/gtest.h"
#include "packets.h"

TEST(PacketsTest, TestTPacketAuth) {
	buffer_t buf;

	TPacketAuth p;
	p.sendConfig = 1;
	strncpy (p.key, "xxxxxxxxxxxxxxx", 16);

	p.toBuffer(buf);

	int chars[] = {0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x00,0x01};

	for(int i=0; i<sizeof(chars)/sizeof(chars[0]);i++) {
		ASSERT_EQ(buf[i], chars[i]);
	}
}

TEST(PacketsTest, TestTPacketReply) {
	buffer_t buf;

	TPacketReply p;
	p.value = 1;
	
	p.toBuffer(buf);

	int chars[] = {0x01,0x00,0x00,0x00};

	for(int i=0; i<sizeof(chars)/sizeof(chars[0]);i++) {
		ASSERT_EQ(buf[i], chars[i]);
	}
}

TEST(PacketsTest, TestTPacketAgentData) {
	buffer_t buf;

	TPacketAgentData p;

	p.id = 100;
	p.oldData = 200;
	p.name = "test";
	p.data.timestamp = 302192301;
	p.data.temp = 10.9;
	p.data.tempValid = true;
	p.data.cpuUsage = 80.9;
	p.data.totalRam = 102932100;
	p.data.freeRam = 102932100;
	p.data.uptime = 1000;

	TDiskUsage diskUsage;
	diskUsage.name = "dysk";
	diskUsage.totalSpace = 3029301;
	diskUsage.usedSpace = 30219;

	p.data.disksUsage.push_back(diskUsage);

	TService service;
	service.name = "ssh";
	service.available = true;

	p.data.services.push_back(service);
	
	p.toBuffer(buf);

	char chars[] = {0x64,0x00,0xc8,0x04,0x00,0x74,0x65,0x73,0x74,0xad,0x16,0x03,0x12,0x66,0x66,0x2e,0x41,0x01,0xcd,0xcc,0xa1,0x42,0x84,0x9e,0x22,0x06,0x00,0x00,0x00,0x00,0x84,0x9e,0x22,0x06,0x00,0x00,0x00,0x00,0xe8,0x03,0x00,0x00,0x01,0x00,0x04,0x00,0x64,0x79,0x73,0x6b,0x35,0x39,0x2e,0x00,0x00,0x00,0x00,0x00,0x0b,0x76,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x03,0x00,0x73,0x73,0x68,0x01};

	for(int i=0; i<sizeof(chars)/sizeof(chars[0]);i++) {
		ASSERT_EQ(buf[i], chars[i]);
	}

}

TEST(PacketsTest, TestTPacketKeyReply) {
	buffer_t buf;

	TPacketKeyReply p;

	strncpy (p.key, "xxxxxxxxxxxxxxx", 16);

	p.toBuffer(buf);

	char chars[] = {0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x78,0x00};

	for(int i=0; i<sizeof(chars)/sizeof(chars[0]);i++) {
		ASSERT_EQ(buf[i], chars[i]);
	}
}



TEST(PacketsTest, TestTPacketConfig) {
	buffer_t buf;

	TPacketConfig p;

	p.agentId = 32193;
	p.tempPath = "test";
	p.tempDivider = 10;

	TPacketConfig::TService service;
	service.name = "ssh";
	service.tcp = true;
	service.port = 22;

	p.services.push_back(service);
	p.interval = 100;

	p.toBuffer(buf);

	char chars[] = {0xffffffc1,0x7d,0x04,0x00,0x74,0x65,0x73,0x74,0x0a,0x00,0x01,0x00,0x03,0x00,0x73,0x73,0x68,0x01,0x16,0x00,0x64,0x00};

	for(int i=0; i<sizeof(chars)/sizeof(chars[0]);i++) {
		ASSERT_EQ(buf[i], chars[i]);
	}
}


TEST(PacketsTest, TestTPacketStatsReply) {
	buffer_t buf;

	TPacketStatsReply p;

	p.points.push_back(10.0);
	p.points.push_back(20.0);

	p.toBuffer(buf);

	char chars[] = {0x02,0x00,0x00,0x00,0x20,0x41,0x00,0x00,0xa0,0x41};

	for(int i=0; i<sizeof(chars)/sizeof(chars[0]);i++) {
		ASSERT_EQ(buf[i], chars[i]);
	}
}

