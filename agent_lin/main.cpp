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
	if (!c.fromFile ("config"))
	{
		printf ("Unable to read config file\r\n");
		return 1;
	}

	signal (SIGPIPE, SIG_IGN);
	
	Server serv;
	serv.setup (c.getString ("host"), c.getInt ("port"));

	uint32_t lastSendTime = getTicks ();

	for (;;)
	{
		serv.process ();

		usleep (10000);

		TSensorsData d;
		getSensorsData (d, serv.getConfig ());

		if (serv.isValid () && getTicks () - lastSendTime >= serv.getConfig ().interval)
		{
			TPacketAgentData agentData;
			agentData.id = 1;
			agentData.data = d;
			agentData.data.timestamp = time (0);

			serv.sendPacket (agentData);

			lastSendTime = getTicks ();
		}
	}

	return 0;
}
