#include "sensors.h"

#include <stdio.h>
#include <string.h>

#include <sstream>
using namespace std;

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

void getSensorsData (TSensorsData& data)
{
	char path[500];

	data.tempValid = false;
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
