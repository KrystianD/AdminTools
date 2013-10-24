#define _WIN32_WINNT  0x0501

#include <cstdlib>
#include <iostream>
#include "windows.h"
#include "Config.h"
#include "Config.cpp"
#include "sensors.h"
#include "windows.h"
#include "winbase.h"



extern "C" {
#include "sigar.h"
#include "sigar_format.h"
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
	
    // Ustawienie temperatury
    
    
    // Pamiêæ
    
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex); 

    GlobalMemoryStatusEx (&statex);
    cout << "Physical RAM: " << (float)statex.ullTotalPhys/(1024*1024*1024)<< endl;
     
    data.totalRam = (float)statex.ullTotalPhys/(1024*1024*1024);

    
    
    
    
    
    data.tempValid = true;
    
    data.uptime = 0;
	data.freeRam = 0;
	
//	TDiskUsage d;
//	d.name = dev;
//	d.totalSpace = blocks * blksize;
//	d.usedSpace = (blocks - freeblks) * blksize;
//	data.disksUsage.push_back (d);
	
}

int main() {
    Config c;
    
	if (!c.fromFile ("config")) {
       printf("Brak pliku konfiguracyjnego lub plik uszkodzony! \n");
       system("pause");
       return 0;
    };

	printf ("Port: %d\r\n", c.getInt ("port"));

    TSensorsData t;
 
    readSensors(t);

    system("pause");
    return 0;
}
