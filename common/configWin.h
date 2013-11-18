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
