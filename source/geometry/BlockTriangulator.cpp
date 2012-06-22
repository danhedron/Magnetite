#include "BlockTriangulator.h"
#include <Chunk.h>
#include <BaseBlock.h>
#include <Renderer.h>
#include <MagnetiteCore.h>
#include <World.h>
#include <TextureManager.h>
#include "glgeometry.h"

// These are here for reference & eveuntually some cleaner code.
/*static float face_vertices[] = {
	0.f, 0.f, 0.f, // 0
	0.f, 0.f, 1.f, 
	0.f, 1.f, 0.f, // 2
	0.f, 1.f, 1.f,
	1.f, 0.f, 0.f, // 4
	1.f, 0.f, 1.f,
	1.f, 1.f, 0.f, // 6
	1.f, 1.f, 1.f
};

static size_t face_indicies[] = {
	0, 1, 2,	2, 1, 3, 
	4, 0, 6,	6, 0, 2,
	5, 4, 7,	7, 4, 6,
	1, 5, 3,	3, 5, 7,
	6, 2, 7,	7, 2, 3, // Top
	0, 1, 4,	4, 1, 5  // Bottom
};*/

void BlockTriangulator::triangulateChunk( GLgeometry* geom, Chunk* chunk )
{
	// Prepare the geometry
	GLuint numVerts = chunk->getVisibleFaceCount() * 4;
	GLuint numEdges = chunk->getVisibleFaceCount() * 6;
	
	geom->vertexData = new GLvertex[numVerts];
	geom->edgeData = new GLedge[numEdges];
	geom->vertexCount = numVerts;
	geom->edgeCount = numEdges;

	//BlockContext context;
	
	size_t eInd = 0, ind = 0;
	
	auto data = geom->vertexData;
	auto edges = geom->edgeData;
	long cx = (chunk->getX() * CHUNK_WIDTH), cy = (chunk->getY() * CHUNK_WIDTH), cz = (chunk->getZ() * CHUNK_WIDTH);
	
	for( BlockList::iterator it = chunk->getVisibleBlocks().begin(); it != chunk->getVisibleBlocks().end(); ++it )
	{
		Vector3 pos = Util::indexToPosition( it->first );
		/*context.worldX = pos.x;
		context.worldY = pos.y;
		context.worldZ = pos.z;
		context.chunk = chunk;
		context.world = chunk->getWorld();*/
		//it->second->buildCubeData( context, vind, eind, verts, edges );
		
		long wx = cx + pos.x, wy = cy + pos.y, wz = cz + pos.z;
		short texX = 0, texY = 0;
		short visFlags = it->second->getVisFlags();
			
		/* Face -Z */
		if((visFlags & FACE_BACK) == FACE_BACK ) {
			it->second->getTextureCoords( FACE_BACK, texX, texY );
			GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( texX, texY );
			
			float color = World::getLightColor( chunk->getWorld()->getLightLevel( wx, wy, wz+1 ) );
			data[ind + 0] = GLgeometry::vertex( pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, // Coordinates
									rect.x, rect.y,
									color );
			data[ind + 1] = GLgeometry::vertex( pos.x - 0.0f, pos.y + 1.0f, pos.z + 1.0f, // Coordinates
									rect.x + rect.w, rect.y,
									color );
			data[ind + 2] = GLgeometry::vertex( pos.x - 0.0f, pos.y - 0.0f, pos.z + 1.0f, // Coordinates
									rect.x + rect.w, rect.y + rect.h,
									color );
			data[ind + 3] = GLgeometry::vertex( pos.x + 1.0f, pos.y - 0.0f, pos.z + 1.0f, // Coordinates
									rect.x, rect.y + rect.h,
									color );
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face +Z */
		if((visFlags & FACE_FORWARD) == FACE_FORWARD ) {
			it->second->getTextureCoords( FACE_FORWARD, texX, texY );
			GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( texX, texY );

			float color = World::getLightColor( chunk->getWorld()->getLightLevel( wx, wy, wz-1 ) );
			data[ind + 0] = GLgeometry::vertex( pos.x + 1.0f, pos.y + 1.0f, pos.z - 0.0f, // Coordinates
									rect.x, rect.y,
									color );
			data[ind + 1] = GLgeometry::vertex( pos.x - 0.0f, pos.y + 1.0f, pos.z - 0.0f, // Coordinates
									rect.x + rect.w, rect.y,
									color );
			data[ind + 2] = GLgeometry::vertex( pos.x - 0.0f, pos.y - 0.0f, pos.z - 0.0f, // Coordinates
									rect.x + rect.w, rect.y + rect.h,
									color );
			data[ind + 3] = GLgeometry::vertex( pos.x + 1.0f, pos.y - 0.0f, pos.z - 0.0f, // Coordinates
									rect.x, rect.y + rect.h,
									color );
			edges[eInd + 5] = ind + 2; edges[eInd + 4] = ind + 1; edges[eInd + 3] = ind + 0;
			edges[eInd + 2] = ind + 2; edges[eInd + 1] = ind + 0; edges[eInd + 0] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face +X */
		if((visFlags & FACE_RIGHT) == FACE_RIGHT) {
			it->second->getTextureCoords( FACE_RIGHT, texX, texY );
			GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( texX, texY );

			float color = World::getLightColor( chunk->getWorld()->getLightLevel( wx+1, wy, wz ) );
			data[ind + 0] = GLgeometry::vertex( pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, // Coordinates
									rect.x + rect.w, rect.y,
									color );
			data[ind + 1] = GLgeometry::vertex( pos.x + 1.0f, pos.y - 0.0f, pos.z + 1.0f, // Coordinates
									rect.x + rect.w, rect.y + rect.h,
									color );
			data[ind + 2] = GLgeometry::vertex( pos.x + 1.0f, pos.y - 0.0f, pos.z - 0.0f, // Coordinates
									rect.x, rect.y + rect.h,
									color );
			data[ind + 3] = GLgeometry::vertex( pos.x + 1.0f, pos.y + 1.0f, pos.z - 0.0f, // Coordinates
									rect.x, rect.y,
									color );
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face -Y */
		if((visFlags & FACE_BOTTOM) == FACE_BOTTOM) {
			it->second->getTextureCoords( FACE_BOTTOM, texX, texY );
			GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( texX, texY );

			float color = World::getLightColor( chunk->getWorld()->getLightLevel( wx, wy-1, wz ) );
			data[ind + 0] = GLgeometry::vertex( pos.x - 0.0f, pos.y - 0.0f, pos.z + 1.0f, // Coordinates
									rect.x, rect.y,
									color );
			data[ind + 1] = GLgeometry::vertex( pos.x - 0.0f, pos.y - 0.0f, pos.z - 0.0f, // Coordinates
									rect.x + rect.w, rect.y,
									color );
			data[ind + 2] = GLgeometry::vertex( pos.x + 1.0f, pos.y - 0.0f, pos.z - 0.0f, // Coordinates
									rect.x + rect.w, rect.y + rect.h,
									color );
			data[ind + 3] = GLgeometry::vertex( pos.x + 1.0f, pos.y - 0.0f, pos.z + 1.0f, // Coordinates
									rect.x, rect.y + rect.h,
									color );
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face +Y */
		if((visFlags & FACE_TOP) == FACE_TOP) {
			it->second->getTextureCoords( FACE_TOP, texX, texY );
			GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( texX, texY );

			float color = World::getLightColor( chunk->getWorld()->getLightLevel( wx, wy+1, wz ) );
			data[ind + 0] = GLgeometry::vertex( pos.x - 0.0f, pos.y + 1.0f, pos.z + 1.0f, // Coordinates
									rect.x, rect.y,
									color );
			data[ind + 1] = GLgeometry::vertex( pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, // Coordinates
									rect.x + rect.w, rect.y,
									color );
			data[ind + 2] = GLgeometry::vertex( pos.x + 1.0f, pos.y + 1.0f, pos.z - 0.0f, // Coordinates
									rect.x + rect.w, rect.y + rect.h,
									color );
			data[ind + 3] = GLgeometry::vertex( pos.x - 0.0f, pos.y + 1.0f, pos.z - 0.0f, // Coordinates
									rect.x, rect.y + rect.h,
									color );
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face -X */
		if((visFlags & FACE_LEFT) == FACE_LEFT) {
			it->second->getTextureCoords( FACE_LEFT, texX, texY );
			GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( texX, texY );

			float color = World::getLightColor( chunk->getWorld()->getLightLevel( wx-1, wy, wz ) );
			data[ind + 0] = GLgeometry::vertex( pos.x - 0.0f, pos.y + 1.0f, pos.z - 0.0f, // Coordinates
									rect.x + rect.w, rect.y,
									color );
			data[ind + 1] = GLgeometry::vertex( pos.x - 0.0f, pos.y - 0.0f, pos.z - 0.0f, // Coordinates
									rect.x + rect.w, rect.y + rect.h,
									color );
			data[ind + 2] = GLgeometry::vertex( pos.x - 0.0f, pos.y - 0.0f, pos.z + 1.0f, // Coordinates
									rect.x, rect.y + rect.h,
									color );
			data[ind + 3] = GLgeometry::vertex( pos.x - 0.0f, pos.y + 1.0f, pos.z + 1.0f, // Coordinates
									rect.x, rect.y,
									color );
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
	}
	
}