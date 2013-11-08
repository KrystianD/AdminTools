#pragma once

#include "Types.h"
#include "ModuleInfo.h"
#include <vector>
#include <map>

namespace SystemInfo
{
	/**
	 *	\class FileSystem
	 *	\brief Class contains file system informations
	 */
	class FileSystem : public ModuleInfo
	{
		public:
			/**
			 *	\fn FileSystem()
			 *	\brief Default constructor, initialize sub-structures for details/usage information
			 */
			FileSystem()
				:	dirDetails(std::vector<Details*>()),
					dirUsages(std::vector<Usage*>())	{}
			/**
			 *	\struct Details
			 *	\brief General information about device/disk
			 */
			struct Details
			{
				/**
				 *	\fn Details()
				 *	\brief Default constructor, initialize general directory/device information
				 */
				Details() {
					dir = dev = type = sys_type = options = fsType = "";
					flags = 0;
				}
				std::string dir;	//! Directory name
				std::string dev;	//! Device name
				std::string type;	//! Device type
				std::string sys_type;	//! Device system type
				std::string options;	//! Additional options
				std::string fsType;		//! File system type
				uint32 flags;	//! Additional flags
			};
			std::vector<Details*> dirDetails;	//! List of details per directory

			/**
			 *	\struct Usage
			 *	\brief Informations about usage of directory/devie
			 */
			struct Usage
			{
				/**
				 *	\fn Usage()
				 *	\brief Default constructor, zeroing usage informations
				 */
				Usage() {
					dir = "";
					percent_use = disc_service_time = disc_queue = 0.0;
					total = free = used = avail = files = free_files =
						disc_reads = disc_writes = disc_write_bytes =
						disc_read_bytes = disc_rtime = disc_wtime =
						disc_qtime = disc_time = disc_snaptime = 0;
				}
				std::string dir;	//! Directory/disc name for usage informations

				double percent_use;	//! Percent use of directory/disc
				uint64 total;	//! Total memory assigned to disc
				uint64 free;	//!	Free memory on disc
				uint64 used;	//! Memory used on disc
				uint64 avail;	//! Memory available on disc
				uint64 files;	//!
				uint64 free_files;	//!

				uint64 disc_reads;	//!
				uint64 disc_writes;	//!
				uint64 disc_write_bytes;	//!
				uint64 disc_read_bytes;
				uint64 disc_rtime;
				uint64 disc_wtime;
				uint64 disc_qtime;
				uint64 disc_time;
				uint64 disc_snaptime;
				double disc_service_time;
				double disc_queue;
			};
			std::vector<Usage*> dirUsages;	//! List with usage information for every disc

			/**
			 * 	\fn getFullFsTypeName(sigar_file_system_type_e type)
			 *	\brief Map file system type to human-readable string
			 *	\return Human-readable file system type
			 */
			static std::string getFullFsTypeName(sigar_file_system_type_e type)
			{
				std::map<sigar_file_system_type_e, std::string> names;
				names[SIGAR_FSTYPE_UNKNOWN] = "Unknown";
				names[SIGAR_FSTYPE_NONE] = "None";
				names[SIGAR_FSTYPE_LOCAL_DISK] = "Local disc";
				names[SIGAR_FSTYPE_NETWORK] = "Network";
				names[SIGAR_FSTYPE_RAM_DISK] = "RAM disc";
				names[SIGAR_FSTYPE_CDROM] = "CD-ROM";
				names[SIGAR_FSTYPE_SWAP] = "Swap";
				names[SIGAR_FSTYPE_MAX] = "Max";
				return names[type];
			}
			/**
			 *	\fn void displayCombinedInfo()
			 *	\brief Display all informations about file system on standard output (std::cout)
			 *	\return None
			 */
			void displayCombinedInfo()
			{
				displayModuleHeader("HDD");
				for(uint16 i = 0; i < dirDetails.size(); ++i) {
					displayGroupHeader("Disc details #"
						+ lexical_cast<std::string>(i+1));
					displayDetailsInfo(dirDetails[i]);
				}
				for(uint16 j = 0; j < dirUsages.size(); ++j) {
					displayGroupHeader("Disc usage #"
						+ lexical_cast<std::string>(j+1));
					displayUsageInfo(dirUsages[j]);
				}
			}

			/**
			 *	\fn void displayDetailsInfo(Details* details)
			 *	\brief Display informations from Details struct over standard output (std::cout)
			 *	\return None
			 */
			void displayDetailsInfo(Details* details)
			{
				displaySingleInfo("Directory", details -> dir);
				displaySingleInfo("Device", details -> dev);
				displaySingleInfo("Type", details -> type);
				displaySingleInfo("System type", details -> sys_type);
				displaySingleInfo("Options", details -> options);
				displaySingleInfo("FS Type", details -> fsType);
				displaySingleInfo("Flags", details -> flags);
			}

			/**
			 *	\fn void displayUsageInfo(Usage* usage)
			 *	\brief Display informations from Usage struct over standard output (std::cout)
			 *	\return None
			 */
			void displayUsageInfo(Usage* usage)
			{

			}
	};
}