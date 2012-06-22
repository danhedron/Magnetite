#ifndef _GLSHADER_H_
#define _GLSHADER_H_
#include "prerequisites.h"

class GLshader {
	/**
	 * OpenGL Object name
	 */
	GLuint mName;
	
	/**
	 * Shader type
	 */
	GLenum mType;
	
	/**
	 * Shader source code string
	 */
	std::string mSource;
	
	/**
	 * Shader filename
	 */
	std::string mFilename;
	
public:
	
	/**
	 * Constructor, only stores the data
	 */
	GLshader( const std::string& src, const std::string& filename = "Unamed Shader" );
	
	/**
	 * Deconstructor
	 * Frees data if the shader has been compiled onto the GPU
	 */
	~GLshader();
	
	/**
	 * puts the shader together
	 */
	void create();
	
	/**
	 * Sets the shader type
	 */
    void setType(GLenum type);
	
	/**
	 * Returns the OpenGL name for the shader
	 */
	GLuint getName();
	
	/**
	 * Returns true if the shader is valid
	 */
	bool isValid();
};

#endif