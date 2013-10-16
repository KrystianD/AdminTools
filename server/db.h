#ifndef __DB_H__
#define __DB_H__

#include <string>
using namespace std;

#include "sqlite3.h"

class DB
{
public:
	static sqlite3 *db;

	static bool open (const string& path);

	static bool createTables ();

// private:
	static bool execute (const string& query);
};

#endif
