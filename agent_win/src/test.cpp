#define _WIN32_WINNT  0x0501

#include <vector>
#include <iostream>
#include <time.h>

#include <tchar.h>
#include <csignal>


#include "../../common/configWin.h"
#include "../../common/configWin.cpp"
#include "../../common/sensors.h"
#include "SystemInfo/DiagnosticMgr.h"
#include "serverWin.h"
#include "kutilsWin.h"

bool endd = false;

void signal_handler (int signum)
{
	endd = true;
}


BOOL WINAPI ConsoleHandler(DWORD CEvent)
{

	endd = true;
    char mesg[128];

    switch(CEvent)
    {
    case CTRL_C_EVENT:
        MessageBox(NULL,
            _T("CTRL+C received!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_BREAK_EVENT:
        MessageBox(NULL,
            _T("CTRL+BREAK received!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_CLOSE_EVENT:
        MessageBox(NULL,
            _T("Program being closed!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_LOGOFF_EVENT:
        MessageBox(NULL,
            _T("User is logging off!"),_T("CEvent"),MB_OK);
        break;
    case CTRL_SHUTDOWN_EVENT:
        MessageBox(NULL,
            _T("User is logging off!"),_T("CEvent"),MB_OK);
        break;

    }

	endd = true;
    return false;
}


using SystemInfo::DiagnosticMgr;
using SystemInfo::FileSystem;

using namespace WinAgent;

extern "C" {
       #include "SystemInfo/sigar/sigar.h"
       #include "SystemInfo/sigar/sigar_format.h"
}


void readSensors(TSensorsData& data, TPacketConfig t) {   
   
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
    data.cpuUsage = (float)avg/1000;
    
    sigar_close(sigar_cpu);
    
    // Liczba rdzeni
    
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    
    int numCPU = sysinfo.dwNumberOfProcessors;
    
    cout << "Number of cores: " << numCPU << std::endl;
	
    // Całkowita pamięc RAM
    
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex); 

    GlobalMemoryStatusEx (&statex);
    cout << "Physical RAM: " << (float)statex.ullTotalPhys << " B" << std::endl;
     
    data.totalRam = (uint64_t)statex.ullTotalPhys;

    // Pobranie wolnej pamięci RAM

    MEMORYSTATUS m;
   	
    m.dwLength = sizeof(m);
   	uint64_t ret = 0;
   	
    GlobalMemoryStatus(&m);
   	ret = (m.dwAvailPhys);
    cout << "Available physical RAM: " << ret << " B" << std::endl;
     
	data.freeRam = ret;    
    
    // Pobranie czasu pracy
    
    float minup = (float)GetTickCount() / 1000 / 60 ;

	float secup = (float)GetTickCount() / 1000;
    
    std::cout << "Uptime: " << minup << " minutes." << std::endl;
    data.uptime = (uint32_t)secup;       

	// Current CPU temperature
	std::cout << "CPU temperature: " << DiagnosticMgr::getInstance().getCpuTemp() << std::endl;
	data.temp = (float) DiagnosticMgr::getInstance().getCpuTemp();
	data.tempValid = true;
    
	// Disks usage	
	std::vector<FileSystem::Usage*> discsUsage = 
		DiagnosticMgr::getInstance().getFileSystemInfo() -> dirUsages;

	for(auto it = discsUsage.begin(); it != discsUsage.end(); ++it) {
		TDiskUsage currentDisc;
		currentDisc.name = (*it) -> dir;
		currentDisc.totalSpace = (*it) -> total*1024;
		currentDisc.usedSpace = (*it) -> used*1024;
		data.disksUsage.push_back(currentDisc);
		
		std::cout << "\tName: " << currentDisc.name << std::endl;
		std::cout << "\tTotal: " << currentDisc.totalSpace /(1024 * 1024) << "GB" << std::endl;
		std::cout << "\tUsed: " << currentDisc.usedSpace /(1024 * 1024) << "GB" << std::endl;
	}

	// Services
	for (int i = 0; i < t.services.size (); i++) {
		const TPacketConfig::TService& s = t.services[i];

		std::cout << "Service " << s.name << " is " ;
		
		if (s.tcp) {
		
			int iResult;
			WSADATA wsaData;
			// Initialize Winsock
			iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
			if (iResult != 0) {
				printf("WSAStartup failed: %d\n", iResult);
				return;
			} else {
				
			}

			sockaddr_in servaddr;

			SOCKET fd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (fd == INVALID_SOCKET)
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
			
			if (!::connect (fd, servinfo->ai_addr, servinfo->ai_addrlen)) {
				service.available = 1;
				std::cout << "available." << std::endl;
			} else {
				service.available = 0;
				std::cout << "not available." << std::endl;
			}

			freeaddrinfo (servinfo);

			data.services.push_back (service);
			
			closesocket(fd);
			fd = INVALID_SOCKET;

		} else {
		
			WSADATA wsaData;

		    int iResult = 0;            

			SOCKET ListenSocket = INVALID_SOCKET;
			sockaddr_in service;

			TService serviceS;
			serviceS.name = s.name;


			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != NO_ERROR) {
				serviceS.available = 0;
				std::cout << "not available. ERROR 1" << std::endl;
				WSACleanup();
				continue;
			}

			
			ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (ListenSocket == INVALID_SOCKET) {
				serviceS.available = 0;
				std::cout << "not available. ERROR Socket" << std::endl;
				WSACleanup();
				continue;
			}

			service.sin_family = AF_INET;
			service.sin_addr.s_addr = inet_addr("127.0.0.1");
			service.sin_port = htons(s.port);
			
			iResult = bind(ListenSocket, (SOCKADDR *) &service, sizeof (service));
			if (iResult == 0 || iResult == -1) {
				
				serviceS.available = 0;
				std::cout << "not available" << std::endl;
				if (iResult == -1) {
					std::cout << "Error" << std::endl;
				}
			}
			else {
				std::cout << iResult << std::endl;
				std::cout << "available" << std::endl;
				serviceS.available = 1;
			}

			closesocket(ListenSocket);
			WSACleanup();

			data.services.push_back (serviceS);
			
		}
	}

	std::cout << "End of data." << std::endl << std::endl;
}

int main(int argc, char** argv) {
    const char *configPath = "config.cfg";

	printf ("Using config: %s\r\n", configPath);	
	Config c;	
    
	if (! c.fromFile (configPath)) {
        std::cout << "Plik uszkodzony lub nie istnieje!" << std::endl; 
		return 1;
    }

	//signal (SIGPIPE, SIG_IGN);
	signal (SIGINT, signal_handler);
	signal (SIGTERM, signal_handler);
	signal(SIGABRT, signal_handler);
    signal(SIGTERM, signal_handler);


	if (SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE)
    {
        // unable to install handler... 
        // display message to the user
        printf("Unable to install handler!\n");
        return -1;
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

	while(!endd)
	{
		serv.process();
		Sleep(10);
		TSensorsData d;

		if (getTicks () - lastSendTime >= serv.getConfig ().interval)
		{
			readSensors(d, serv.getConfig());

			TPacketAgentData agentData;
			agentData.id = 0;
			agentData.data = d;
			agentData.data.timestamp = time (0);
			agentData.name = "Win";

			if (serv.isValid ())
			{
				agentData.oldData = 0;
				serv.sendPacket (agentData);
				std::cout << "Packet has been sent!" << std::endl;
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

	std::cout << "Zapisuje dane!" << std::endl;

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
