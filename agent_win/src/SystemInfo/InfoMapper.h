#pragma once

#include "Types.h"
#include "Cpu.h"
#include "Memory.h"
#include "Resources.h"
#include "Processes.h"
#include "FileSystem.h"

namespace SystemInfo
{
	/**
	 *	\class InfoMapper
	 *	\brief Utils for easy Sigar to SystemInfo data mapping.
	 */
	class InfoMapper
	{
		public:
			/**
			 *	\fn static Cpu::Times* sigarCpuToCpuTimes(sigar_cpu_t* cpuData)
			 *	\brief Mapping cpu times data.
			 *	\param cpuData Sigar Cpu times data.
			 *	\return Cpu::Times data.
			 */
			static Cpu::Times* sigarCpuToCpuTimes(sigar_cpu_t* cpuData);
			static Cpu::Details* sigarCpuInfoToDetails(sigar_cpu_info_t* cpuData);
			static Memory* sigarMemAndSwapToMemory(sigar_mem_t* memData, sigar_swap_t* swapData);
			static Resources* sigarResourcesLimitToResources(sigar_resource_limit_t* resData);
			static Processes::Stats* sigarProcStatToProcessesStats(sigar_proc_stat_t* procData);
			static FileSystem::Details* sigarFileSystemToFsDetails(sigar_file_system_t* fsData);
			static FileSystem::Usage* sigarFileSystemUsageToFsUsage(sigar_file_system_usage_t* fsData);

			static void fillProcessDetailsWithSigarProcState(Processes::Details* details, sigar_proc_state_t* state);
			static void fillProcessDetailsWithSigarProcCpu(Processes::Details* details, sigar_proc_cpu_t* cpu);
			static void fillProcessDetailsWithSigarProcTime(Processes::Details* details, sigar_proc_time_t* time);
			static void fillProcessDetailsWithSigarProcMemory(Processes::Details* details, sigar_proc_mem_t* mem);
	};
}