#ifndef _UTIL_H_
#define _UTIL_H_
#include "prerequisites.h"

class Util
{
public:
	// String split function
	static stringvector &split(const std::string &s, char delim, stringvector &elems) {
		std::stringstream ss(s);
		std::string item;
		while(std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
	static stringvector split(const std::string &s, char delim) {
		stringvector elems;
		return Util::split(s, delim, elems);
	}

	static void log(const std::string text)
	{
		std::cout << "| " << text << std::endl;
	}

	static std::string toString(int number)
	{
		char* buff = new char[100];
		_itoa_s(number, buff, 100, 10);
		return std::string( buff );
	}

	// Causes Ambiguity errors.
	/*static std::string toString(bool t)
	{
		if( t )
			return "true";
		return "false";
	}*/
};

#endif