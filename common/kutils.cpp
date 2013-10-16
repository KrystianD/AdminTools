#include "kutils.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef WIN32
#include <grp.h>
#endif

using namespace std;

uint32_t getTicks ()
{
	timeval tv;
	gettimeofday (&tv, 0);
	uint32_t val = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return val;
}
uint32_t getTicksUS ()
{
	timeval tv;
	gettimeofday (&tv, 0);
	uint32_t val = tv.tv_sec * 1000000 + tv.tv_usec;
	return val;
}

