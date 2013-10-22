#ifndef __DB_H__
#define __DB_H__

#include <stdint.h>

#include <string>
#include <vector>
using namespace std;

#include "sqlite3.h"

#include "sensors.h"

class TDBService
{
public:
	string name;
	int port;
};
class TDBAgent
{
public:
	int id;
	string name;
	vector<TDBService> services;
	string tempPath;
	int tempDivider;
	int interval;
};

class DB
{
public:
	static sqlite3 *db;

	static bool open (const string& path);

	static bool createTables ();

	static bool generateNewKey (char key[16]);
	static bool findAgentByKey (const char key[16], TDBAgent& agent);
	static bool findAgentById (uint16_t id, TDBAgent& agent);
	static bool insertRecord (const TDBAgent& agent, const TSensorsData& data);

// private:
	static bool execute (const string& query);
	static sqlite3_stmt* prepare (const string& query);

	static void fetchAgent (sqlite3_stmt* stm, TDBAgent& agent);
};

#endif
