#pragma once

#include "Types.h"
#include "Cpu.h"
#include "Resources.h"
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
			static Resources* sigarResourcesLimitToResources(sigar_resource_limit_t* resData);			
			static FileSystem::Details* sigarFileSystemToFsDetails(sigar_file_system_t* fsData);
			static FileSystem::Usage* sigarFileSystemUsageToFsUsage(sigar_file_system_usage_t* fsData, std::string dirName);			
	};
}