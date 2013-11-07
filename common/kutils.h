#ifndef __KUTILS_H__
#define __KUTILS_H__

#include <stdint.h>
#include <string>
#include <vector>
#include <sys/types.h>

#include <vector>
using namespace std;

uint32_t getTicks ();
uint32_t getTicksUS ();

vector<string> explode (const string& str, const string& delim);
string trim (const string& str);

class Timer
{
	public:
		Timer () : m_interval (0), m_lastCheck (-1) { }

		void setInterval (uint32_t interval)
		{
			m_interval = interval;
		}

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
