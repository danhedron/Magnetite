#include "ModelResource.h"
#include <util.h>
#include "Renderer.h"

#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>

#include "glgeometry.h"

ModelResource::ModelResource(const std::string& file)
: BaseResource(file),
mFilename(file),
mGeometry(NULL)
{
	
}

ModelResource::~ModelResource()
{
	
}

void ModelResource::load()
{
	if( mGeometry != nullptr ) return;
	Assimp::Importer imp;
	
	const aiScene* mdlScene = imp.ReadFile(mFilename.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices 
	);
	
	if( !mdlScene ) {
		Util::log(imp.GetErrorString());
	}
	
	for( size_t i = 0; i < mdlScene->mNumMeshes; i++ ) {
		aiMesh* m = mdlScene->mMeshes[i];
		if(m->HasBones()) {
			Util::log("Skeletons not supported.");
		}
		
		mGeometry = new GLgeometry;
		
		mGeometry->vertexData = new GLvertex[m->mNumVertices];
		mGeometry->edgeData = new GLedge[m->mNumFaces*3];
		mGeometry->vertexCount = m->mNumVertices;
		mGeometry->edgeCount = m->mNumFaces*3;
		
		for( size_t v = 0; v < m->mNumVertices; v++ )
		{
			aiVector3D vec = m->mVertices[v];
			mGeometry->vertexData[v].x = vec.x;
			mGeometry->vertexData[v].y = vec.y;
			mGeometry->vertexData[v].z = vec.z;
			mGeometry->vertexData[v].l = 1;
		}
		
		for( size_t f = 0; f < m->mNumFaces; f+=3 )
		{
			for( size_t ind = 0; ind < m->mNumFaces; ind++ )
			{
				mGeometry->edgeData[f+ind] = m->mFaces[f].mIndices[ind];
			}
		}
		
		mGeometry->bindToBuffer();
	}
}

void ModelResource::draw()
{
	glBindBuffer( GL_ARRAY_BUFFER, mGeometry->vertexBO );
	glVertexPointer( 3, GL_FLOAT, sizeof(GLvertex), BUFFER_OFFSET(0) );
	
	// Todo here: Get some kind of GLprogram involved.

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mGeometry->indexBO );
	
	glDrawRangeElements( GL_TRIANGLES, 0, mGeometry->vertexCount, mGeometry->edgeCount, GL_UNSIGNED_SHORT, 0);
}