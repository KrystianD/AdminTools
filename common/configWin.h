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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <map>
using namespace std;

/**
 *	\class Config
 *	\brief Simple handler for configuration files.
 */
class Config
{
public:
	/**
	 *	\fn	Config()
	 *	\brief Default constructor, do nothing.
	 */
	Config ();
	/**
	 *	\fn ~Config()
	 *	\brief Destructor, do nothing.
	 */
	~Config ();

	/**
	 *	\fn bool fromFile (const string& path)
	 *	\brief Load configuration from given file.
	 *	\param path Path pointing to configuration file.
	 *	\return If succeeded.
	 */
	bool fromFile (const string& path);
	/**
	 *	\fn bool saveToFile (const string& path)
	 *	\brief Save configuration to given file.
	 *	\param path Path pointing to configuration file.
	 *	\return If succeeded.
	 */
	bool saveToFile (const string& path);

	/**
	 *	\fn bool hasKey (const string& key)
	 *	\brief Check if configuration contains given key.
	 *	\param key Key to check.
	 *	\return If contained.
	 */
	bool hasKey (const string& key);

	/**
	 *	\fn string getString (const string& key, const string& def = "")
	 *	\brief Get string value connected with given key.
	 *	\param key Key connected with wanted value.
	 *	\param def Default value in case of absence of given key.
	 *	\return Value connected with key or default value def otherwise.
	 */
	string getString (const string& key, const string& def = "");
	/**
	 *	\fn int getInt (const string& key, int def = 0)
	 *	\brief Get int value connected with given key.
	 *	\param key Key connected with wanted value.
	 *	\param def Default value in case of absence of given key.
	 *	\return Value connected with key or default value def otherwise.
	 */
	int getInt (const string& key, int def = 0);
	/**
	 *	\fn void setString (const string& key, const string& value)
	 *	\brief Insert string value and assign it with given key.
	 *	\param key Key connected with inserted value.
	 *	\param value Inserted value.
	 *	\return None.
	 */
	void setString (const string& key, const string& value);
	/**
	 *	\fn void setInt (const string& key, int val)
	 *	\brief Insert int value and assign it with given key.
	 *	\param key Key connected with inserted value.
	 *	\param value Inserted value.
	 *	\return None.
	 */
	void setInt (const string& key, int val);

private:
	map<string,string> m_data;
};

#endif
