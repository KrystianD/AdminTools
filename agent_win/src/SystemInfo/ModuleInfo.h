#pragma once

#include <string>
#include <iostream>

namespace SystemInfo
{
	class ModuleInfo
	{
		public:
			
			virtual void displayCombinedInfo() = 0;					

			void displayModuleHeader(std::string name) {
				std::cout << "======= " << name << " =======" << std::endl;
			}
			
			void displayGroupHeader(std::string name) {
				std::cout << "[" << name << "]" << std::endl;
			}

			template<typename T>
			void displaySingleInfo(std::string description, T value) {
				std::cout << "\t" << description << ": " 
					<< value << std::endl;
			}

			template<typename T, typename V>
			void displayPairInfo(std::string description, T current, V max) {
				std::cout << "\t" << description << ": " 
					<< current << "/" << max << std::endl;
			}
	};
}