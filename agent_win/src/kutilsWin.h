#ifndef __KUTILS_H__
#define __KUTILS_H__

#include <stdint.h>
#include <string>
#include <vector>

#include <vector>
using namespace std;
/**
 *	\fn vector<string> explode (const string& str, const string& delim)
 *	\brief Split given text by delimiter.
 *	\param str Text to explode.
 *	\param delim Delimiter.
 *	\return Splitted strings.
 */
vector<string> explode (const string& str, const string& delim);
/**
 *	\fn uint32_t getTicks()
 *	\brief Get time elapsed since Epoch in milliseconds.
 *	\return Time elapsed in milliseconds.
 */
uint32_t getTicks ();

#endif