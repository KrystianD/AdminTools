#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <vector>
#include <string>
using namespace std;

class buffer_t : public vector<char>
{
public:
	template<typename T>
	bool append (const T& val)
	{
		insert (end (), (char*)&val, (char*)&val + sizeof (T));
		return true;
	}
	template<typename T>
	bool append (const vector<T>& array)
	{
		printf ("vec %d\n", array.size ());
		uint16_t len = array.size ();
		append (len);
		for (int i = 0; i < len; i++)
		{
			const T& val = array[i];
			insert (end (), (char*)&val, (char*)&val + sizeof (T));
		}
		return true;
	}
};
// typedef vector<char> buffer_t;

int recvall (int sockfd, void* data, int size, int timeout);
int sendall (int sockfd, void* data, int size);
bool fileExists (const string& path);
bool fileAccessible (const string& path);

class ISerializable
{
public:
	virtual void toBuffer (buffer_t& buf) = 0;
	virtual bool fromBuffer (buffer_t& buf) = 0;

	template<typename T>
	bool append (buffer_t& buf, T& val)
	{
		buf.insert (buf.end (), (char*)&val, (char*)&val + sizeof (T));
		return true;
	}
	bool append (buffer_t& buf, string& val)
	{
		uint16_t len = val.size ();
		append (buf, len);
		const char *c = &val[0];
		while (len--)
			append (buf, *c++);
		return true;
	}
	template<typename T>
	bool fetch (buffer_t& buf, T& val)
	{
		if (m_pos + sizeof (T) > buf.size ())
			return false;
		memcpy (&val, &buf[m_pos], sizeof (T));
		m_pos += sizeof (T);
		return true;
	}
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
	int m_pos;
};

#endif
