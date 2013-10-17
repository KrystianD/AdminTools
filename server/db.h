#ifndef __DB_H__
#define __DB_H__

#include <string>
#include <vector>
using namespace std;

#include "sqlite3.h"

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
	int divider;
};

class DB
{
public:
	static sqlite3 *db;

	static bool open (const string& path);

	static bool createTables ();

	static bool generateNewKey (char key[16]);
	static bool findAgent (const char key[16], TDBAgent* agent);

// private:
	static bool execute (const string& query);
	static sqlite3_stmt* prepare (const string& query);
};

#endif
