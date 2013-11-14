#include "DiagnosticMgr.h"

#include <iostream>
#include "InfoMapper.h"

// for CpuTemp
#include <comdef.h>
#include <WbemIdl.h>

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
		std::cout << "SIGAR couldn't acquire process state for pid: "
			<< pid << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcState(result, procState);

	sigar_proc_cpu_t* procCpu = new sigar_proc_cpu_t();
	if(sigar_proc_cpu_get(sigarCore, pid, procCpu)) {
		std::cout << "SIGAR couldn't acquire process cpu for pid: "
			<< pid << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcCpu(result, procCpu);

	sigar_proc_time_t* procTime = new sigar_proc_time_t();
	if(sigar_proc_time_get(sigarCore, pid, procTime)) {
		std::cout << "SIGAR couldn't acquire process time for pid: "
			<< pid << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcTime(result, procTime);

	sigar_proc_mem_t* procMem = new sigar_proc_mem_t();
	if(sigar_proc_mem_get(sigarCore, pid, procMem)) {
		std::cout << "SIGAR couldn't acquire process memory for pid: "
			<< pid << std::endl;
		return result;
	}
	InfoMapper::fillProcessDetailsWithSigarProcMemory(result, procMem);
	return result;
}

FileSystem* SystemInfo::DiagnosticMgr::getFileSystemInfo()
{
	if(!initialized) {
		std::cout << "Couldn't acquire file system info" << std::endl;
		return NULL;
	}

	sigar_file_system_list_t* fsAvail = new sigar_file_system_list_t();
	if(sigar_file_system_list_get(sigarCore, fsAvail)) {
		std::cout << "SIGAR couldn't acquire file system list info" << std::endl;
		return NULL;
	}
	FileSystem* result = new FileSystem();
	for(uint16 i = 0; i < fsAvail -> number; ++i) {
		result -> dirDetails.push_back(
			InfoMapper::sigarFileSystemToFsDetails(&(fsAvail -> data[i])));
	}
	std::vector<FileSystem::Details*> details = result -> dirDetails;
	for(auto it = details.begin(); it != details.end(); ++it) {
		std::string currentDir = (*it) -> dir;

		sigar_file_system_usage_t* fsDirUsage = new sigar_file_system_usage_t();
		if(sigar_file_system_usage_get(sigarCore, currentDir.c_str(), fsDirUsage)) {
			std::cout << "SIGAR couldn't acquire file system usage for dir: "
				<< currentDir << std::endl;
		} else {
			result -> dirUsages.push_back(
				InfoMapper::sigarFileSystemUsageToFsUsage(fsDirUsage, currentDir));			
		}
	}
	return result;
}

/*
 *	Solution by: Simon Mourier
 *	From: http://stackoverflow.com/questions/5327203/how-to-access-cpus-heat-sensors
 *
 *	For linking use: Wbemuuid.lib
 *	Running info: application must run under administrator privileges!
 */
double SystemInfo::DiagnosticMgr::getCpuTemp()
{
	LONG result;
	LPLONG pTemperature = &result;
	*pTemperature = -1;
	HRESULT ci = CoInitialize(NULL); // needs comdef.h
	HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL, EOAC_NONE, NULL);
	if (SUCCEEDED(hr))
	{
		IWbemLocator *pLocator; // needs Wbemidl.h & Wbemuuid.lib
		hr = CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER , IID_IWbemLocator, (LPVOID*)&pLocator);
		if (SUCCEEDED(hr))
		{
			IWbemServices *pServices;
			BSTR ns = SysAllocString(L"root\\WMI");
			hr = pLocator->ConnectServer(ns, NULL, NULL, NULL, 0, NULL, NULL, &pServices);
			pLocator->Release();
			SysFreeString(ns);
			if (SUCCEEDED(hr))
			{
				BSTR query = SysAllocString(L"SELECT * FROM MSAcpi_ThermalZoneTemperature");
				BSTR wql = SysAllocString(L"WQL");
				IEnumWbemClassObject *pEnum;
				hr = pServices->ExecQuery(wql, query, WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);
				SysFreeString(wql);
				SysFreeString(query);
				pServices->Release();
				if (SUCCEEDED(hr))
				{
					IWbemClassObject *pObject;
					ULONG returned;
					hr = pEnum->Next(-1, 1, &pObject, &returned);
					pEnum->Release();
					if (SUCCEEDED(hr))
					{
						BSTR temp = SysAllocString(L"CurrentTemperature");
						VARIANT v;
						VariantInit(&v);
						hr = pObject->Get(temp, 0, &v, NULL, NULL);
						pObject->Release();
						SysFreeString(temp);
						if (SUCCEEDED(hr))
						{
							*pTemperature = V_I4(&v);
						}
						VariantClear(&v);
					}
				}
			}
			if (ci == S_OK)
			{
				CoUninitialize();
			}
		}
	}
	double tempInTensOfKelvins = (double)*pTemperature;
	return (tempInTensOfKelvins > 0.0) ? ((double)*pTemperature / 10.0) - 273.15 : 0.0;
}
