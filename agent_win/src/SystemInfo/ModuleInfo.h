#pragma once

#include <string>
#include <iostream>

namespace SystemInfo
{
	/**
	 *	\class ModuleInfo
	 *	\brief Pure abstract class for module info (Cpu, Memory etc.).
	 *
	 *	Provides method for easy displaying / formating system informations.
	 */
	class ModuleInfo
	{
		public:

			/**
			 *	\fn virtual void displayCombinedInfo() = 0;
			 *	\brief Display full informations from module instance.
			 * 	\return None.
			 */
			virtual void displayCombinedInfo() = 0;

			/**
			 *	\fn void displayModuleHeader(std::string name)
			 *	\brief Display given module header using standard output (std::cout).
			 *	\param name Header to display.
			 *	\return None.
			 */
			void displayModuleHeader(std::string name) {
				std::cout << "======= " << name << " =======" << std::endl;
			}

			/**
			 *	\fn void displayGroupHeader(std::string name)
			 *	\brief Display given group header using standard output (std::cout).
			 *	\param name Group header to display.
			 *	\return None.
			 */
			void displayGroupHeader(std::string name) {
				std::cout << "[" << name << "]" << std::endl;
			}

			/**
			 *	\fn template<typename T> void displaySingleInfo(std::string description, T value)
			 *	\brief Display single information as pair of label and parameterized value.
			 *	\param description Description label for value.
			 *	\param value Parameterized value to display.
			 *	\return None;
			 */
			template<typename T>
			void displaySingleInfo(std::string description, T value) {
				std::cout << "\t" << description << ": "
					<< value << std::endl;
			}

			/**
			 *	\fn template<typename T, typename V> void displayPairInfo(std::string description, T current, V max)
			 *	\brief Display signle information as pair of label and parameterized pair value of current/max status.
			 *	\param description Description label for value.
			 *	\param current Parameterized current value to display.
			 *	\param max Parameterized maximum value to display.
			 *	\return None.
			 */
			template<typename T, typename V>
			void displayPairInfo(std::string description, T current, V max) {
				std::cout << "\t" << description << ": "
					<< current << "/" << max << std::endl;
			}
	};
}