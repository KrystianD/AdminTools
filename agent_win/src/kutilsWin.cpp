#include "kutilsWin.h"

#include <stdio.h>
#include <time.h>
#include <Windows.h>
using namespace std;


namespace WinAgent
{

uint32_t getTicks () // na linuksie zwraca z dok³adnoœci¹ do milisekudny (tutaj do sekundy), na ile to jest wa¿ne?
{
	time_t sec = time(0);
	uint32_t val = sec*1000;
	return val;
}

vector<string> explode (const string& str, const string& delim)
{
  vector<string> parts;
  size_t idx = 0, delimIdx;

  delimIdx = str.find (delim, idx);
  if (delimIdx == string::npos) { parts.push_back (str); return parts; }
  do
  {
    string part = str.substr (idx, delimIdx - idx);
    parts.push_back (part);
    idx = delimIdx + delim.size ();
    delimIdx = str.find (delim, idx);
  } while (delimIdx != string::npos && idx < str.size ());

  if (idx < str.size ())
  {
    string part = str.substr (idx);
    parts.push_back (part);
  }

  return parts;
}

void usleep(__int64 usec) 
{ 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}

}