#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_
#include "prerequisites.h"

class ProgramResource;
/**
 * @struct Struct to represent a terrain vertex, memory aligned for performance.
 */
struct TerrainVertex {
	float x, y, z; // 8, 8, 8, 24 bytes
	GLubyte u0, v0, l; // 1, 1, 1, 27 bytes
	GLbyte p; // padding, 28 bytes.
};

/**
 * @struct Struct to represent a normal vertex, memory aligned for performance.
 */
struct GeometryVertex {
	float x, y, z; // 8, 8, 8, 24 bytes
	float u0, v0; // 8, 8, 40 bytes.
};

typedef unsigned short GLedge;

class Geometry {
public:
	/**
	 * Edge data for the geometry, shared over all types of geometry
	 */
	GLedge* edgeData;
	
	/**
	 * The number of vertices stored in the Geometry
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
	Geometry();
	
	/**
	 * Destructor
	 */
	virtual ~Geometry();
	
	/**
	 * Instruct the geometry to release it's buffer
	 */
	virtual void releaseBuffer();
	
	/**
	 * Bind the data to a GPU buffer
	 */
	virtual void bindToBuffer() = 0;
	
	/**
	 * Set the given vertex attribute with data from the vertices
	 */
	virtual void bindVertexAttributes( ProgramResource* program ) = 0;
	
	/**
	 * Utility function for constructing a vertex
	 */
	static TerrainVertex terrainVertex( float x, float y, float z, float u = 0.f, float v = 0.f, float l = 0.f );
	static GeometryVertex vertex( float x, float y, float z, float u = 0.f, float v = 0.f, float l = 0.f );

};

class MeshGeometry : public Geometry {
public:
	/**
	 */
	GeometryVertex* vertexData;
	
	MeshGeometry();
	
	virtual ~MeshGeometry();
	
	virtual void bindToBuffer();
	
	virtual void bindVertexAttributes( ProgramResource* program );
};

class TerrainGeometry : public Geometry {
public:
	/**
	 * Verterx data
	 */
	TerrainVertex* vertexData;
	
	/**
	 * Constructor
	 */
	TerrainGeometry();
	
	/**
	 * Destructor
	 */
	virtual ~TerrainGeometry();
	
	/**
	 * Bind the data to a GPU buffer
	 */
	virtual void bindToBuffer();
	
	virtual void bindVertexAttributes( ProgramResource* program );
	
};

#endif