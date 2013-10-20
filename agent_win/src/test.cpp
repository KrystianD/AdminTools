#include "SystemInfo/DiagnosticMgr.h"
using namespace SystemInfo;

int main()
{
	DiagnosticMgr& diagnosticMgr = DiagnosticMgr::getInstance();	

	diagnosticMgr.getMemoryInfo() -> displayCombinedInfo();
	diagnosticMgr.getResourcesInfo() -> displayCombinedInfo();
	diagnosticMgr.getCpuInfo() -> displayCombinedInfo();
	diagnosticMgr.getProcessesInfo() -> displayCombinedInfo();

	getchar();
}