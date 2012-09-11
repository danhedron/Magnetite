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

	virtual void load() = 0;

	virtual void unload() = 0;
	
	/**
	 * @return true if the resource is loaded
	 */
	virtual bool isLoaded() const = 0;
};

#endif
