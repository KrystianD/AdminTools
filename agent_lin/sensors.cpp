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

string readFile (const string& path)
{
	FILE *f = fopen (path.c_str (), "rt");
	if (!f)
		return "";
	char buf[500];
	fscanf (f, "%s", buf);
	fclose (f);
	return buf;
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
		for (int i = 0; ; i++)
		{
			sprintf (path, "/sys/class/hwmon/hwmon%d/device/name", i);
			string s = readFile (path);
			if (s.size () == 0)
				break;

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

		sockaddr_in servaddr;

		int fd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
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

		data.services.push_back (service);
		close (fd);
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
