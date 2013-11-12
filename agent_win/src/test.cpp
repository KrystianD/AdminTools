#define _WIN32_WINNT  0x0501

#include <vector>
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <winbase.h>

#include "../../common/config.h"
#include "../../common/sensors.h"
#include "SystemInfo/DiagnosticMgr.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include "serverWin.cpp"

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
    data.cpuUsage = avg;
    
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
    
    float minup = GetTickCount() / 1000 / 60 ;
    
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
	
    int cc;
	opterr = 0;
	while ((cc = getopt (argc, argv, "c:")) != -1) {
		switch (cc) {
		
        case 'c':
			configPath = optarg;
			break;
        default:
			break;
        }
	}

	printf ("Using config: %s\r\n", configPath);
	
	Config c;
	
    if (! c.fromFile (configPath)) {
        std::cout << "Plik uszkodzony lub nie istnieje!" << std::endl;  
    }

    Server serv;
	serv.setup (c.getString ("host"), c.getInt ("port"), c.getString ("key"));

    TSensorsData t;
    readSensors(t);
    
    

    system("pause");
    return 0;
}
