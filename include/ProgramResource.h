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
	 * Makes this shader active (for rendering and shit)
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
};

#endif
