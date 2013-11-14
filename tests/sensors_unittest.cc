#include "../common/sensors.h"
#include "gtest/gtest.h"
#include "packets.h"
#include <stdlib.h>
#include <stdio.h>

TEST(SensorsTest, TestSensorsDataTypes) {
	TSensorsData data;
	TPacketConfig config;
	
	getSensorsData (data, config);
	
	ASSERT_EQ(typeid(data.tempValid), typeid(bool));
	ASSERT_EQ(typeid(data.temp), typeid(float));
	ASSERT_EQ(typeid(data.cpuUsage), typeid(float));
	ASSERT_EQ(typeid(data.totalRam), typeid(uint64_t));
	ASSERT_EQ(typeid(data.freeRam), typeid(uint64_t));
	ASSERT_EQ(typeid(data.uptime), typeid(uint32_t));
}

TEST(SensorsTest, TestValuesGreaterThanZero) {
	TSensorsData data;
	TPacketConfig config;
	
	getSensorsData (data, config);

	ASSERT_GT(data.cpuUsage, 0);
	ASSERT_GT(data.totalRam, 0);
	ASSERT_GT(data.freeRam, 0);
	ASSERT_GT(data.uptime, 0);
}