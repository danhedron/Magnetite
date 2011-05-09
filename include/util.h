#ifndef _UTIL_H_
#define _UTIL_H_
#include "prerequisites.h"

#ifdef _WIN32
#include <psapi.h>
#endif


// Simple 3d vector
struct Vector3 {
	float x,y,z,w;
	Vector3( float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f ) {
		x = _x; y = _y; z = _z; w = _w;
	}
	Vector3 operator- () {
		return Vector3(-x,-y,-z);
	}
};

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

	static void getMemoryUsage(size_t &mainCurrent, size_t &mainMax, size_t &pagedCurrent, size_t &pagedMax, size_t &pageFault)
	{
#ifdef _WIN32
		HANDLE hProcess;
		PROCESS_MEMORY_COUNTERS pmc;

		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
								PROCESS_VM_READ,
								FALSE,
								GetCurrentProcessId() );

		if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc) ) ) {
			mainCurrent = pmc.WorkingSetSize;
			mainMax = pmc.PeakWorkingSetSize;
			pagedCurrent = pmc.PagefileUsage;
			pagedMax = pmc.PeakPagefileUsage;
			pageFault = pmc.PageFaultCount;
		}

		CloseHandle( hProcess );
#endif
	}

	static void log(const std::string text)
	{
		std::cout << "| " << text << std::endl;
	}

	static std::string toString(int number)
	{
		return itoa(number, 10);
	}

	static std::string toString(Vector3 &vec)
	{
		return itoa(vec.x, 10) + "," + itoa(vec.y, 10) + "," + itoa(vec.z, 10);
	}

	/**
	 * C++ version std::string style "itoa":
	*/
	static std::string itoa(int value, int base) {
		enum { kMaxDigits = 35 };
		std::string buf;
		buf.reserve( kMaxDigits ); // Pre-allocate enough space.

		// check that the base if valid

		if (base < 2 || base > 16) return buf;
		int quotient = value;
		// Translating number to string with base:
		do {
			buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
			quotient /= base;

		} while ( quotient );

		// Append the negative sign for base 10
		if ( value < 0 && base == 10) buf += '-';
		std::reverse( buf.begin(), buf.end() );
		return buf;
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
