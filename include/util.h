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
};

#endif