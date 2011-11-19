#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_
#include "prerequisites.h"
class BaseResource;

typedef std::map<std::string, std::string> ResourceMap;
typedef std::map<std::string, BaseResource*> Resources;

class ResourceManager 
{
protected:
	ResourceMap	mResourceIndex;
	Resources	mResources;
public:

	~ResourceManager();

	template<class T> T* getResource( const std::string& file )
	{
		Resources::iterator it = mResources.find(file);
		if( it != mResources.end() ) {
			return (T*)it->second;
		}

		ResourceMap::iterator loc = mResourceIndex.find(file);
		if( loc != mResourceIndex.end() ) {
			// Create a new resource for this wotsit
			T* resource = new T(loc->second);
			Util::log("Loaded resource: " + loc->first);
			mResources[loc->first] = resource;
			return resource;
		}

		Util::log("Unable to find resource: " + file);

		// This resource doesn't exist
		return NULL;
	}
	
	/**
	 * Adds a folder to the resource list
	 * @param loc Location of folder
	 */
	void addLocation( const std::string& loc );

	/**
	 * Iterates through a folder, indexing the resources in it
	 * @param loc Location of folder
	 */
	void indexFolder( const std::string& loc );
};

#endif
