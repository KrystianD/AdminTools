#include "sensors.h"

#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>

// net
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sstream>
using namespace std;

#include "packets.h"
#include "kutils.h"

string readFile (const string& path)
{
	FILE *f = fopen (path.c_str (), "rt");
	if (!f)
		return "";
	int r;
	string val = "";
	do
	{
		char buf[500];
		r = fread (buf, 1, 500, f);
		// printf ("r %d\r\n", r);
		val += string (buf, r);
	} while (r);
	fclose (f);
	return val;
}

inline std::string trim(const std::string& str)
{
	string str2 = str;
	str2.erase(0, str2.find_first_not_of(" \r\n"));       //prefixing spaces
	str2.erase(str2.find_last_not_of(" \r\n")+1);         //surfixing spaces
	return str2;
}


void getSensorsData (TSensorsData& data, const TPacketConfig& config)
{
	char path[500];

	data.tempValid = false;
	if (config.tempPath.size () > 0)
	{				
		string s = readFile (config.tempPath);
		if (s.size () != 0)
		{
			int temp;
			stringstream ss;
			ss << s;
			ss >> temp;

			data.temp = (float)temp / (float)config.tempDivider;
			data.tempValid = true;
		}
	}
	else
	{
		for (int i = 0; i < 64; i++)
		{
			sprintf (path, "/sys/class/hwmon/hwmon%d/device/name", i);
			string s = trim (readFile (path));
			if (s.size () == 0)
				continue;

			// printf ("t |%s|\r\n", s.c_str());
			int hwmonIdx = i;
			if (s == "coretemp")
			{
				double tempAvg = 0;
				int cnt = 0;
				for (i = 0; i < 16; i++)
				{
					sprintf (path, "/sys/class/hwmon/hwmon%d/device/temp%d_input", hwmonIdx, i);
					string s = readFile (path);
					if (s.size () != 0)
					{
						int temp;
						stringstream ss;
						ss << s;
						ss >> temp;

						tempAvg += (float)temp / 1000.0f;
						cnt++;
					}
				}
				tempAvg /= cnt;
				data.temp = tempAvg;
				data.tempValid = true;
				break;
			}
		}
	}

	struct sysinfo info;
	sysinfo (&info);

	data.uptime = info.uptime;
	data.freeRam = info.freeram + info.bufferram;
	data.totalRam = info.totalram;
	data.cpuUsage = info.loads[0] / 65536.0f;

	FILE *f = fopen ("/proc/mounts", "rt");
	if (!f)
		return;
	
	while (!feof (f))
	{
		char buf[500];
		char dev[100];
		char mnt[100];
		char* p = fgets (buf, 500, f);
		if (!p)
			break;

		sscanf (buf, "%s %s", dev, mnt);
		if (strncmp (dev, "/dev", 4) == 0)
		{
			struct statvfs stat;
			statvfs (mnt, &stat);

			uint64_t blksize, blocks, freeblks;
			blksize = stat.f_bsize;
			blocks = stat.f_blocks;
			freeblks = stat.f_bfree;

			TDiskUsage d;
			d.name = dev;
			d.totalSpace = blocks * blksize;
			d.usedSpace = (blocks - freeblks) * blksize;
			data.disksUsage.push_back (d);
		}
	}
	fclose (f);

	for (int i = 0; i < config.services.size (); i++)
	{
		const TPacketConfig::TService& s = config.services[i];

		if (s.tcp)
		{
			sockaddr_in servaddr;

			int fd;
			fd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (fd == -1)
			{
				perror ("socket");
				return;
			}

			memset (&servaddr, 0, sizeof (servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_port = htons (0);
			servaddr.sin_addr.s_addr = INADDR_ANY;

			struct addrinfo hints;
			struct addrinfo *servinfo;

			memset (&hints, 0, sizeof (hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;

			stringstream port;
			string portStr;
			port << s.port;
			port >> portStr;
			getaddrinfo ("127.0.0.1", portStr.c_str (), &hints, &servinfo);
			
			TService service;
			service.name = s.name;
			if (::connect (fd, servinfo->ai_addr, servinfo->ai_addrlen))
				service.available = 0;
			else
				service.available = 1;
			freeaddrinfo (servinfo);

			// printf ("%d %d\r\n", s.tcp, service.available);
			data.services.push_back (service);
			close (fd);
		}
		else
		{
			string dataStr = readFile ("/proc/net/udp");
			TService service;
			service.name = s.name;
			service.available = 0;
			// printf ("%s\r\n", data.c_str());
			vector<string> lines = explode (dataStr, "\n");
			for (int i = 1; i < lines.size (); i++)
			{
				// printf ("%s\r\n", lines[i].c_str());

				int d;
				char local_addr[64];
				int local_port;

				sscanf (lines[i].c_str (),
						"%d: %64[0-9A-Fa-f]:%X",
						&d, local_addr, &local_port);

				if (local_port == s.port)
					service.available = 1;
			}
			data.services.push_back (service);
		}
	}


	// printf ("%s\r\n", s.c_str ());

	// for (int i = 'a'; i <= 'z'; i++)
	// {
		// sprintf (path, "/sys/block/sd%c", i);
		// if (access (path, F_OK) != -1)
		// {
			// // printf ("a\r\n");
			// for (int j = 1; ; j++)
			// {
				// sprintf (path, "/sys/block/sd%c/sd%c%d", i, i, j);
				
				// if (access (path, F_OK) != -1)
				// {
					// sprintf (path, "/dev/sd%c%d", i, j);
					// printf ("%s\t ", path);

					// struct statvfs stat;
					// statvfs (path, &stat);

					// unsigned long blksize, blocks, freeblks, disk_size, used, free;
					// blksize = stat.f_bsize;
					// blocks = stat.f_blocks;
					// freeblks = stat.f_bfree;

					// disk_size = blocks * blksize;
					// free = freeblks * blksize;
					// used = disk_size - free;

					// printf ("%u\r\n", free /1024/1024);
				// }
				// else
				// {
					// break;
				// }
			// }
		// }
		// else
		// {
		// }
	// }
}
