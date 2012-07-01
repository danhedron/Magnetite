#ifndef _PROGRAMRESOURCE_H_
#define _PROGAMRESOURCE_H_
#include "prerequisites.h"
#include "BaseResource.h"
class ShaderResource;

class ProgramResource : public BaseResource 
{
protected:
	/** 
	 * Shader Name 
	 */
	GLuint mName;

	/**
	 * Vertex Shader
	 */
	ShaderResource* mVertexShader;

	/**
	 * Geometry Shader
	 */
	ShaderResource* mGeometryShader;

	/**
	 * Fragment Shader
	 */
	ShaderResource* mFragmentShader;
	
	/**
	 * Cache for attribute indicies
	 */
	std::map<std::string, int> mAttributes;
	
	/**
	 * Cache for uniform locations
	 */
	std::map<std::string, int> mUniforms;

public:
	ProgramResource( std::string file );
	~ProgramResource();

	/**
	 * Loads the shader from the source file
	 */
	void load();

	/**
	 * Sets the Vertex shader on this program
	 */
	void setVertexShader( ShaderResource* shader );

	/**
	 * Sets the Geometry shader on this program
	 */
	void setGeometryShader( ShaderResource* shader );

	/**
	 * Sets the fragment shader
	 */
	void setFragmentShader( ShaderResource* shader );	
	
	/**
	 * Makes this the active program.
	 */
	void makeActive();

	/**
	 * Links the shader
	 */
	void link();

	/**
	 * Returns the OpenGL name of this program
	 */
	GLuint getName();
	
	/**
	 * Returns the index of an attribute
	 */
	GLint getAttributeIndex( const std::string& attr );
	
	/**
	 * Returns the location of a uniform
	 */
	GLint getUniformLocation( const std::string& uni );
};

#endif
