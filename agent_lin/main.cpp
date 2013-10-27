#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "sensors.h"
#include "server.h"
#include "config.h"
#include "packets.h"
#include "kutils.h"

int main (int argc, char** argv)
{
	const char *configPath = "config.cfg";
	int cc;
	opterr = 0;
	while ((cc = getopt (argc, argv, "c:")) != -1)
	{
		switch (cc)
		{
		case 'c':
			configPath = optarg;
			break;
		default:
			break;
		}
	}

	printf ("using config: %s\r\n", configPath);

	if (!fileExists (configPath))
	{
		printf ("No config file\r\n");
		return 1;
	}
	if (!fileAccessible (configPath))
	{
		printf ("Config file not writable\r\n");
		return 1;
	}

	Config c;
	c.fromFile (configPath);

	signal (SIGPIPE, SIG_IGN);
	
	Server serv;
	serv.setup (c.getString ("host"), c.getInt ("port"), c.getString ("key"));

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

		if (serv.configChanged ())
		{
			const TPacketConfig& cfg = serv.getConfig ();
			c.setString ("tempPath", cfg.tempPath);
			c.setInt ("tempDivider", cfg.tempDivider);
			c.setInt ("services", cfg.services.size ());
			for (int i = 0; i < cfg.services.size (); i++)
			{
				const TPacketConfig::TService s = cfg.services[i];
				char key[20], val[100];
				sprintf (key, "srv%d", i);
				sprintf (val, "%s:%d:%d", s.name.c_str (), s.tcp, s.port);
				c.setString (key, val);
			}
			c.setInt ("interval", cfg.interval);
			c.saveToFile (configPath);
			serv.configApplied ();
		}
	}

	return 0;
}
