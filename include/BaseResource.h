#ifndef _BASERESOURCE_H_
#define _BASERESOURCE_H_
#include "prerequisites.h"

class BaseResource
{
protected:
	std::string mFilename;
public:
	BaseResource( const std::string& file );
	~BaseResource();

	void load();

	void unload();
};

#endif
