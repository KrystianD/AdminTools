#pragma once

#include "Types.h"
#include "ModuleInfo.h"

namespace SystemInfo
{
	class Memory : public ModuleInfo
	{
		public:
			Memory()
			{
				used_percent = free_percent = 0.0;
				ram = total = used = free = used_actual =
					free_actual = total_swap = used_swap =
					free_swap = page_in = page_out = 0;
			}

			// general memory info
			uint64 ram;
			uint64 total;
			uint64 used;
			uint64 free;
			uint64 used_actual;
			uint64 free_actual;
			double used_percent;
			double free_percent;

			// swap info
			uint64 total_swap;
			uint64 used_swap;
			uint64 free_swap;
			uint64 page_in;
			uint64 page_out;

			void displayCombinedInfo()
			{
				displayModuleHeader("Memory");

				displayGroupHeader("General Info");
				displaySingleInfo("RAM", ram);
				displaySingleInfo("Total", total);
				displaySingleInfo("Used", used);
				displaySingleInfo("Free", free);
				displaySingleInfo("Actual used", used_actual);
				displaySingleInfo("Actual free", free_actual);
				displaySingleInfo("Used %", used_percent);
				displaySingleInfo("Free %", free_percent);

				displayGroupHeader("Swap Info");
				displaySingleInfo("Total", total_swap);
				displaySingleInfo("Used", used_swap);
				displaySingleInfo("Free", free_swap);
				displaySingleInfo("Page In", page_in);
				displaySingleInfo("Page Out", page_out);
			}
	};

}