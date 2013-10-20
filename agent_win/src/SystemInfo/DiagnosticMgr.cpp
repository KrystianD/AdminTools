#include "DiagnosticMgr.h"

#include <iostream>
#include "InfoMapper.h"

using namespace SystemInfo;

DiagnosticMgr::DiagnosticMgr()
	: initialized(false)
{
	sigar_open(&sigarCore);

	if(sigarCore != NULL)
		initialized = true;		
	else
		std::cout << "Error while opening SIGAR core" << std::endl;		
}

DiagnosticMgr::~DiagnosticMgr()
{
	sigar_close(sigarCore);
}

Memory* DiagnosticMgr::getMemoryInfo()
{
	if(!initialized) {
		std::cout << "Couldn't acquire memory info" << std::endl;
		return NULL;
	}
	sigar_mem_t* memData = new sigar_mem_t();
	if(sigar_mem_get(sigarCore, memData)) {
		std::cout << "SIGAR couldn't acquire memory info" << std::endl;
		return NULL;
	}
	sigar_swap_t* swapData = new sigar_swap_t();
	if(sigar_swap_get(sigarCore, swapData)) {
		std::cout << "SIGAR couldn't acquire swap info" << std::endl;		
		return NULL;
	}	
	return InfoMapper::sigarMemAndSwapToMemory(memData, swapData);	
}

Resources* DiagnosticMgr::getResourcesInfo()
{
	if(!initialized) {
		std::cout << "Couldn't acquire resources info" << std::endl;
		return NULL;
	}
	sigar_resource_limit_t* resourcesData = new sigar_resource_limit_t();
	if(sigar_resource_limit_get(sigarCore, resourcesData)) {
		std::cout << "SIGAR couldn't acquire resources info" << std::endl;
		return NULL;
	}

	return InfoMapper::sigarResourcesLimitToResources(resourcesData);
}

Cpu* DiagnosticMgr::getCpuInfo()
{
	if(!initialized) {
		std::cout << "Couldn't acquire cpu info" << std::endl;
		return NULL;
	}
	sigar_cpu_t* cpuData = new sigar_cpu_t();
	if(sigar_cpu_get(sigarCore, cpuData)) {
		std::cout << "SIGAR couldn't acquire cpu info" << std::endl;
		return NULL;
	}
	Cpu* result = new Cpu();
	result -> generalTimes = InfoMapper::sigarCpuToCpuTimes(cpuData);	

	sigar_cpu_list_t* coreTimes = new sigar_cpu_list_t();
	if(sigar_cpu_list_get(sigarCore, coreTimes)) {
		std::cout << "SIGAR couldn't acquire cpu cores info" << std::endl;
		return result;
	}
	for(uint16 i = 0; i < coreTimes -> number; ++i) {
		result -> coreTimes.push_back(
			InfoMapper::sigarCpuToCpuTimes(&(coreTimes -> data[i])));
	}		

	sigar_cpu_info_list_t* coreDetails = new sigar_cpu_info_list_t();
	if(sigar_cpu_info_list_get(sigarCore, coreDetails)) {
		std::cout << "SIGAR couldn't acquire cpu details info" << std::endl;
		return result;
	}
	for(uint16 j = 0; j < coreDetails -> number; ++j) {
		result -> coreDetails.push_back(
			InfoMapper::sigarCpuInfoToDetails(&(coreDetails -> data[j])));
	}
	return result;
}

Processes* DiagnosticMgr::getProcessesInfo()
{
	if(!initialized) {
		std::cout << "Couldn't acquire processes info" << std::endl;
		return NULL;
	}

	sigar_proc_stat_t* procStats = new sigar_proc_stat_t();
	if(sigar_proc_stat_get(sigarCore, procStats)) {
		std::cout << "SIGAR couldn't acquire processes stats" << std::endl;
		return NULL;
	}
	Processes* result = new Processes();
	result -> stats = InfoMapper::sigarProcStatToProcessesStats(procStats);

	sigar_proc_list_t* procList = new sigar_proc_list_t();
	if(sigar_proc_list_get(sigarCore, procList)) {
		std::cout << "SIGAR couldn't acquire processes list" << std::endl;
		return result;
	}
	for(uint16 i = 0; i < procList -> number; ++i) {
		uint64 pid = procList -> data[i];
		result -> pids.push_back(pid);
		result -> details.push_back(getProcessDetails(pid));
	}

	return result;
}

Processes::Details* DiagnosticMgr::getProcessDetails( uint64 pid )
{
	Processes::Details* result = new Processes::Details();
	result -> pid = pid;

	sigar_proc_state_t* procState = new sigar_proc_state_t();
	if(sigar_proc_state_get(sigarCore, pid, procState)) {
		std::cout << "SIGAR couldn't acquire process state" << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcState(result, procState);

	sigar_proc_cpu_t* procCpu = new sigar_proc_cpu_t();
	if(sigar_proc_cpu_get(sigarCore, pid, procCpu)) {
		std::cout << "SIGAR couldn't acquire process cpu" << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcCpu(result, procCpu);

	sigar_proc_time_t* procTime = new sigar_proc_time_t();
	if(sigar_proc_time_get(sigarCore, pid, procTime)) {
		std::cout << "SIGAR couldn't acquire process time" << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcTime(result, procTime);

	sigar_proc_mem_t* procMem = new sigar_proc_mem_t();
	if(sigar_proc_mem_get(sigarCore, pid, procMem)) {
		std::cout << "SIGAR couldn't acquire process memory" << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcMemory(result, procMem);

	return result;
}
