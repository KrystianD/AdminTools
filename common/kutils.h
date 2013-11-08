#ifndef __KUTILS_H__
#define __KUTILS_H__

#include <stdint.h>
#include <string>
#include <vector>
#include <sys/types.h>

#include <vector>
using namespace std;

/**
 *	\fn uint32_t getTicks()
 *	\brief Get time elapsed since Epoch in milliseconds.
 *	\return Time elapsed in milliseconds.
 */
uint32_t getTicks ();
/**
 *	\fn uint32_t getTicksUS()
 *	\brief Get time elapsed since Epoch in microseconds.
 *	\return Time elapsed in microseconds.
 */
uint32_t getTicksUS ();
/**
 *	\fn vector<string> explode (const string& str, const string& delim)
 *	\brief Split given text by delimiter.
 *	\param str Text to explode.
 *	\param delim Delimiter.
 *	\return Splitted strings.
 */
vector<string> explode (const string& str, const string& delim);
/**
 *	\fn string trim (const string& str)
 *	\brief Reduce text from unnecessary whitespaces at begin and end.
 *	\param str Text to trim.
 *	\return Trimmed text.
 */
string trim (const string& str);

/**
 *	\class Timer
 *	\brief Counts steps between the time interval.
 */
class Timer
{
	public:
		/**
		 *	\fn Timer()
		 *	\brief Default constructor, zeroes time interval.
		 */
		Timer () : m_interval (0), m_lastCheck (-1) { }

		/**
		 *	\fn void setInterval(uint32_t interval)
		 *	\brief Set interval to count to given value.
		 *	\param interval Time interval.
		 *	\return None.
		 */
		void setInterval (uint32_t interval)
		{
			m_interval = interval;
		}

		/**
		 *	\fn bool process()
		 *	\brief Check if interval passed.
		 *	\return If passed.
		 */
		bool process ()
		{
			uint64_t curTime = getTicks ();

			if (curTime - m_lastCheck > m_interval)
			{
				m_lastCheck = curTime;
				return true;
			}
			return false;
		}

	private:
		uint32_t m_interval;
		uint32_t m_lastCheck;
};

#endif
