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

	/**
	 *	\fn static bool open (const string& path)
	 *	\brief Open file with database.
	 *	\param path Path pointing to database file.
	 *	\return If succeeded.
	 */
	static bool open (const string& path);

	/**
	 *	\fn static bool createTables ()
	 *	\brief Creates initial tables.
	 *	\return If succeeded.
	 */
	static bool createTables ();

	/**
	 *	\fn static bool generateNewKey (char key[16])
	 *	\brief Generate new key for agent.
	 *	\param[out] key Table with key.
	 *	\return If succeeded.
	 */
	static bool generateNewKey (char key[16]);

	/**
	 *	\fn static bool findAgentByKey (const char key[16], TDBAgent& agent)
	 *	\brief Find agent with given key.
	 *	\param[in] key Key to check.
	 *	\param[out] agent Agent data.
	 *	\return If succeeded.
	 */
	static bool findAgentByKey (const char key[16], TDBAgent& agent);

	/**
	 *	\fn static bool findAgentById (uint16_t id, TDBAgent& agent)
	 *	\brief Find agent with given id.
	 *	\param[in] id Agent id to check.
	 *	\param[out] agent Agent data.
	 *	\return If succeeded.
	 */
	static bool findAgentById (uint16_t id, TDBAgent& agent);

	/**
	 *	\fn static bool updateAgent (const TDBAgent& agent)
	 *	\brief Update agent data based on id inside.
	 *	\param[in] agent Agent data to update with existing agent id.
	 *	\return If succeeded.
	 */
	static bool updateAgent (const TDBAgent& agent);

	/**
	 *	\fn static bool insertRecord (int agentId, const TSensorsData& data)
	 *	\brief Insert data from sensors connected to agent with given id.
	 *	\param agentId Agent-owner of sensors data.
	 *	\param data Sensors data.
	 *	\return If succeeded.
	 */
	static bool insertRecord (int agentId, const TSensorsData& data);

	/**
	 *	\fn static bool insertRecord (const TDBAgent& agent, const TSensorsData& data)
	 *	\brief Insert data from sensors connected to given agent.
	 *	\param agent Agent-owner of sensors data.
	 *	\param data Sensors data.
	 *	\return If succeeded.
	 */
	static bool insertRecord (const TDBAgent& agent, const TSensorsData& data) { insertRecord (agent.id, data); }

	/**
	 *	\fn static bool insertRecords (int agentId, const vector<TSensorsData>& data)
	 *	\brief Isert many data from sensors connected to given agent id.
	 *	\param agentId Agent-owner of sensors datas.
	 *	\param data List of sensors datas.
	 *	\return If succeeded.
	 */
	static bool insertRecords (int agentId, const vector<TSensorsData>& data);

	/**
	 *	\fn static bool insertRecords (const TDBAgent& agent, const vector<TSensorsData>& data)
	 *	\brief Isert many data from sensors connected to given agent.
	 *	\param agent Agent-owner of sensors datas.
	 *	\param data List of sensors datas.
	 *	\return If succeeded.
	 */
	static bool insertRecords (const TDBAgent& agent, const vector<TSensorsData>& data) { insertRecords (agent.id, data); }

	/**
	 *	\fn static bool getRecords (int agentId, uint32_t startDate, uint32_t endDate, vector<TSensorsRecord>& records)
	 *	\brief Get sensors data for given agent id based on time interval.
	 *	\param[in] agentId Agent-owner of sensors data.
	 *	\param[in] startDate Interval start time.
	 *	\param[in] endDate Interval end time.
	 *	\param[out] records Filtered sensors datas.
	 */
	static bool getRecords (int agentId, uint32_t startDate, uint32_t endDate, vector<TSensorsRecord>& records);

	/**
	 *	\fn static bool cleanup ()
	 *	\brief Remove old records from database.
	 */
	static bool cleanup ();

private:
	/**
	 *	\fn static bool execute (const string& query)
	 *	\brief Execute given SQL query.
	 *	\param query Query to execute.
	 *	\return If succeeded.
	 */
	static bool execute (const string& query);

	/**
	 *	\fn static sqlite3_stmt* prepare (const string& query)
	 *	\brief Prepare SQLite statment based on given SQL query.
	 *	\param query Query to convert.
	 *	\return SQLite statement object connected with given query.
	 */
	static sqlite3_stmt* prepare (const string& query);

	/**
	 *	\fn static void fetchAgent (sqlite3_stmt* stm, TDBAgent& agent)
	 *	\brief Fetch from database agent data based on given SQLite statement.
	 *	\param[in] stm SQLite statement to execute.
	 *	\param[out] agent Agent DTO with fetched data.
	 *	\return None.
	 */
	static void fetchAgent (sqlite3_stmt* stm, TDBAgent& agent);
};

#endif
