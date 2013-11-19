/*
Copyright (c) 2013, AGH University of Science and Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __KUTILS_H__
#define __KUTILS_H__

#include <stdint.h>
#include <string>
#include <vector>

#include <vector>
using namespace std;

namespace WinAgent
{
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

/**
  *  \fn void usleep(__int64 usec)
  *  \brief holds for usec microseconds
  *  \param usec microseconds
 */
void usleep(__int64 usec);

}

#endif