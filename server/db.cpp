#include "db.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5.h"

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
		" temp TEXT NOT NULL DEFAULT('')"
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
bool DB::findAgent (const char key[16], TDBAgent* agent)
{
	string query = "SELECT id,name,services,temp FROM agents WHERE key=?";
	sqlite3_stmt *stm;
	int res = sqlite3_prepare_v2 (db, query.c_str (), query.size (), &stm, 0);
	string hash = md5 (string (key, 16));
	sqlite3_bind_text (stm, 1, hash.c_str (), hash.size (), 0);
	res = sqlite3_step (stm);
	if (res == SQLITE_ROW)
	{
		agent->id = sqlite3_column_int (stm, 1);

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
