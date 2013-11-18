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
				//! Directory name
				std::string dir;
				//! Device name
				std::string dev;
				//! Device type
				std::string type;
				//! Device system type
				std::string sys_type;
				//! Additional options
				std::string options;
				//! File system type
				std::string fsType;
				//! Additional flags
				uint32 flags;
			};
			//! List of details per directory
			std::vector<Details*> dirDetails;

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
				//! Directory/disc name for usage informations
				std::string dir;
				//! Percent use of directory/disc
				double percent_use;
				//! Total memory assigned to disc
				uint64 total;
				//!	Free memory on disc
				uint64 free;
				//! Memory used on disc
				uint64 used;
				//! Memory available on disc
				uint64 avail;
				//! Memory used by files.
				uint64 files;
				//! Memory not used by files.
				uint64 free_files;

				//! Number of disc reads operation.
				uint64 disc_reads;
				//! Number of disc writes operation.
				uint64 disc_writes;
				//! Number of bytes writed.
				uint64 disc_write_bytes;
				//! Number of bytes readed.
				uint64 disc_read_bytes;
				//! Time spent on read.
				uint64 disc_rtime;
				//! Time spent on write.
				uint64 disc_wtime;
				//! Time spent in queue.
				uint64 disc_qtime;
				//! Total time.
				uint64 disc_time;
				//! Snap time.
				uint64 disc_snaptime;
				//! Disc service time.
				double disc_service_time;
				//! Number of disc queues.
				double disc_queue;
			};
			//! List with usage information for every disc
			std::vector<Usage*> dirUsages;

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