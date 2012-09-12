#include "ModelResource.h"
#include <util.h>
#include "Renderer.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "Geometry.h"

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
		aiProcess_FlipWindingOrder |
		aiProcess_JoinIdenticalVertices 
	);
	
	if( !mdlScene ) {
		Util::log(imp.GetErrorString());
	}
	
	Util::log("Loading " + Util::toString((size_t)mdlScene->mNumMeshes) + " meshes from " + mFilename);
	for( size_t i = 0; i < mdlScene->mNumMeshes; i++ ) {
		aiMesh* m = mdlScene->mMeshes[i];
		if(m->HasBones()) {
			Util::log("Skeletons not supported.");
		}
		
		mGeometry = new MeshGeometry();
		
		mGeometry->vertexData = new GeometryVertex[m->mNumVertices];
		mGeometry->edgeData = new GLedge[m->mNumFaces*3];
		mGeometry->vertexCount = m->mNumVertices;
		mGeometry->edgeCount = m->mNumFaces*3;
		
		for( size_t v = 0; v < m->mNumVertices; v++ )
		{
			aiVector3D vec = m->mVertices[v];
			mGeometry->vertexData[v].x = vec.x;
			mGeometry->vertexData[v].y = vec.y;
			mGeometry->vertexData[v].z = vec.z;
		}
		
		for(size_t f = 0; f < m->mNumFaces; f++) {
			aiFace face = m->mFaces[f];
			size_t v0 = face.mIndices[0];
			size_t v1 = face.mIndices[1];
			size_t v2 = face.mIndices[2];
			mGeometry->edgeData[f*3+0] = v0;
			mGeometry->edgeData[f*3+1] = v1;
			mGeometry->edgeData[f*3+2] = v2;
		}
		
		mGeometry->bindToBuffer();
	}
}

void ModelResource::unload()
{
	
}

bool ModelResource::isLoaded() const
{
	return mGeometry != nullptr;
}

MeshGeometry* ModelResource::getGeometry() const
{
	return mGeometry;
}