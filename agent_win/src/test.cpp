#include <cstdlib>
#include <iostream>
#include "windows.h"
#include "Config.h"
#include "Config.cpp"
#include "sensors.h"

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

   for (int i = 0; i < 5; i++) {
        sigar_cpu_get(sigar_cpu, &current);
        sigar_cpu_perc_calculate(&old, &current, &perc);
        cout << perc.combined * 100 << std::endl;
        
        if (i != 0)
           avg = avg + perc.combined * 100;
        
        old = current;
        Sleep(100);
    }
    avg = avg/4;
    cout << "Cpu Usage: " << avg << std::endl;
    data.cpuUsage = avg;
    
    sigar_close(sigar_cpu);
	
    // Ustawienie temperatury
    
    data.tempValid = true;
    
    data.uptime = 0;
	data.freeRam = 0;
	data.totalRam = 0;
	
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

	printf ("%d\r\n", c.getInt ("port"));

    TSensorsData t;
 
    readSensors(t);

    system("pause");
    return 0;
}
