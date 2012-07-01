#ifndef _GLPROGRAM_H_
#define _GLPROGRAM_H_
#include "prerequisites.h"

class GLshader;

class GLprogram {
	/**
	 * OpenGL name of the program
	 */
	GLuint mName;
	
	/**
	 * Pointer to the GLshader object for the vertex shader.
	 */
	GLshader* mVertex;
	
	/**
	 * Pointer to the GLshader object for the fragment shader
	 */
	GLshader* mFragment;
	
	/**
	 * Optional pointer to the Geometry shader
	 */
	GLshader* mGeometry;
	
	/**
	 * True if the program has been linked succesfully
	 */
	bool mValid;
	
	/**
	 * Caching for uniform locations 
	 */
	std::map<std::string, GLuint> mUniforms;
	
	std::map<std::string, int> mAttributes;

	GLprogram();
	
	void link();
	
	void bindUniformTexture( std::string var, GLint unit );
	
	size_t getAttributeIndex( const std::string& attribute );
};


#endif