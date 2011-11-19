#include "ResourceManager.h"
#include "BaseResource.h"
#include <dirent.h>

ResourceManager::~ResourceManager()
{
	for( Resources::iterator it = mResources.begin(); it != mResources.end(); it++ )
	{
		delete it->second;
	}
	mResources.erase(mResources.begin(), mResources.end());	
}

void ResourceManager::addLocation( const std::string& loc )
{
	Util::log("Adding Location: " + loc);
	// indexFolder does everything with regards to the indexing 
	indexFolder( loc );
}

void ResourceManager::indexFolder( const std::string& loc )
{
	DIR* d = opendir( loc.c_str() );
	dirent* dir = readdir(d);
	while( dir != NULL ) {
		if( dir->d_type == DT_REG) {
			Util::log("\tRIndex: " + std::string(dir->d_name));
			mResourceIndex[dir->d_name] = loc + std::string(dir->d_name);
		}
		dir = readdir(d);
	}
	closedir(d);
}
