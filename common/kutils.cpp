/*
Copyright (c) 2013, AGH University of Science and Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
vector<string> explode (const string& str, const string& delim)
{
  vector<string> parts;
  size_t idx = 0, delimIdx;

  delimIdx = str.find (delim, idx);
  if (delimIdx == string::npos) { parts.push_back (str); return parts; }
  do
  {
    string part = str.substr (idx, delimIdx - idx);
    parts.push_back (part);
    idx = delimIdx + delim.size ();
    delimIdx = str.find (delim, idx);
  } while (delimIdx != string::npos && idx < str.size ());

  if (idx < str.size ())
  {
    string part = str.substr (idx);
    parts.push_back (part);
  }

  return parts;
}
std::string trim (const std::string& str)
{
	string str2 = str;
	str2.erase (0, str2.find_first_not_of (" \r\n"));
	str2.erase (str2.find_last_not_of (" \r\n") + 1);
	return str2;
}
