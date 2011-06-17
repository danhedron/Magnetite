#ifndef _BASEBLOCK_H_
#define _BASEBLOCK_H_
#include "prerequisites.h"

struct GLvertex;
typedef unsigned short GLedge;

enum {
	FACE_TOP		= (1<<0),
	FACE_BOTTOM	= (1<<1),
	FACE_LEFT	= (1<<2),
	FACE_RIGHT	= (1<<3),
	FACE_FORWARD	= (1<<4),
	FACE_BACK	= (1<<5),
	FACE_ALL	= FACE_TOP | FACE_BOTTOM | FACE_LEFT | FACE_RIGHT | FACE_FORWARD | FACE_BACK
};

enum {
	BMASK_XPOS = ( 1 | (1<<1) | (1<<2) | (1<<3) ),
	BMASK_ZPOS = ( (1<<4) | (1<<5) | (1<<6) | (1<<7) ),
	BMASK_YPOS = ( (1<<8) | (1<<9) | (1<<10) | (1<<11) | (1<<12) | (1<<13)  | (1<<14) ),
	BMASK_VISFLAGS = ( (1<<15) | (1<<16) | (1<<17) | (1<<18) | (1<<19) | (1<<20) )
};

class WorldChunk;

class BaseBlock
{
protected:
	unsigned int mDataFlags;

	WorldChunk* mChunk;

	void _setPosition( unsigned short x, unsigned  short y, unsigned short z );
public:
	BaseBlock();
	~BaseBlock(void);

	void _setChunk( WorldChunk* chnk );

	virtual void created() = 0;
	virtual void hit() = 0;

	/**
	 * Returns true if this object inherits FluidBlock
	 */
	virtual bool isFluid() { return false; }
	
	/**
	 * Returns true if block should stop occlusion
	 */
	virtual bool isOpaque() { return true; }

	/**
	 * Returns true if this block is solid.
	 */
	virtual bool isSolid() { return true; }

	/**
	 * Called when a nearby block changes.
	 * @param face The direction of the changed block.
	 */
	virtual void connectedChange( short face );
	virtual void getTextureCoords( short face, short &x, short &y );

	/**
	 * Updates the viewflags for this block
	 * @param flags the new bitfield
	 */
	void updateVisFlags(unsigned int flags);

	/**
	 * Returns the viewflags for this block
	 */
	unsigned short getVisFlags();

	/**
	 * Returns the Verex Data for this block
	 */
	virtual void buildCubeData(size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges);

	virtual std::string getType() = 0;

	short getX();
	short getY();
	short getZ();

	void setPosition( long x, long y, long z );
	void setPosition( const Vector3& vec );
};

#endif