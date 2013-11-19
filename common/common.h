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
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <vector>
#include <string>
using namespace std;

/**
 *	\class buffer_t
 *	\brief Byte buffer used for marshalling.
 *
 *	Implemented as stl vector of chars.
 */
class buffer_t : public vector<char>
{
public:
	/**
	 *	\fn template<typename T> bool append (const T& val)
	 *	\brief Append single data to buffer.
	 *	\param val Data to append.
	 *	\return If succeeded.
	 */
	template<typename T>
	bool append (const T& val)
	{
		insert (end (), (char*)&val, (char*)&val + sizeof (T));
		return true;
	}
	/**
	 *	\fn template<typename T> bool append (const vector<T>& array)
	 *	\brief Append vector of data to buffer.
	 *	\param array Data to append.
	 *	\return If succeeded.
	 */
	template<typename T>
	bool append (const vector<T>& array)
	{
		uint16_t len = array.size ();
		append (len);
		for (int i = 0; i < len; i++)
		{
			const T& val = array[i];
			insert (end (), (char*)&val, (char*)&val + sizeof (T));
		}
		return true;
	}

	/**
	 *	\fn	template<typename T> bool fetch (T& val)
	 *	\brief Fetch parametrized data from buffer last position.
	 *	\param[out] val Fetched value.
	 *	\return If succeeded.
	 */
	template<typename T>
	bool fetch (T& val)
	{
		if (m_pos + sizeof (T) > size ())
			return false;
		memcpy (&val, data () + m_pos, sizeof (T));
		m_pos += sizeof (T);
		return true;
	}
	/**
	 *	\fn	bool fetch (string& val)
	 *	\brief Fetch string from buffer last position.
	 *	\param[out] val Fetched string.
	 *	\return If succeeded.
	 */
	bool fetch (string& val)
	{
		uint16_t len;
		fetch (len);
		val = "";
		while (len--)
		{
			char c;
			fetch (c);
			val += c;
		}
		return true;
	}

	/**
	 *	\fn	void rewind ()
	 *	\brief Reset internal pointer
	 */
	void rewind ()
	{
		m_pos = 0;
	}

private:
	int m_pos;
};

/**
 *	\fn int recvall (int sockfd, void* data, int size, int timeout)
 *	\brief Receive data from given socket.
 *	\param sockfd Socket descriptor.
 *	\param data Pointer to received data.
 *	\param size Number of bytes to be read.
 *	\param timeout Time to wait for data.
 *	\return Size of received data.
 */
int recvall (int sockfd, void* data, int size, int timeout);
/**
 *	\fn int sendall (int sockfd, void* data, int size)
 *	\brief Send data to given socket.
 *	\param sockfd Socket descriptor.
 *	\param data Pointer to data to send.
 *	\param size Number of bytes to be send.
 *	\return Size of data send.
 */
int sendall (int sockfd, void* data, int size);
/**
 *	\fn	bool fileExists (const string& path)
 *	\brief Check if given file exists.
 *	\param path Path pointing to file to be checked.
 *	\return If exist.
 */
bool fileExists (const string& path);
/**
 *	\fn	bool fileAccessible (const string& path)
 *	\brief Check if given file is accessible.
 *	\param path Path pointing to file to be checked.
 *	\return If accessible.
 */
bool fileAccessible (const string& path);

/**
 * 	\interface ISerializable
 *	\brief Object able to serialize himself into/from buffer_t class.
 */
class ISerializable
{
public:
	/**
	 *	\fn virtual void toBuffer (buffer_t& buf) = 0
	 *	\brief Convert object to byte buffer.
	 *	\param[out] buf Byte buffer representation of object.
	 *	\return None.
	 */
	virtual void toBuffer (buffer_t& buf) = 0;
	/**
	 *	\fn virtual bool fromBuffer (buffer_t& buf) = 0
	 *	\brief Fill object data with given buffer.
	 *	\param[in] buf Buffer with object data.
	 *	\return If succeeded.
	 */
	virtual bool fromBuffer (buffer_t& buf) = 0;

	/**
	 *	\fn template<typename T> bool append (buffer_t& buf, T& val)
	 *	\brief Append parametrized value to end of buffer.
	 *	\param[out] buf Target buffer.
	 *	\param[in] val Value to append.
	 *	\return If succeeded.
	 */
	template<typename T>
	bool append (buffer_t& buf, T& val)
	{
		buf.insert (buf.end (), (char*)&val, (char*)&val + sizeof (T));
		return true;
	}
	/**
	 *	\fn bool append (buffer_t& buf, string& val)
	 *	\brief Append string to end of buffer.
	 *	\param[out] buf Target buffer.
	 *	\param[in] val String to append.
	 *	\return If succeeded.
	 */
	bool append (buffer_t& buf, string& val)
	{
		uint16_t len = val.size ();
		append (buf, len);
		const char *c = &val[0];
		while (len--)
			append (buf, *c++);
		return true;
	}
	/**
	 *	\fn	template<typename T> bool fetch (buffer_t& buf, T& val)
	 *	\brief Fetch parametrized data from buffer last position.
	 *	\param[in] buf Source buffer.
	 *	\param[out] val Fetched value.
	 *	\return If succeeded.
	 */
	template<typename T>
	bool fetch (buffer_t& buf, T& val)
	{
		if (m_pos + sizeof (T) > buf.size ())
			return false;
		memcpy (&val, &buf[m_pos], sizeof (T));
		m_pos += sizeof (T);
		return true;
	}
	/**
	 *	\fn	bool fetch (buffer_t& buf, string& val)
	 *	\brief Fetch string from buffer last position.
	 *	\param[in] buf Source buffer.
	 *	\param[out] val Fetched string.
	 *	\return If succeeded.
	 */
	bool fetch (buffer_t& buf, string& val)
	{
		uint16_t len;
		fetch (buf, len);
		val = "";
		while (len--)
		{
			char c;
			fetch (buf, c);
			val += c;
		}
		return true;
	}

protected:
	//! Buffer cursor
	int m_pos;
};

#endif
