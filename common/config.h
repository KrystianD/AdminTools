#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <map>
using namespace std;

class Config
{
public:
	Config ();
	~Config ();

	bool fromFile (const string& path);
	bool saveToFile (const string& path);

	bool hasKey (const string& key);
	string getString (const string& key, const string& def = "");
	int getInt (const string& key, int def = 0);

private:
	map<string,string> m_data;
};

#endif
