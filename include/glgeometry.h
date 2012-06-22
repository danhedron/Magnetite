#ifndef _GLGEOMETRY_H_
#define _GLGEOMETRY_H_
#include "prerequisites.h"

/**
 * @struct Struct to represent a vertex, memory aligned for easy array-xing
 */
struct GLvertex {
	float x, y, z;
	/*float nx, ny, nz;*/ // Normals are undeeded at the moment.
	GLubyte u0, v0;
	GLubyte l;
	GLubyte b; // Is this even?
};

typedef unsigned short GLedge;

struct GLgeometry {
	/**
	 * Geometry vertex data
	 */
	GLvertex* vertexData;
	
	/**
	 * Geometry edge data
	 */
	GLedge* edgeData;
	
	/**
	 * Number of vertices in vertexData
	 */
	size_t vertexCount;
	
	/**
	 * Number of edges in edgeData
	 */
	size_t edgeCount;
	
	/**
	 * Vertex Buffer object names
	 */
	GLuint vertexBO;
	
	/**
	 * Edge buffer object names
	 */
	GLuint indexBO;

	/**
	 * Constructor
	 */
	GLgeometry();
	
	/**
	 * Destructor
	 */
	~GLgeometry();
	void releaseBuffer();
	void bindToBuffer();
	
	/**
	 * Utility function for constructing a vertex
	 */
	static GLvertex vertex( float x, float y, float z, float u, float v, float l );

};


#endif