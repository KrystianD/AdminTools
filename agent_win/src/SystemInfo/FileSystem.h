#pragma once

#include "Types.h"
#include "ModuleInfo.h"
#include <vector>
#include <map>

namespace SystemInfo
{
	class FileSystem : public ModuleInfo
	{
		public:
			FileSystem()
				:	dirDetails(std::vector<Details*>()),
					dirUsages(std::vector<Usage*>())	{}

			struct Details
			{
				Details() {
					dir = dev = type = sys_type = options = fsType = "";
					flags = 0;
				}
				std::string dir;
				std::string dev;
				std::string type;
				std::string sys_type;
				std::string options;
				std::string fsType;
				uint32 flags;
			};
			std::vector<Details*> dirDetails;

			struct Usage
			{
				Usage() {
					dir = "";
					percent_use = disc_service_time = disc_queue = 0.0;
					total = free = used = avail = files = free_files =
						disc_reads = disc_writes = disc_write_bytes =
						disc_read_bytes = disc_rtime = disc_wtime =
						disc_qtime = disc_time = disc_snaptime = 0;
				}
				std::string dir;

				double percent_use;
				uint64 total;
				uint64 free;
				uint64 used;
				uint64 avail;
				uint64 files;
				uint64 free_files;

				uint64 disc_reads;
				uint64 disc_writes;
				uint64 disc_write_bytes;
				uint64 disc_read_bytes;
				uint64 disc_rtime;
				uint64 disc_wtime;
				uint64 disc_qtime;
				uint64 disc_time;
				uint64 disc_snaptime;
				double disc_service_time;
				double disc_queue;
			};
			std::vector<Usage*> dirUsages;

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

			void displayUsageInfo(Usage* usage)
			{

			}
	};
}