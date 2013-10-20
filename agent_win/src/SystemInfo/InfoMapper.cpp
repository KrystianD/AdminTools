#include "InfoMapper.h"

using namespace SystemInfo;

Cpu::Times* SystemInfo::InfoMapper::sigarCpuToCpuTimes( sigar_cpu_t* cpuData )
{
	Cpu::Times* result = new Cpu::Times();
	if(!cpuData) return result;

	result -> user = cpuData -> user;
	result -> sys = cpuData -> sys;
	result -> nice = cpuData -> nice;
	result -> idle = cpuData -> idle;
	result -> wait = cpuData -> wait;
	result -> irq = cpuData -> irq;
	result -> soft_irq = cpuData -> soft_irq;
	result -> stolen = cpuData -> stolen;
	result -> total = cpuData -> total;
	return result;
}

Cpu::Details* SystemInfo::InfoMapper::sigarCpuInfoToDetails( sigar_cpu_info_t* cpuData )
{
	Cpu::Details* result = new Cpu::Details();
	if(!cpuData) return result;

	result -> vendor = std::string(cpuData -> vendor);
	result -> model = std::string(cpuData -> model);
	result -> mhz = cpuData -> mhz;
	result -> cache_size = cpuData -> cache_size;
	result -> total_sockets = cpuData -> total_sockets;
	result -> total_cores = cpuData -> total_cores;
	result -> cores_per_socket = cpuData -> cores_per_socket;
	return result;
}

Memory* SystemInfo::InfoMapper::sigarMemAndSwapToMemory( sigar_mem_t* memData, sigar_swap_t* swapData )
{
	Memory* result = new Memory();
	if(!memData || !swapData) return result;

	result -> ram = memData -> ram;
	result -> total = memData -> total;
	result -> used = memData -> used;
	result -> free = memData -> free;
	result -> used_actual = memData -> actual_used;
	result -> free_actual = memData -> actual_free;
	result -> used_percent = memData -> used_percent;
	result -> free_percent = memData -> free_percent;

	result -> total_swap = swapData -> total;
	result -> used_swap = swapData -> used;
	result -> free_swap = swapData -> free;
	result -> page_in = swapData -> page_in;
	result -> page_out = swapData -> page_out;
	return result;
}

Resources* SystemInfo::InfoMapper::sigarResourcesLimitToResources( sigar_resource_limit_t* resData )
{
	Resources* result = new Resources();
	if(!resData) return result;

	result -> cpuCurrent = resData -> cpu_cur;
	result -> cpuMax = resData -> cpu_max;
	result -> fileSizeCurrent = resData -> file_size_cur;
	result -> fileSizeMax = resData -> file_size_max;
	result -> pipeSizeCurrent = resData -> pipe_size_cur;
	result -> pipeSizeMax = resData -> pipe_size_max;
	result -> dataCurrent = resData -> data_cur;
	result -> dataMax = resData -> data_max;
	result -> stackCurrent = resData -> stack_cur;
	result -> stackMax = resData -> stack_max;
	result -> coreCurrent = resData -> core_cur;
	result -> coreMax = resData -> core_max;
	result -> memoryCurrent = resData -> memory_cur;
	result -> memoryMax = resData -> memory_max;
	result -> processesCurrent = resData -> processes_cur;
	result -> processesMax = resData -> processes_max;
	result -> openFilesCurrent = resData -> open_files_cur;
	result -> openFilesMax = resData -> open_files_max;
	result -> virtualMemoryCurrent = resData -> virtual_memory_cur;
	result -> virtualMemoryMax = resData -> virtual_memory_max;
	return result;
}

Processes::Stats* SystemInfo::InfoMapper::sigarProcStatToProcessesStats( sigar_proc_stat_t* procData )
{
	Processes::Stats* result = new Processes::Stats();
	if(!procData) return result;

	result -> total = procData -> total;
	result -> sleeping = procData -> sleeping;
	result -> running = procData -> running;
	result -> zombie = procData -> zombie;
	result -> stopped = procData -> stopped;
	result -> idle = procData -> idle;
	result -> threads = procData -> threads;
	return result;
}

void SystemInfo::InfoMapper::fillProcessDetailsWithSigarProcState( Processes::Details* details, sigar_proc_state_t* state )
{
	if(!details || !state) return;

	details -> name = std::string(state -> name);
	details -> state = Processes::getFullStateName(state -> state);
	details -> parentPid = state -> ppid;
	details -> tty = state -> tty;
	details -> priority = state -> priority;
	details -> nice = state -> nice;
	details -> processor = state -> processor;
	details -> threads = state -> threads;
}

void SystemInfo::InfoMapper::fillProcessDetailsWithSigarProcCpu( Processes::Details* details, sigar_proc_cpu_t* cpu )
{
	if(!details || !cpu) return;

	details -> cpu_start_time = cpu -> start_time;
	details -> cpu_user = cpu -> user;
	details -> cpu_sys = cpu -> sys;
	details -> cpu_total = cpu -> total;
	details -> cpu_last_time = cpu -> last_time;
	details -> cpu_percent = cpu -> percent;
}

void SystemInfo::InfoMapper::fillProcessDetailsWithSigarProcTime( Processes::Details* details, sigar_proc_time_t* time )
{
	if(!details || !time) return;

	details -> time_start_time = time -> start_time;
	details -> time_user = time -> user;
	details -> time_sys = time -> sys;
	details -> time_total = time -> total;
}

void SystemInfo::InfoMapper::fillProcessDetailsWithSigarProcMemory( Processes::Details* details, sigar_proc_mem_t* mem )
{
	if(!details || !mem) return;

	details -> mem_size = mem -> size;
	details -> mem_resident = mem -> resident;
	details -> mem_share = mem -> share;
	details -> mem_minor_faults = mem -> minor_faults;
	details -> mem_major_faults = mem -> major_faults;
	details -> mem_page_faults = mem -> page_faults;
}
