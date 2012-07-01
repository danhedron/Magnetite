#include "glgeometry.h"

GLgeometry::GLgeometry()
: vertexData(NULL),
edgeData(NULL),
vertexCount(0),
edgeCount(0),
vertexBO(0),
indexBO(0)
{
	
}

GLgeometry::~GLgeometry()
{
	releaseBuffer(); 
	delete[] edgeData; 
	delete[] vertexData;
}

void GLgeometry::releaseBuffer()
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

void GLgeometry::bindToBuffer()
{
	glGenBuffers(1, &this->vertexBO);
	glBindBuffer( GL_ARRAY_BUFFER, this->vertexBO);
	glBufferData( GL_ARRAY_BUFFER, sizeof(GLvertex)*this->vertexCount+1, this->vertexData, GL_STATIC_DRAW );

	glGenBuffers(1, &this->indexBO);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->indexBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLedge)*this->edgeCount+1, this->edgeData, GL_STATIC_DRAW );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLvertex GLgeometry::vertex(float x, float y, float z, float u, float v, float l)
{
	GLvertex vert;
	vert.x = x;
	vert.y = y;
	vert.z = z;
	vert.u0 = (GLubyte)u;
	vert.v0 = (GLubyte)v;
	vert.l = (GLubyte)(l*255);
	return vert;
}
