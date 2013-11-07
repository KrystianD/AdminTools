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
	bool tcp;
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
struct TSensorsRecord
{
	struct TDisk
	{
		string name;
		double usage;
	};
	int id;
	uint32_t timestamp;
	double temp, cpuUsage, ramUsage;
	vector<TDisk> disks;
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
	static bool updateAgent (const TDBAgent& agent);
	static bool insertRecord (int agentId, const TSensorsData& data);
	static bool insertRecord (const TDBAgent& agent, const TSensorsData& data) { insertRecord (agent.id, data); }
	static bool insertRecords (int agentId, const vector<TSensorsData>& data);
	static bool insertRecords (const TDBAgent& agent, const vector<TSensorsData>& data) { insertRecords (agent.id, data); }
	static bool getRecords (int agentId, uint32_t startDate, uint32_t endDate, vector<TSensorsRecord>& records);

// private:
	static bool execute (const string& query);
	static sqlite3_stmt* prepare (const string& query);

	static void fetchAgent (sqlite3_stmt* stm, TDBAgent& agent);
};

#endif
