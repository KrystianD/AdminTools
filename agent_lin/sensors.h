#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <stdint.h>

class TSensorsData
{
public:
	double temp;
	bool tempValid;
	uint32_t totalRam, freeRam;
};

void getSensorsData (TSensorsData& data);

#endif
