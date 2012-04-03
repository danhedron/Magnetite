#include "LogBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<LogBlock> logFactory("log");

LogBlock::LogBlock(long x, long y, long z)
: BaseBlock()
{
}

LogBlock::~LogBlock(void)
{
}

void LogBlock::created()
{

}

void LogBlock::hit()
{

}

void LogBlock::getTextureCoords( short face, short &x, short &y )
{
	if( face == FACE_TOP || face == FACE_BOTTOM )
	{
		x = 1; y = 2;
	}
	else 
	{
		x = 0;
		y = 2;
	}
}

std::string LogBlock::getType()
{
	return "log";
}