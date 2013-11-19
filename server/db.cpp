#include "db.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5.h"
#include "kutils.h"

#include <vector>
#include <sstream>
using namespace std;

sqlite3 *DB::db;

bool DB::open (const string& path)
{
	int res = sqlite3_open (path.c_str (), &db);

	if (res != SQLITE_OK)
		return false;

	return true;
}

bool DB::createTables ()
{
	const char* str;
	
	str = 
		"CREATE TABLE records ("
		" id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
		" agentId INTEGER NOT NULL,"
		" date DATETIME NOT NULL,"
		" temp FLOAT NOT NULL,"
		" cpu FLOAT NOT NULL,"
		" ram FLOAT NOT NULL,"
		" diskUsages TEXT NOT NULL"
	 	");";	

	if (!execute (str))
		return false;

	str = 
		"CREATE TABLE agents ("
		" id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
		" key TEXT NOT NULL DEFAULT(''),"
		" name TEXT NOT NULL DEFAULT(''),"
		" services TEXT NOT NULL DEFAULT(''),"
		" temp TEXT NOT NULL DEFAULT(''),"
		" interval INT NOT NULL DEFAULT(2000)"
		");";

	if (!execute (str))
		return false;

	return true;
}

bool DB::generateNewKey (char key[16])
{
	for (int i = 0; i < 16; i++)
		key[i] = rand () % ('Z' - 'A' + 1) + 'A';

	string query = "INSERT INTO agents(key) VALUES (?);";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);
	string hash = md5 (string (key, 16));
	sqlite3_bind_text (stm, 1, hash.c_str (), hash.size (), 0);
	sqlite3_step (stm);
	sqlite3_finalize (stm);

	return true;
}
bool DB::findAgentByKey (const char key[16], TDBAgent& agent)
{
	string query = "SELECT id,name,services,temp,interval FROM agents WHERE key=?";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);
	string hash = md5 (string (key, 16));
	sqlite3_bind_text (stm, 1, hash.c_str (), hash.size (), 0);
	res = sqlite3_step (stm);
	if (res == SQLITE_ROW)
	{
		fetchAgent (stm, agent);
		sqlite3_finalize (stm);
		return true;
	}
	else if (res == SQLITE_DONE)
	{
		sqlite3_finalize (stm);
		return false;
	}
	else
	{
		sqlite3_finalize (stm);
		return false;
	}
}
bool DB::findAgentById (uint16_t id, TDBAgent& agent)
{
	string query = "SELECT id,name,services,temp,interval FROM agents WHERE id=?";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);
	sqlite3_bind_int (stm, 1, id);
	res = sqlite3_step (stm);
	if (res == SQLITE_ROW)
	{
		fetchAgent (stm, agent);
		sqlite3_finalize (stm);
		return true;
	}
	else if (res == SQLITE_DONE)
	{
		sqlite3_finalize (stm);
		return false;
	}
	else
	{
		sqlite3_finalize (stm);
		return false;
	}
}
bool DB::updateAgent (const TDBAgent& agent)
{
	string query = "UPDATE agents SET name=?, services=?, temp=?, interval=? WHERE id=?";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);

	string srvStr = "";
	for (int i = 0; i < agent.services.size (); i++)
	{
		const TDBService& s = agent.services[i];
	
		if (i > 0)
			srvStr += ",";

		char buf[100];
		sprintf (buf, "%s:%d:%d", s.name.c_str (), s.tcp, s.port);
		srvStr += buf;
	}

	sqlite3_bind_text (stm, 1, agent.name.c_str (), agent.name.size (), 0);
	sqlite3_bind_text (stm, 2, srvStr.c_str (), srvStr.size (), 0);

	char buf[100];
	sprintf (buf, "%s:%d", agent.tempPath.c_str (), agent.tempDivider);
	sqlite3_bind_text (stm, 3, buf, -1, 0);

	sqlite3_bind_int (stm, 4, agent.interval);
	sqlite3_bind_int (stm, 5, agent.id);

	res = sqlite3_step (stm);
	if (res == SQLITE_DONE)
	{
		sqlite3_finalize (stm);
		return true;
	}
	else
	{
		sqlite3_finalize (stm);
		return false;
	}
}
bool DB::insertRecord (int agentId, const TSensorsData& data)
{
	string query = "INSERT INTO records(agentId,date,temp,cpu,ram,diskUsages) VALUES(?,?,?,?,?,?);";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);
	sqlite3_bind_int (stm, 1, agentId);
	sqlite3_bind_int (stm, 2, data.timestamp);
	sqlite3_bind_double (stm, 3, data.temp);
	sqlite3_bind_double (stm, 4, data.cpuUsage);
	sqlite3_bind_double (stm, 5, (double)data.freeRam / (double)data.totalRam);

	string disk = "";

	stringstream ss;
	ss.precision (1);
	for (int i = 0; i < data.disksUsage.size (); i++)
	{
		const TDiskUsage& d = data.disksUsage[i];
		if (i > 0)
			ss << ",";
		ss << d.name << ":" << ((double)d.usedSpace / (double)d.totalSpace);
	}
	disk = ss.str ();

	sqlite3_bind_text (stm, 6, disk.c_str (), disk.size (), 0);

	// uint32_t s = getTicks ();
	res = sqlite3_step (stm);
	// uint32_t e = getTicks ();
	// printf ("time: %d\n", e-s);
	if (res == SQLITE_DONE)
	{
		sqlite3_finalize (stm);
		return true;
	}
	else
	{
		sqlite3_finalize (stm);
		return false;
	}
}
bool DB::insertRecords (int agentId, const vector<TSensorsData>& data)
{
	uint32_t s = getTicks ();
	if (!execute ("BEGIN TRANSACTION"))
		return false;
	
	for (int i = 0; i < data.size (); i++)
	{
		if (!DB::insertRecord (agentId, data[i]))
		{
			execute ("ROLLBACK TRANSACTION");
			return false;
		}
	}

	if (!execute ("COMMIT TRANSACTION"))
		return false;
	uint32_t e = getTicks ();
	fprintf (stderr, "time all: %d\n", e-s);
	return true;
}
bool DB::getRecords (int agentId, uint32_t startDate, uint32_t endDate, vector<TSensorsRecord>& records)
{
	string query = "SELECT id,date,temp,cpu,ram,diskUsages FROM records WHERE agentId=? AND date>=? AND date<=? ORDER BY date";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);
	sqlite3_bind_int (stm, 1, agentId);
	sqlite3_bind_int (stm, 2, startDate);
	sqlite3_bind_int (stm, 3, endDate);

	fprintf (stderr, "ag: %d s: %d e: %d\n", agentId, startDate, endDate);
	res = sqlite3_step (stm);
	while (res == SQLITE_ROW)
	{
		TSensorsRecord r;
		r.id = sqlite3_column_int (stm, 0);
		r.timestamp = sqlite3_column_int (stm, 1);
		r.temp = sqlite3_column_double (stm, 2);
		r.cpuUsage = sqlite3_column_double (stm, 3);
		r.ramUsage = sqlite3_column_double (stm, 4);
		
		string str = (const char*)sqlite3_column_text (stm, 5); 

		vector<string> parts = explode (str, ",");
		for (int i = 0; i < parts.size (); i++)
		{
			vector<string> parts2 = explode (parts[i], ":");
			if (parts2.size () == 2)
			{
				TSensorsRecord::TDisk d;
				d.name = parts2[0];
				d.usage = atof (parts2[1].c_str ());
				r.disks.push_back (d);
			}
		}

		records.push_back (r);
		res = sqlite3_step (stm);
	}
	fprintf (stderr, "%d\n", records.size ());
	if (res == SQLITE_DONE)
	{
		sqlite3_finalize (stm);
		return true;
	}
	else
	{
		sqlite3_finalize (stm);
		return false;
	}
}
bool DB::cleanup ()
{
	execute ("DELETE FROM records WHERE date<strftime('%s','now')-60*60*24*30");
}
void DB::close ()
{
	sqlite3_close (db);
}

bool DB::execute (const string& query)
{
	int res;

	sqlite3_stmt *stm;
	const char *tail;
	res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, &tail);
	res = sqlite3_step (stm);
	sqlite3_finalize (stm);
	return res == SQLITE_DONE;
}
sqlite3_stmt* DB::prepare (const string& query)
{
	sqlite3_stmt *stm;
	const char *tail;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, &tail);
	return stm;
}

void DB::fetchAgent (sqlite3_stmt* stm, TDBAgent& agent)
{
	agent.id = sqlite3_column_int (stm, 0);
	agent.name = (const char*)sqlite3_column_text (stm, 1); 
	
	string tmp = (const char*)sqlite3_column_text (stm, 2);
	vector<string> parts = explode (tmp, ",");
	agent.services.clear ();
	for (int i = 0; i < parts.size (); i++)
	{
		vector<string> parts2 = explode (parts[i], ":");
		if (parts2.size () == 3)
		{
			TDBService srv;
			srv.name = parts2[0];
			srv.tcp = atoi (parts2[1].c_str ());
			srv.port = atoi (parts2[2].c_str ());
			agent.services.push_back (srv);
		}
	}

	tmp = (const char*)sqlite3_column_text (stm, 3);
	parts = explode (tmp, ":");
	agent.tempPath = "";
	agent.tempDivider = 1;
	if (parts.size () == 2)
	{
		agent.tempPath = parts[0];
		agent.tempDivider = atoi (parts[1].c_str ());
	}

	agent.interval = sqlite3_column_int (stm, 4);
}
