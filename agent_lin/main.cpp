#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "sensors.h"
#include "server.h"
#include "config.h"
#include "packets.h"
#include "kutils.h"

bool end = false;

void signal_handler (int signum)
{
	end = true;
}

int main (int argc, char** argv)
{
	const char *configPath = "/etc/config_agent.cfg";
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

	signal (SIGPIPE, SIG_IGN);
	signal (SIGINT, signal_handler);
	signal (SIGHUP, signal_handler);
	signal (SIGQUIT, signal_handler);
	signal (SIGTERM, signal_handler);

	// load config file
	Config c;
	c.fromFile (configPath);
	
	Server serv;
	serv.setup (c.getString ("host"), c.getInt ("port"), c.getString ("key"));

	TPacketConfig& cfg = serv.getConfig ();
	cfg.tempPath = c.getString ("tempPath", "");
	cfg.tempDivider = c.getInt ("tempDivider", 1);
	
	int srvCnt = c.getInt ("services", 0);
	cfg.services.clear ();
	for (int i = 0; i < srvCnt; i++)
	{
		TPacketConfig::TService s;
		char key[20];
		sprintf (key, "srv%d", i);
		vector<string> parts = explode (c.getString (key, ""), ":");
		if (parts.size () == 3)
		{
			s.name = parts[0];
			s.tcp = atoi (parts[1].c_str ());
			s.port = atoi (parts[2].c_str ());
			cfg.services.push_back (s);
		}
	}
	cfg.interval = c.getInt ("interval", 2000);

	uint32_t lastSendTime = getTicks (), lastOldSendTime = getTicks ();
	vector<TPacketAgentData> oldSensorsData;

	// load old sensors data
	FILE *f = fopen ("olddata", "rb");
	if (f)
	{
		uint32_t cnt;
		fread (&cnt, sizeof(cnt), 1, f);
		for (int i = 0; i < cnt; i++)
		{
			uint32_t size;
			fread (&size, sizeof(size), 1, f);

			buffer_t buf;
			buf.resize (size);
			fread (buf.data (), size, 1, f);

			TPacketAgentData p;
			p.fromBuffer (buf);
			oldSensorsData.push_back (p);
		}
		fclose (f);
	}

	int i = 0;
	while (!end)
	{
		serv.process ();

		usleep (10000);

		TSensorsData d;

		// gather data from senosors and send it to server or save in memory depending on connection state
		if (getTicks () - lastSendTime >= serv.getConfig ().interval)
		{
			getSensorsData (d, serv.getConfig ());

			TPacketAgentData agentData;
			agentData.id = 0;
			agentData.data = d;
			agentData.data.timestamp = time (0);

			if (serv.isValid ())
			{
				agentData.oldData = 0;
				if (serv.sendPacket (agentData))
					printf ("Packet sent (%d)\r\n", i++);
				else
					printf ("Unable to send packet\r\n");
			}
			else
			{
				oldSensorsData.push_back (agentData);
				printf ("Packet saved\r\n");
			}

			lastSendTime = getTicks ();
		}
		// send old sensors data in periods of 10ms
		if (oldSensorsData.size () > 0 && getTicks () - lastOldSendTime >= 10 && serv.isValid ())
		{
			TPacketAgentData p = oldSensorsData[0];
			p.oldData = 1;
			serv.sendPacket (p);
			oldSensorsData.erase (oldSensorsData.begin ());
			printf ("Sent old packet, %u left\n", oldSensorsData.size ());
			lastOldSendTime = getTicks ();
		}

		// config file update
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

	// save old data in order to send it to server when connected
	f = fopen ("olddata", "wb");

	buffer_t buf;
	uint32_t cnt = oldSensorsData.size ();
	printf ("Saving data (%d to save)...\n", cnt);
	buf.append (cnt);
	fwrite (buf.data (), buf.size (), 1, f);
	for (int i = 0; i < oldSensorsData.size (); i++)
	{
		buffer_t buf2;
		oldSensorsData[i].toBuffer (buf2);
		uint32_t size = buf2.size ();

		fwrite (&size, sizeof(size), 1, f);
		fwrite (buf2.data (), buf2.size (), 1, f);
	}
	printf ("Data saved\n");

	fclose (f);

	return 0;
}
