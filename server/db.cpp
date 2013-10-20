#include "db.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5.h"
#include "kutils.h"

#include <vector>
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
	// const char* str = 
		// "CREATE TABLE agents ("
		// " id INTEGER PRIMARY KEY AUTOINCREMENT,"
		// " key TEXT NOT NULL,"
		// " name TEXT NOT NULL);";

	// execute (str);

	str = 
		"CREATE TABLE agents ("
		" id INTEGER PRIMARY KEY AUTOINCREMENT,"
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
	{
		key[i] = rand () % ('Z' - 'A' + 1) + 'A';
	}

	string query = "INSERT INTO agents(key) VALUES (?);";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);
	string hash = md5 (string (key, 16));
	sqlite3_bind_text (stm, 1, hash.c_str (), hash.size (), 0);
	sqlite3_step (stm);
	sqlite3_finalize (stm);

	printf ("New generated key: %.*s\r\n", 16, key);
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
}bool DB::findAgentById (uint16_t id, TDBAgent& agent)
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

bool DB::execute (const string& query)
{
	int res;

	sqlite3_stmt *stm;
	const char *tail;
	res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, &tail);
	printf ("prepare: %d\r\n", res);
	printf ("%s %d\r\n", tail, strlen (tail));
	res = sqlite3_step (stm);
	printf ("step: %d\r\n", res);
	sqlite3_finalize (stm);
	return res == SQLITE_DONE;
}
sqlite3_stmt* DB::prepare (const string& query)
{
	sqlite3_stmt *stm;
	const char *tail;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, &tail);
	printf ("prepare: %d\r\n", res);
	printf ("%s %d\r\n", tail, strlen (tail));
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
		TDBService srv;
		srv.name = parts2[0];
		srv.port = atoi (parts2[1].c_str ());
		agent.services.push_back (srv);

		printf ("n %s p %d\r\n", srv.name.c_str(), srv.port);
	}

	tmp = (const char*)sqlite3_column_text (stm, 3);
	parts = explode (tmp, ":");
	agent.tempPath = "";
	if (parts.size () == 2)
	{
		agent.tempPath = parts[0];
		agent.tempDivider = atoi (parts[1].c_str ());
	}

	agent.interval = sqlite3_column_int (stm, 4);

	printf ("%s %s %d %d\r\n", agent.name.c_str(), agent.tempPath.c_str(), agent.tempDivider, agent.interval);
}
