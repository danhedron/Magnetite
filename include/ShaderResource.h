#ifndef _SHADERRESOURCE_H_
#define _SHADERRESOURCE_H_
#include "prerequisites.h"
#include "BaseResource.h"

class ShaderResource : public BaseResource 
{
protected:
	/**
	 * Shader Source Code
	 */
	std::string mSource; 
	
	/** 
	 * Shader Name 
	 */
	GLuint mName;

	/**
	 * Shader type
	 */
	GLenum mType;

public:
	ShaderResource( std::string file );
	virtual ~ShaderResource();

	/**
	 * Loads the shader from the source file
	 */
	virtual void load();
	
	/**
	 * Does nothing.
	 */
	virtual void unload() {};
	
	virtual bool isLoaded() const;
	
	/**
	 * Compiles the shader on the GPU
	 */
	void compile( GLenum type );

	/**
	 * Returns the OpenGL name 
	 */
	GLuint getName();
};

#endif
