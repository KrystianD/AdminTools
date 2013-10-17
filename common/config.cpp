#include "config.h"

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
