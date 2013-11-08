#pragma once

#include "Types.h"
#include "ModuleInfo.h"
#include <vector>
#include <string>

namespace SystemInfo
{
	class Cpu : public ModuleInfo
	{
		public:
			Cpu()
				:	generalTimes(new Times()),
					coreTimes(std::vector<Times*>()),
					coreDetails(std::vector<Details*>()) {}

			struct Times
			{
				Times() {
					user = sys = nice = idle = wait =
						irq = soft_irq = stolen = total = 0;
				}
				uint64 user;
				uint64 sys;
				uint64 nice;
				uint64 idle;
				uint64 wait;
				uint64 irq;
				uint64 soft_irq;
				uint64 stolen;
				uint64 total;
			} *generalTimes;
			std::vector<Times*> coreTimes;

			struct Details
			{
				Details() {
					vendor = model = "";
					mhz = total_sockets = total_cores =
						cores_per_socket = 0;
					cache_size = 0;
				}

				std::string vendor;
				std::string model;
				uint16 mhz;
				uint64 cache_size;
				uint16 total_sockets;
				uint16 total_cores;
				uint16 cores_per_socket;
			};
			std::vector<Details*> coreDetails;

			void displayCombinedInfo()
			{
				displayModuleHeader("Cpu");

				displayGroupHeader("General Times");
				displayTimesInfo(generalTimes);

				for(uint16 i = 0; i < coreTimes.size(); ++i) {
					displayGroupHeader("Times Core #"
						+ lexical_cast<std::string>(i+1));
					displayTimesInfo(coreTimes[i]);
				}
				for(uint16 j = 0; j < coreDetails.size(); ++j) {
					displayGroupHeader("Details Core #"
						+ lexical_cast<std::string>(j+1));
					displayDetailsInfo(coreDetails[j]);
				}
			}

			void displayTimesInfo(Times* times)
			{
				displaySingleInfo("User", times -> user);
				displaySingleInfo("Sys", times -> sys);
				displaySingleInfo("Nice", times -> nice);
				displaySingleInfo("Idle", times -> idle);
				displaySingleInfo("Wait", times -> wait);
				displaySingleInfo("IRQ", times -> irq);
				displaySingleInfo("Soft IRQ", times -> soft_irq);
				displaySingleInfo("Stolen", times -> stolen);
				displaySingleInfo("Total", times -> total);
			}

			void displayDetailsInfo(Details* details)
			{
				displaySingleInfo("Vendor", details -> vendor);
				displaySingleInfo("Model", details -> model);
				displaySingleInfo("Mhz", details -> mhz);
				displaySingleInfo("Cache size", details -> cache_size);
				displaySingleInfo("Total sockets", details -> total_sockets);
				displaySingleInfo("Total cores", details -> total_cores);
				displaySingleInfo("Cores per socket", details -> cores_per_socket);
			}
	};
}