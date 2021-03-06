#include "BlockTriangulator.h"
#include <Chunk.h>
#include <BaseBlock.h>
#include <Renderer.h>
#include <MagnetiteCore.h>
#include <World.h>
#include <TextureManager.h>
#include "Geometry.h"

/*
 * 
	vert.x = x;
	vert.y = y;
	vert.z = z;
	vert.u0 = (GLubyte)u;
	vert.v0 = (GLubyte)v;
	vert.l = (GLubyte)(l*255);
*/

#define VERTEX( fn, xp, yp, zp, up, vp, lp ) \
	data[ind + fn].x = pos.x + xp; \
	data[ind + fn].y = pos.y + yp; \
	data[ind + fn].z = pos.z + zp; \
	data[ind + fn].u0 = up; \
	data[ind + fn].v0 = vp; \
	data[ind + fn].l = lp*255.f;

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

void BlockTriangulator::triangulateChunk( TerrainGeometry* geom, Chunk* chunk )
{
	// Prepare the geometry
	GLuint numVerts = chunk->getVisibleFaceCount() * 4;
	GLuint numEdges = chunk->getVisibleFaceCount() * 6;
	
	if(geom->vertexData != nullptr)
	{
		delete[] geom->vertexData;
	}
	if(geom->edgeData != nullptr)
	{
		delete[] geom->edgeData;
	}
	
	geom->vertexData = new TerrainVertex[numVerts];
	geom->edgeData = new GLedge[numEdges];
	geom->vertexCount = numVerts;
	geom->edgeCount = numEdges;

	//BlockContext context;
	
	size_t eInd = 0, ind = 0;
	
	auto data = geom->vertexData;
	auto edges = geom->edgeData;
	ChunkScalar cx = (chunk->getX() * CHUNK_WIDTH), cy = (chunk->getY() * CHUNK_WIDTH), cz = (chunk->getZ() * CHUNK_WIDTH);
	auto tm = MagnetiteCore::Singleton->getTextureManager();
	auto w = chunk->getWorld();
	auto &vb = chunk->getVisibleBlocks();
	
	ChunkScalar wx, wy, wz;
	short texX, texY;
	short visFlags;
	Vector3 pos;
	GLuvrect rect;
	
	BaseBlock* b;
	
	for( BlockList::iterator it = vb.begin(); it != vb.end(); ++it )
	{
		pos = Util::indexToPosition( it->first );
		b = it->second;
		
		wx = cx + pos.x; wy = cy + pos.y; wz = cz + pos.z;
		texX = 0, texY = 0;
		visFlags = it->second->getVisFlags();
			
		/* Face -Z */
		if((visFlags & FACE_BACK) == FACE_BACK ) {
			b->getTextureCoords( FACE_BACK, texX, texY );
			rect = tm->getBlockUVs( texX, texY );
			
			float color = World::getLightColor( w->getLightLevel( wx, wy, wz+1 ) );
			
			VERTEX( 0, 1.0f, 1.0f, 1.0f, rect.x,          rect.y,          color )
			VERTEX( 1, 0.0f, 1.0f, 1.0f, rect.x + rect.w, rect.y,          color )
			VERTEX( 2, 0.0f, 0.0f, 1.0f, rect.x + rect.w, rect.y + rect.h, color )
			VERTEX( 3, 1.0f, 0.0f, 1.0f, rect.x,          rect.y + rect.h, color )
			
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face +Z */
		if((visFlags & FACE_FORWARD) == FACE_FORWARD ) {
			b->getTextureCoords( FACE_FORWARD, texX, texY );
			rect = tm->getBlockUVs( texX, texY );

			float color = World::getLightColor( w->getLightLevel( wx, wy, wz-1 ) );
			
			VERTEX( 0, 1.0f, 1.0f, 0.0f, rect.x,          rect.y,          color )
			VERTEX( 1, 0.0f, 1.0f, 0.0f, rect.x + rect.w, rect.y,          color )
			VERTEX( 2, 0.0f, 0.0f, 0.0f, rect.x + rect.w, rect.y + rect.h, color )
			VERTEX( 3, 1.0f, 0.0f, 0.0f, rect.x,          rect.y + rect.h, color )
			
			edges[eInd + 5] = ind + 2; edges[eInd + 4] = ind + 1; edges[eInd + 3] = ind + 0;
			edges[eInd + 2] = ind + 2; edges[eInd + 1] = ind + 0; edges[eInd + 0] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face +X */
		if((visFlags & FACE_RIGHT) == FACE_RIGHT) {
			b->getTextureCoords( FACE_RIGHT, texX, texY );
			rect = tm->getBlockUVs( texX, texY );
			
			float color = World::getLightColor( w->getLightLevel( wx+1, wy, wz ) );

			// + rect.w, rect.y,          color )
			// + rect.w, rect.y + rect.h, color )
			//          rect.y + rect.h, color )
			VERTEX( 0, 1.0f, 1.0f, 1.0f, rect.x,          rect.y,          color )
			VERTEX( 1, 1.0f, 0.0f, 1.0f, rect.x,          rect.y + rect.h, color )
			VERTEX( 2, 1.0f, 0.0f, 0.0f, rect.x + rect.w, rect.y + rect.h, color )
			VERTEX( 3, 1.0f, 1.0f, 0.0f, rect.x + rect.w, rect.y,          color )
			
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face -Y */
		if((visFlags & FACE_BOTTOM) == FACE_BOTTOM) {
			b->getTextureCoords( FACE_BOTTOM, texX, texY );
			rect = tm->getBlockUVs( texX, texY );

			float color = World::getLightColor( w->getLightLevel( wx, wy-1, wz ) );
			
			VERTEX( 0, 0.0f, 0.0f, 1.0f, rect.x,          rect.y,          color )
			VERTEX( 1, 0.0f, 0.0f, 0.0f, rect.x + rect.w, rect.y,          color )
			VERTEX( 2, 1.0f, 0.0f, 0.0f, rect.x + rect.w, rect.y + rect.h, color )
			VERTEX( 3, 1.0f, 0.0f, 1.0f, rect.x,          rect.y + rect.h, color )
			
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face +Y */
		if((visFlags & FACE_TOP) == FACE_TOP) {
			b->getTextureCoords( FACE_TOP, texX, texY );
			rect = tm->getBlockUVs( texX, texY );

			float color = World::getLightColor( w->getLightLevel( wx, wy+1, wz ) );
			
			VERTEX( 0, 0.0f, 1.0f, 1.0f, rect.x,          rect.y,          color )
			VERTEX( 1, 1.0f, 1.0f, 1.0f, rect.x + rect.w, rect.y,          color )
			VERTEX( 2, 1.0f, 1.0f, 0.0f, rect.x + rect.w, rect.y + rect.h, color )
			VERTEX( 3, 0.0f, 1.0f, 0.0f, rect.x,          rect.y + rect.h, color )
			
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
		/* Face -X */
		if((visFlags & FACE_LEFT) == FACE_LEFT) {
			b->getTextureCoords( FACE_LEFT, texX, texY );
			rect = tm->getBlockUVs( texX, texY );

			float color = World::getLightColor( w->getLightLevel( wx-1, wy, wz ) );
			
			VERTEX( 0, 0.0f, 1.0f, 0.0f, rect.x,          rect.y,          color )
			VERTEX( 1, 0.0f, 0.0f, 0.0f, rect.x,          rect.y + rect.h, color )
			VERTEX( 2, 0.0f, 0.0f, 1.0f, rect.x + rect.w, rect.y + rect.h, color )
			VERTEX( 3, 0.0f, 1.0f, 1.0f, rect.x + rect.w, rect.y,          color )
			
			edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
			edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
			eInd += 6;
			ind += 4;
		}
	}
	
}