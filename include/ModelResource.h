#ifndef _MODELRESOURCE_H_
#define _MODELRESOURCE_H_
#include "BaseResource.h"
#include "Renderer.h"

class MeshGeometry;

/**
 * @class ModelResource
 * Class for 3D-Models loaded into the engine,
 * Uses Assimp for loading a wide array of mesh formats.
 */
class ModelResource : public BaseResource
{
protected:
	/**
	 * Filename the model is loaded from
	 */
	std::string mFilename;
	
	/**
	 * Geometry for rendering.
	 */
	MeshGeometry* mGeometry;
	
public:
	/**
	 * Resource constructor 
	 */
    ModelResource(const std::string& file);
	virtual ~ModelResource();
	
	/**
	 * Loads the resouce into memory
	 */
	void load();
	
	/**
	 * Frees the memory associated with this resource.
	 */
	void unload();
	
	/**
	 * Draws the geometries with the current settings
	 */
	void draw();
};

/**
 * Convenience Typedef
 */
typedef ModelResource Model;
typedef Model* ModelPtr;

#endif