#ifndef _UTIL_H_
#define _UTIL_H_
#include "prerequisites.h"

#ifdef _WIN32
#include <psapi.h>
#endif

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

	static Vector3 indexToPosition( unsigned int index )
	{
		Vector3 v;
		v.x = index % CHUNK_WIDTH;
		v.z = floorf(index / (CHUNK_WIDTH * CHUNK_HEIGHT));
		v.y = (index - v.x - ( v.z * (CHUNK_WIDTH * CHUNK_HEIGHT)))/CHUNK_WIDTH;	
		return v;
	}

	static int clamp( int v, int m )
	{
		if( v < 0 ) return 0;
		if( v >= m ) return m - 1;
		return v;
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

	enum LogLevel {
		Verbose = 1,
		Info = 2,
		Warning = 3,
		Error = 4
	};

private:
	static LogLevel _loglevel;
public:

	static void setLogLevel( const LogLevel ll )
	{
		_loglevel = ll;
	}

	static void log(const std::string text, const LogLevel ll = Info )
	{
		if( ll >= _loglevel )
			std::cout << "| " << text << std::endl;
	}

	static std::string toString(size_t number)
	{
		std::stringstream ss;
		ss << number;
		return ss.str();
	}

	static std::string toString(int number)
	{
		return itoa(number, 10);
	}

	static std::string toString(long number)
	{
		return itoa(number, 10);
	}

	static std::string toString(GLuint& number)
	{
		return itoa(number, 10);
	}

	static std::string toString(float number)
	{
		std::stringstream ss;
		ss << number;
		return ss.str();
	}

	static std::string toString( const bool b )
	{
		return b ? "True" : "False";
	}

	static std::string toString(const Vector3 &vec)
	{
		return toString(vec.x) + "," + toString(vec.y) + "," + toString(vec.z);
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
