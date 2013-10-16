#include "db.h"

#include <stdio.h>
#include <string.h>

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
	char* str = 
		"CREATE TABLE agents ("
		" id INTEGER PRIMARY KEY AUTOINCREMENT,"
		" key TEXT NOT NULL,"
		" name TEXT NOT NULL);";

	execute (str);
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
