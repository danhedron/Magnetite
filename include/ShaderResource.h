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
	~ShaderResource();

	/**
	 * Loads the shader from the source file
	 */
	void load();	
	
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
