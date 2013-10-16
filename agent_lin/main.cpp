#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "sensors.h"
#include "server.h"
#include "config.h"
#include "packets.h"
#include "kutils.h"

int main ()
{
	Config c;
	c.fromFile ("config");

	printf ("%d\r\n", c.getInt ("port"));

	signal (SIGPIPE, SIG_IGN);
	
	Server serv;
	serv.setup (c.getString ("host"), c.getInt ("port"));

	uint32_t lastSendTime = getTicks ();

	for (;;)
	{
		serv.process ();

		usleep (10000);

		TSensorsData d;
		getSensorsData (d);

		if (serv.isValid () && getTicks () - lastSendTime >= 1000)
		{
			TAgentData agentData;
			agentData.temp = d.temp;

			serv.sendPacket (agentData);

			lastSendTime = getTicks ();
		}
	}

	return 0;
}
