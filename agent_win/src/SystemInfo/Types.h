#pragma once

#include <sstream>

// don't define this on linux :)
#define WIN32
#include "sigar/sigar.h"

typedef unsigned short int uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;

template<typename Out, typename In>
Out lexical_cast(In arg)
{
	std::stringstream sstream;
	Out result;
	sstream << arg;
	sstream >> result;
	return result;
}