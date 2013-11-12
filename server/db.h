#ifndef __DB_H__
#define __DB_H__

#include <stdint.h>

#include <string>
#include <vector>
using namespace std;

#include "sqlite3.h"

#include "sensors.h"

/**
 *	\class TDBService
 *	\brief Service DTO for database use.
 */
class TDBService
{
public:
	//! Service name.
	string name;
	//! Service tranport type (TCP/UDP).
	bool tcp;
	//! Service port number.
	int port;
};

/**
 *	\class TDBAgent
 *	\brief Client-agent DTO for database use.
 */
class TDBAgent
{
public:
	//! Agent id.
	int id;
	//! Agent name.
	string name;
	//! List of agent services.
	vector<TDBService> services;
	//! Temperature path.
	string tempPath;
	//! Temperature divider.
	int tempDivider;
	//! Time interval.
	int interval;
};

/**
 *	\class TSensorsRecord
 *	\brief Client-agent sensors DTO for database use.
 */
struct TSensorsRecord
{
	/**
	 *	\struct TDisk
	 *	\brief Simple disc data.
	 */
	struct TDisk
	{
		//! Disc name.
		string name;
		//! Percent disc usage.
		double usage;
	};
	//! Agent id.
	int id;
	//! Time of sensors read.
	uint32_t timestamp;
	//! Temperature value.
	double temp;
	//! Cpu percent usage.
	double cpuUsage;
	//! RAM percent usage.
	double ramUsage;
	//! List of discs data.
	vector<TDisk> disks;
};

/**
 *	\class DB
 *	\brief Database handler. Uses SQLite.
 */
class DB
{
public:
	//! Current database handle.
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
