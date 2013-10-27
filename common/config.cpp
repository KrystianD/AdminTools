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
