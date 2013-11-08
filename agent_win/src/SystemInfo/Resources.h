#pragma once

#include "Types.h"
#include "ModuleInfo.h"

namespace SystemInfo
{
	class Resources : public ModuleInfo
	{
		public:
			Resources()
			{
				cpuCurrent = cpuMax = fileSizeCurrent = fileSizeMax =
					pipeSizeCurrent = pipeSizeMax = dataCurrent = dataMax =
					stackCurrent = stackMax = coreCurrent = coreMax =
					memoryCurrent = memoryMax = processesCurrent =
					processesMax = openFilesCurrent = openFilesMax =
					virtualMemoryCurrent = virtualMemoryMax = 0;
			}

			// general resources
			uint64 cpuCurrent, cpuMax;
			uint64 fileSizeCurrent, fileSizeMax;
			uint64 pipeSizeCurrent, pipeSizeMax;
			uint64 dataCurrent, dataMax;
			uint64 stackCurrent, stackMax;
			uint64 coreCurrent, coreMax;
			uint64 memoryCurrent, memoryMax;
			uint64 processesCurrent, processesMax;
			uint64 openFilesCurrent, openFilesMax;
			uint64 virtualMemoryCurrent, virtualMemoryMax;

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