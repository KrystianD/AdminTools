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


#include "configWin.h"

#include <stdio.h>
#include <string.h>
#include <sstream>

Config::Config ()
{
}
Config::~Config ()
{
}

bool Config::fromFile (const string& path)
{
	FILE *f = fopen (path.c_str (), "rt");

	if (!f)
		return false;
	
	while (!feof (f))
	{
		char str[200];
		fscanf (f, "%s", str);

		char* pos = strchr (str, '=');
		if (pos == 0)
			continue;
		*pos = 0;

		char value[200];
		strcpy (value, pos + 1);

		m_data[str] = value;
	}

	fclose (f);

	return true;
}
bool Config::saveToFile (const string& path)
{
	map<string,string>::iterator it;

	string bckPath = path + ".bck";

	FILE *f = fopen (bckPath.c_str (), "wt");

	if (!f)
		return false;

	for (it = m_data.begin (); it != m_data.end (); it++)
	{
		fprintf (f, "%s=%s\r\n", it->first.c_str (), it->second.c_str ());
	}
	fclose (f);

	remove(path.c_str ());
	rename (bckPath.c_str (), path.c_str ());

	return true;
}

bool Config::hasKey (const string& key)
{
	return m_data.find (key) != m_data.end ();
}
string Config::getString (const string& key, const string& def)
{
	map<string,string>::iterator it = m_data.find (key);
	if (it == m_data.end ())
		return def;
	return it->second;
}
int Config::getInt (const string& key, int def)
{
	map<string,string>::iterator it = m_data.find (key);
	if (it == m_data.end ())
		return def;
	stringstream ss;
	ss << it->second;
	int val;
	ss >> val;
	return val;
}
void Config::setString (const string& key, const string& value)
{
	m_data[key] = value;
}
void Config::setInt (const string& key, int val)
{
	stringstream ss;
	ss << val;
	m_data[key] = ss.str ();
}
