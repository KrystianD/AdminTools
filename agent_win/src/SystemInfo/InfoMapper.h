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
			/**
			 *	\fn static Cpu::Details* sigarCpuInfoToDetails(sigar_cpu_info_t* cpuData)
			 *	\brief Mapping cpu info data.
			 *	\param cpuData Sigar Cpu info data.
			 *	\return Cpu::Details data.
			 */
			static Cpu::Details* sigarCpuInfoToDetails(sigar_cpu_info_t* cpuData);
			/**
			 *	\fn static Resources* sigarResourcesLimitToResources(sigar_resource_limit_t* resData)
			 *	\brief Mapping resources limits data.
			 *	\param resData Sigar resources data.
			 *	\return Resources data.
			 */
			static Resources* sigarResourcesLimitToResources(sigar_resource_limit_t* resData);
			/**
			 *	\fn FileSystem::Details* sigarFileSystemToFsDetails(sigar_file_system_t* fsData)
			 *	\brief Mapping file system data.
			 *	\param fsData Sigar file system data.
			 *	\return FileSystem::Details data.
			 */
			static FileSystem::Details* sigarFileSystemToFsDetails(sigar_file_system_t* fsData);
			/**
			 *	\fn FileSystem::Usage* sigarFileSystemUsageToFsUsage(sigar_file_system_usage_t* fsData, std::string dirName)
			 *	\brief Mapping file system usage data.
			 *	\param fsData Sigar file system usage data.
			 *	\param dirName Directory name.
			 *	\return FileSystem::Usage data.
			 */
			static FileSystem::Usage* sigarFileSystemUsageToFsUsage(sigar_file_system_usage_t* fsData, std::string dirName);
	};
}