#pragma once

#include "Types.h"
#include "ModuleInfo.h"
#include <vector>
#include <map>

namespace SystemInfo
{
	class Processes : public ModuleInfo
	{
		public:
			Processes()
				:	stats(new Stats()),
					pids(std::vector<uint64>()),
					details(std::vector<Details*>())	{}

			struct Stats
			{
				Stats() {
					total = sleeping = running = zombie =
						stopped = idle = threads = 0;
				}
				uint64 total;
				uint64 sleeping;
				uint64 running;
				uint64 zombie;
				uint64 stopped;
				uint64 idle;
				uint64 threads;
			} *stats;

			std::vector<uint64> pids;

			struct Details
			{
				Details() {
					name = state = "";
					tty = priority = nice = processor = 0;
					parentPid = threads = cpu_start_time = cpu_user =
						cpu_sys = cpu_total = cpu_last_time =
						time_start_time = time_user = time_sys =
						time_total = mem_size = mem_resident =
						mem_share = mem_minor_faults = pid =
						mem_major_faults = mem_page_faults = 0;
					cpu_percent = 0.0;
				}

				// state
				uint64 pid;
				std::string name;
				std::string state;
				uint64 parentPid;
				uint16 tty;
				uint16 priority;
				uint16 nice;
				uint16 processor;
				uint64 threads;
				// cpu
				uint64 cpu_start_time;
				uint64 cpu_user;
				uint64 cpu_sys;
				uint64 cpu_total;
				uint64 cpu_last_time;
				double cpu_percent;
				// time
				uint64 time_start_time;
				uint64 time_user;
				uint64 time_sys;
				uint64 time_total;
				// memory
				uint64 mem_size;
				uint64 mem_resident;
				uint64 mem_share;
				uint64 mem_minor_faults;
				uint64 mem_major_faults;
				uint64 mem_page_faults;
			};
			std::vector<Details*> details;

			static std::string getFullStateName(char arg) {
				std::map<char, std::string> states;
				states['S'] = "Sleep";
				states['R'] = "Run";
				states['T'] = "Stop";
				states['Z'] = "Zombie";
				states['D'] = "Idle";
				return states[arg];
			}

			void displayCombinedInfo()
			{
				displayModuleHeader("Processes");

				displayGroupHeader("Statistics");
				displaySingleInfo("Total", stats -> total);
				displaySingleInfo("Sleeping", stats -> sleeping);
				displaySingleInfo("Running", stats -> running);
				displaySingleInfo("Zombie", stats -> zombie);
				displaySingleInfo("Stopped", stats -> stopped);
				displaySingleInfo("Idle", stats -> idle);
				displaySingleInfo("Threads", stats -> threads);

				for(uint16 i = 0; i < details.size(); ++i) {
					displayGroupHeader("Process #" + lexical_cast<std::string>(i+1));
					displayDetailsInfo(details[i]);
				}
			}

			void displayDetailsInfo(Details* details)
			{
				displaySingleInfo("PID", details -> pid);
				displaySingleInfo("Name", details -> name);
				displaySingleInfo("State", details -> state);
				displaySingleInfo("Parent PID", details -> parentPid);
				displaySingleInfo("TTY", details -> tty);
				displaySingleInfo("Priority", details -> priority);
				displaySingleInfo("Nice", details -> nice);
				displaySingleInfo("Processor", details -> processor);
				displaySingleInfo("Threads", details -> threads);

				displaySingleInfo("CPU start time", details -> cpu_start_time);
				displaySingleInfo("CPU user", details -> cpu_user);
				displaySingleInfo("CPU sys", details -> cpu_sys);
				displaySingleInfo("CPU total", details -> cpu_total);
				displaySingleInfo("CPU last time", details -> cpu_last_time);
				displaySingleInfo("CPU %", details -> cpu_percent);

				displaySingleInfo("TIME start time", details -> time_start_time);
				displaySingleInfo("TIME user", details -> time_user);
				displaySingleInfo("TIME sys", details -> time_sys);
				displaySingleInfo("TIME total", details -> time_total);

				displaySingleInfo("MEM size", details -> mem_size);
				displaySingleInfo("MEM resident", details -> mem_resident);
				displaySingleInfo("MEM share", details -> mem_share);
				displaySingleInfo("MEM minor faults", details -> mem_minor_faults);
				displaySingleInfo("MEM major faults", details -> mem_major_faults);
				displaySingleInfo("MEM page faults", details -> mem_page_faults);
			}
	};
}