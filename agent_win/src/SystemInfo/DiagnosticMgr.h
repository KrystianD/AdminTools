#pragma once

#include "Types.h"
#include "Memory.h"
#include "Resources.h"
#include "Cpu.h"
#include "Processes.h"
#include "FileSystem.h"

/**
 *	\namespace SystemInfo
 *	\brief System information gathering module
 */
namespace SystemInfo
{
	/**
	 *	\class DiagnosticMgr
	 *	\brief Singleton manager for information gathering and visualization.
	 *
	 *	Simple wrapper around Sigar and WMI libraries.
	 */
	class DiagnosticMgr
	{
		public:
			/**
			 *	\brief Get informations about memory (RAM/Swap).
			 *	\return Memory informations.
			 */
			Memory* getMemoryInfo();
			/**
			 *	\brief Get informations about resources used and its limits.
			 *	\return Resources informations.
			 */
			Resources* getResourcesInfo();
			/**
			 *	\brief Get informations about CPUs/Cores.
			 *	\return Cpu and cores informations.
			 */
			Cpu* getCpuInfo();
			/**
			 * 	\brief Get informations about processes currently running on system.
			 *	\return Processes informations.
			 */
			Processes* getProcessesInfo();
			/**
			 *	\brief Get informations about file system (HDD, virtual, external drives).
			 *	\return FileSystem informations.
			 */
			FileSystem* getFileSystemInfo();

			/**
			 *	\brief Get CPU temeprature.
			 *	\return Current CPU temperature in celsius degree.
			 */
			static double getCpuTemp();
			/**
			 *	\brief Get instance of DiagnosticMgr singleton class.
			 *	\return Instance of DiagnosticMgr.
			 */
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