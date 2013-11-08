#pragma once

#include "Types.h"
#include "ModuleInfo.h"

namespace SystemInfo
{
	/**
	 *	\class Resources
	 *	\brief Contains current resources status and its limits.
	 */
	class Resources : public ModuleInfo
	{
		public:
			/**
			 *	\fn Resources()
			 *	\brief Default constructor, zeroes resource variables.
			 */
			Resources()
			{
				cpuCurrent = cpuMax = fileSizeCurrent = fileSizeMax =
					pipeSizeCurrent = pipeSizeMax = dataCurrent = dataMax =
					stackCurrent = stackMax = coreCurrent = coreMax =
					memoryCurrent = memoryMax = processesCurrent =
					processesMax = openFilesCurrent = openFilesMax =
					virtualMemoryCurrent = virtualMemoryMax = 0;
			}

			//! Current cpu resource.
			uint64 cpuCurrent;
			//! Limit of cpu resource.
			uint64 cpuMax;
			//!	Current file size.
			uint64 fileSizeCurrent;
			//! Limit of file size.
			uint64 fileSizeMax;
			//! Current pipe buffer size.
			uint64 pipeSizeCurrent;
			//! Limit of pipe buffer size.
			uint64 pipeSizeMax;
			//! Current data size.
			uint64 dataCurrent;
			//! Limit of data size.
			uint64 dataMax;
			//! Current stack use.
			uint64 stackCurrent;
			//! Limit of use of stack.
			uint64 stackMax;
			//! Current core resource.
			uint64 coreCurrent;
			//! Limit of core resource.
			uint64 coreMax;
			//! Current memory use.
			uint64 memoryCurrent;
			//! Limit of memory use.
			uint64 memoryMax;
			//! Curent processes resource.
			uint64 processesCurrent;
			//! Limit of processes resource.
			uint64 processesMax;
			//! Current opened files resource.
			uint64 openFilesCurrent;
			//! Limit of opened files resource at once.
			uint64 openFilesMax;
			//! Current virtual memory resource.
			uint64 virtualMemoryCurrent;
			//! Limit of virtual memory resource.
			uint64 virtualMemoryMax;

			/**
			 *	\fn void displayCombinedInfo()
			 *	\brief Dispaly all informations about resources using standard output (std::cout).
			 *	\return None
			 */
			void displayCombinedInfo()
			{
				displayModuleHeader("Resources");

				displayGroupHeader("General Info");
				displayPairInfo("Cpu", cpuCurrent, cpuMax);
				displayPairInfo("File size", fileSizeCurrent, fileSizeMax);
				displayPairInfo("Pipe size", pipeSizeCurrent, pipeSizeMax);
				displayPairInfo("Data", dataCurrent, dataMax);
				displayPairInfo("Stack", stackCurrent, stackMax);
				displayPairInfo("Core", coreCurrent, coreMax);
				displayPairInfo("Memory", memoryCurrent, memoryMax);
				displayPairInfo("Process", processesCurrent, processesMax);
				displayPairInfo("Open files", openFilesCurrent, openFilesMax);
				displayPairInfo("Virtual memory", virtualMemoryCurrent, virtualMemoryMax);
			}
	};
}