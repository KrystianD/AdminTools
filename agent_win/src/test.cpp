#define _WIN32_WINNT  0x0501

#include <vector>
#include <iostream>
#include <time.h>

#include "../../common/config.h"
#include "../../common/sensors.h"
#include "SystemInfo/DiagnosticMgr.h"
#include "serverWin.h"
#include "kutilsWin.h"

using SystemInfo::DiagnosticMgr;
using SystemInfo::FileSystem;

extern "C" {
       #include "SystemInfo/sigar/sigar.h"
       #include "SystemInfo/sigar/sigar_format.h"
}

void readSensors(TSensorsData& data) {   
   
    sigar_t *sigar_cpu;
    sigar_cpu_t old;
    sigar_cpu_t current;

    // Ustawienie CpuUsage
   
    sigar_open(&sigar_cpu);
    sigar_cpu_get(sigar_cpu, &old);
   
    sigar_cpu_perc_t perc;
    double avg = 0.0;
    Sleep(100);
   
    sigar_cpu_get(sigar_cpu, &current);
    sigar_cpu_perc_calculate(&old, &current, &perc);
    avg = perc.combined * 100;
   
    cout << "Cpu Usage: " << avg << std::endl;
    data.cpuUsage = (float)avg;
    
    sigar_close(sigar_cpu);
    
    // Liczba rdzeni
    
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    
    int numCPU = sysinfo.dwNumberOfProcessors;
    
    cout << "Number of cores: " << numCPU << std::endl;
	
    // Ca�kowita pami�c RAM
    
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex); 

    GlobalMemoryStatusEx (&statex);
    cout << "Physical RAM: " << (float)statex.ullTotalPhys/(1024*1024) << " MB" << std::endl;
     
    data.totalRam = (uint64_t)statex.ullTotalPhys/(1024*1024);

    // Pobranie wolnej pami�ci RAM

    MEMORYSTATUS m;
   	
    m.dwLength = sizeof(m);
   	int ret = 0;
   	
    GlobalMemoryStatus(&m);
   	ret = (int)(m.dwAvailPhys >> 20);
    cout << "Available physical RAM: " << ret << " MB" << std::endl;
     
    data.freeRam = ret;    
    
    // Pobranie czasu pracy
    
    float minup = (float)GetTickCount() / 1000 / 60 ;
    
    std::cout << "Uptime: " << minup << " minutes." << std::endl;
    data.uptime = (uint32_t)minup;       

	// Current CPU temperature
	std::cout << "CPU temperature: " << DiagnosticMgr::getCpuTemp() << std::endl;
	data.temp = (float) DiagnosticMgr::getCpuTemp();
	data.tempValid = true;
    
	// Discs usage	
	std::vector<FileSystem::Usage*> discsUsage = 
		DiagnosticMgr::getInstance().getFileSystemInfo() -> dirUsages;

	for(auto it = discsUsage.begin(); it != discsUsage.end(); ++it) {
		TDiskUsage currentDisc;
		currentDisc.name = (*it) -> dir;
		currentDisc.totalSpace = (*it) -> total;
		currentDisc.usedSpace = (*it) -> used;
		data.disksUsage.push_back(currentDisc);
		
		std::cout << "\tName: " << currentDisc.name << std::endl;
		std::cout << "\tTotal: " << currentDisc.totalSpace /(1024 * 1024) << "GB" << std::endl;
		std::cout << "\tUsed: " << currentDisc.usedSpace /(1024 * 1024) << "GB" << std::endl;
	}
}

int main(int argc, char** argv) {
    const char *configPath = "config.cfg";

	printf ("Using config: %s\r\n", configPath);	
	Config c;	
    
	if (! c.fromFile (configPath)) {
        std::cout << "Plik uszkodzony lub nie istnieje!" << std::endl; 
		return 1;
    }

    Server serv;
	//serv.connectServer();
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

	while(1)
	{
		serv.process();
		usleep(10000);
		TSensorsData d;

		// gather data from senosors and send it to server or save in memory depending on connectino state
		if (getTicks () - lastSendTime >= serv.getConfig ().interval)
		{
			readSensors(d);
			//Krystian tu używa funkcji która jest w sensors.cpp postaci getSensorsData (d, serv.getConfig ());
		
			TPacketAgentData agentData;
			agentData.id = 0;
			agentData.data = d;
			agentData.data.timestamp = time (0);

			if (serv.isValid ())
			{
				agentData.oldData = 0;
				serv.sendPacket (agentData);
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
			printf ("Sent old packet, %d left\n", oldSensorsData.size ());
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

    //system("pause");
    return 0;
}
