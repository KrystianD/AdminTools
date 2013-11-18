#pragma once

#include "Types.h"
#include "ModuleInfo.h"
#include <vector>
#include <string>

namespace SystemInfo
{
	/**
	 *	\class Cpu
	 *	\brief Contains Cpu related information (times, cpu model details).
	 */
	class Cpu : public ModuleInfo
	{
		public:
			/**
			 *	\fn Cpu()
			 *	\brief Default constructor, initialize general times information and core times / details info.
			 */
			Cpu()
				:	generalTimes(new Times()),
					coreTimes(std::vector<Times*>()),
					coreDetails(std::vector<Details*>()) {}

			/**
			 *	\struct Times
			 *	\brief Represents times that Cpu used on various operations and states.
			 */
			struct Times
			{
				/**
				 *	\fn Times()
				 *	\brief Default constructor, zeroes times variables.
				 */
				Times() {
					user = sys = nice = idle = wait =
						irq = soft_irq = stolen = total = 0;
				}
				//! Time spent outside kernel.
				uint64 user;
				//! Time spent inside kernel.
				uint64 sys;
				//! Nice time.
				uint64 nice;
				//! Time spent idleing, without jobs.
				uint64 idle;
				//! Time spent waiting for other tasks.
				uint64 wait;
				//!	Time spent on handling hardware irq (interrupt).
				uint64 irq;
				//! Time spent on handling software irq (interrupt).
				uint64 soft_irq;
				//! Time stolen.
				uint64 stolen;
				//! Total time spent on every tasks.
				uint64 total;
			};

			//! Times combined with all the cores.
			Times *generalTimes;
			//! Times information per core.
			std::vector<Times*> coreTimes;

			/**
			 *	\struct Details
			 *	\brief Represents basic Cpu/core information (vendor, mhz etc.).
			 */
			struct Details
			{
				/**
				 *	\fn Details()
				 *	\brief Default constructor, zeroes details variables.
				 */
				Details() {
					vendor = model = "";
					mhz = total_sockets = total_cores =
						cores_per_socket = 0;
					cache_size = 0;
				}

				//! Vendor name.
				std::string vendor;
				//! Cpu model.
				std::string model;
				//! Clock rate in MHZ.
				uint16 mhz;
				//! Cache size in bits.
				uint64 cache_size;
				//! Total number of available sockets.
				uint16 total_sockets;
				//! Total number of available cores.
				uint16 total_cores;
				//! Number of cores per available sockets.
				uint16 cores_per_socket;
			};
			//! Model Details per core.
			std::vector<Details*> coreDetails;

			/**
			 *	\fn void displayCombinedInfo()
			 *	\brief Display all information about cpu/cores using standard output (std::cout).
			 *	\return None
			 */
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

			/**
			 *	\fn void displayTimesInfo(Times* times)
			 *	\brief Display information about times using standard output (std::cout).
			 *	\return None
			 */
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

			/**
			 *	\fn void displayDetailsInfo(Details* details)
			 *	\brief Display information about details using standard output (std::cout).
			 *	\return None
			 */
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