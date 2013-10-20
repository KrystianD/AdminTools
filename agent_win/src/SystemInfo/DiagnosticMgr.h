#pragma once

#include "Types.h"
#include "Memory.h"
#include "Resources.h"
#include "Cpu.h"
#include "Processes.h"
#include "FileSystem.h"

namespace SystemInfo
{
	class DiagnosticMgr
	{
		public:
			Memory* getMemoryInfo();
			Resources* getResourcesInfo();
			Cpu* getCpuInfo();
			Processes* getProcessesInfo();
			FileSystem* getFileSystemInfo();

			static DiagnosticMgr& getInstance() {
				static DiagnosticMgr instance;
				return instance;
			}
		private:
			DiagnosticMgr();
			DiagnosticMgr(const DiagnosticMgr&) {}
			DiagnosticMgr& operator=(const DiagnosticMgr&) {}
			~DiagnosticMgr();

			Processes::Details* getProcessDetails(uint64 pid);

			sigar_t* sigarCore;
			bool initialized;
	};
}