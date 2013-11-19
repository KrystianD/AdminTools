#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <cmath>

// net
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
using namespace std;

#include "client.h"
#include "config.h"
#include "db.h"

vector<Client> clients;

bool end = false;

void signal_handler (int signum)
{
	end = true;
}

int main (int argc, char** argv)
{
	srand (time (0));

	const char *configPath = "/etc/config_server.cfg";
	bool generateKey = false;
	int c;
	opterr = 0;
	while ((c = getopt (argc, argv, "c:k")) != -1)
	{
		switch (c)
		{
		case 'c':
			configPath = optarg;
			break;
		case 'k':
			generateKey = true;
			break;
		default:
			break;
		}
	}

	fprintf (stderr, "using config: %s\r\n", configPath);

	if (!fileExists (configPath))
	{
		fprintf (stderr, "No config file\r\n");
		return 1;
	}
	if (!fileAccessible (configPath))
	{
		fprintf (stderr, "Config file not writable\r\n");
		return 1;
	}

	Config cfg;
	cfg.fromFile (configPath);

	string dbPath = cfg.getString ("db", "data.db");

	fprintf (stderr, "using database: %s\r\n", dbPath.c_str ());

	if (!fileExists (dbPath))
	{
		fprintf (stderr, "No database file, creating...\r\n");
		if (!DB::open (dbPath))
		{
			fprintf (stderr, "Unable to open database\r\n");
			return 1;
		}
		if (!DB::createTables ())
		{
			unlink (dbPath.c_str ());
			fprintf (stderr, "Unable to create database\r\n");
			return 1;
		}
	}
	else
	{
		if (!fileAccessible (dbPath))
		{
			fprintf (stderr, "Database not writable\r\n");
			return 1;
		}
		if (!DB::open (dbPath))
		{
			fprintf (stderr, "Unable to open database\r\n");
			return 1;
		}
	}

	if (generateKey)
	{
		char key[16];
		if (DB::generateNewKey (key))
		{
			printf ("%.*s\n", 16, key);
			return 0;
		}
	}

	signal (SIGPIPE, SIG_IGN);
	signal (SIGINT, signal_handler);
	signal (SIGHUP, signal_handler);
	signal (SIGQUIT, signal_handler);
	signal (SIGTERM, signal_handler);

	sockaddr_in myaddr;

	memset (&myaddr, 0, sizeof (myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons (cfg.getInt ("port", 1234));
	myaddr.sin_addr.s_addr = INADDR_ANY;

	// create server
	int serverFd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverFd == -1)
	{
		perror ("socket");
		return 1;
	}

	int yes = 1;
	setsockopt (serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int));

	// bind server to port
	if (bind (serverFd, (sockaddr*)&myaddr, sizeof (myaddr)) == -1)
	{
		perror ("bind");
		return 1;
	}

	if (listen (serverFd, 5) == -1)
	{
		perror ("listen");
		return 1;
	}

	fprintf (stderr, "ok\r\n");

	uint32_t lastCleanup = getTicks ();

	while (!end)
	{
		timeval tv;
		fd_set fds;

		tv.tv_sec = 0;
		tv.tv_usec = 10000;

		FD_ZERO(&fds);
		FD_SET(serverFd, &fds);
		int maxfd = serverFd;
		for (int i = 0; i < clients.size (); i++)
		{
			FD_SET(clients[i].fd, &fds);
			if (clients[i].fd > maxfd)
				maxfd = clients[i].fd;
		}

		// checking for new incoming connections
		int res = select (maxfd + 1, &fds, 0, 0, &tv);
		if (res == -1)
		{
			if (errno != EINTR)
			{
				perror ("select");
				return 1;
			}
		}
		else
		{
			if (FD_ISSET(serverFd, &fds))
			{
				// new incoming connection
				fprintf (stderr, "newcon\r\n");
				sockaddr_in clientaddr;
				socklen_t addrsize = sizeof (clientaddr);
				int clientFd = accept (serverFd, (sockaddr*)&clientaddr, &addrsize);
				if (res == -1)
				{
					perror ("accept");
					return 1;
				}

				char str[INET_ADDRSTRLEN];
				inet_ntop (AF_INET, &clientaddr.sin_addr, str, INET_ADDRSTRLEN);

				Client c (clientFd, str, ntohs (clientaddr.sin_port));
				clients.push_back (c);
			}
	
			for (int i = 0; i < clients.size (); i++)
			{
				if (FD_ISSET(clients[i].fd, &fds))
				{
					clients[i].readData ();
				}
			}
		}
		bool needSettingsReload = false;
		for (int i = clients.size () - 1; i >= 0; i--)
		{
			clients[i].process ();
			if (clients[i].settingsChanged)
			{
				needSettingsReload = true;
				clients[i].settingsChanged = false;
			}
		}
		for (int i = clients.size () - 1; i >= 0; i--)
		{
			if (needSettingsReload)
				clients[i].fetchConfig ();
		}
		for (int i = clients.size () - 1; i >= 0; i--)
		{
			if (clients[i].toDelete)
			{
				close (clients[i].fd);
				clients.erase (clients.begin () + i);
				fprintf (stderr, "Client deleted\r\n");
			}
		}

		if (getTicks () - lastCleanup > 60 * 60 * 1000)
		{
			DB::cleanup ();
			lastCleanup = getTicks ();
			fprintf (stderr, "DB cleanup\r\n");
		}
	}
	fprintf (stderr, "Disconnect clients\r\n");
	for (int i = clients.size () - 1; i >= 0; i--)
	{
		close (clients[i].fd);
		fprintf (stderr, "Client deleted\r\n");
	}
	DB::close ();
	fprintf (stderr, "Database closed\r\n");

	return 0;
}
