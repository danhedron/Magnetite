#include "Geometry.h"
#include <ProgramResource.h>

Geometry::Geometry()
:edgeData(NULL),
vertexCount(0),
edgeCount(0),
vertexBO(0),
indexBO(0)
{
	
}

Geometry::~Geometry()
{
	releaseBuffer(); 
	delete[] edgeData; 
}

void Geometry::releaseBuffer()
{
	if( this->vertexBO != 0 ) {
		glDeleteBuffers( 1, &this->vertexBO );
		this->vertexBO = 0;
	}
	if( this->indexBO != 0 ) {
		glDeleteBuffers( 1, &this->indexBO );
		this->indexBO = 0;
	}
}

void Geometry::bind()
{
	if( this->vertexBO != 0 )
	{
		glBindBuffer( GL_ARRAY_BUFFER, this->vertexBO );
	}
	
	if( this->indexBO != 0 )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->indexBO );
	}
}

void Geometry::unbind()
{
	if( this->vertexBO != 0 )
	{
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
	
	if( this->indexBO != 0 )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}
}

TerrainVertex Geometry::terrainVertex(float x, float y, float z, float u, float v, float l)
{
	TerrainVertex vert;
	vert.x = x;
	vert.y = y;
	vert.z = z;
	vert.u0 = (GLubyte)u;
	vert.v0 = (GLubyte)v;
	vert.l = (GLubyte)(l*255);
	return vert;
}

GeometryVertex Geometry::vertex(float x, float y, float z, float u, float v, float l)
{
	GeometryVertex vert;
	vert.x = x;
	vert.y = y;
	vert.z = z;
	vert.u0 = u;
	vert.v0 = v;
	return vert;
}

MeshGeometry::MeshGeometry()
{
	
}

MeshGeometry::~MeshGeometry()
{
	
}

void MeshGeometry::bindToBuffer()
{
	glGenBuffers(1, &this->vertexBO);
	glBindBuffer( GL_ARRAY_BUFFER, this->vertexBO);
	glBufferData( GL_ARRAY_BUFFER, sizeof(GeometryVertex)*this->vertexCount+1, this->vertexData, GL_STATIC_DRAW );

	glGenBuffers(1, &this->indexBO);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->indexBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLedge)*this->edgeCount+1, this->edgeData, GL_STATIC_DRAW );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshGeometry::bindVertexAttributes( ProgramResource* program )
{
	program->setVertexAttribute("vertex_position", 3, GL_FLOAT, GL_FALSE, sizeof(GeometryVertex), BUFFER_OFFSET(0) );
	
}

TerrainGeometry::TerrainGeometry()
{
	
}

TerrainGeometry::~TerrainGeometry()
{
	
}

void TerrainGeometry::bindToBuffer()
{
	glGenBuffers(1, &this->vertexBO);
	glBindBuffer( GL_ARRAY_BUFFER, this->vertexBO);
	glBufferData( GL_ARRAY_BUFFER, sizeof(TerrainVertex)*this->vertexCount+1, this->vertexData, GL_STATIC_DRAW );

	glGenBuffers(1, &this->indexBO);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->indexBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLedge)*this->edgeCount+1, this->edgeData, GL_STATIC_DRAW );

	glBindBuffer( GL_ARRAY_BUFFER, 0);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TerrainGeometry::bindVertexAttributes( ProgramResource* program)
{
	program->setVertexAttribute("in_vertex", 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), BUFFER_OFFSET(0) );
	
	program->setVertexAttribute("in_params", 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(TerrainVertex), BUFFER_OFFSET(12) );
}
