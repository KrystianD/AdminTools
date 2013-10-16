#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "sensors.h"
#include "server.h"
#include "config.h"

int main ()
{
	Config c;
	c.fromFile ("config");

	printf ("%d\r\n", c.getInt ("port"));

	
	Server serv;
	serv.setup (c.getString ("host"), c.getInt ("port"));

	for (;;)
	{
		serv.process ();

		usleep (100000);

		TSensorsData d;
		getSensorsData (d);
	}

	return 0;
}
