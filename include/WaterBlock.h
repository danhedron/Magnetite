#ifndef _WATERBLOCK_H_
#define _WATERBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class WaterBlock : public BaseBlock
{
protected:
	float mFluidLevel;
	bool mIsNew;
public:
	WaterBlock(long x = 0, long y = 0, long z = 0);
	~WaterBlock(void);

	bool isFluid();
	bool isOpaque();

	std::string getType();

	float getFluidLevel();
	void changeFluidLevel( float delta );
	void setFluidLevel( float delta );

	void getTextureCoords( short face, short &x, short &y );

	void connectedChange( short face );

	void flow( float dt );

	void balanceFluid( BaseBlock* block, float dt);
	void flowToBlock(short x, short y, short z, float dt);

	/**
	 * Returns the Verex Data for this block:- includes fluid height.
	 */
	virtual void buildCubeData(size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges);

	void created();
	void hit();
};

#endif