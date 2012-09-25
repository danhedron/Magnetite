#include "Tests.h"
#include "World.h"
#include "BaseBlock.h"
#include "Renderer.h"

int tests = 0, failed = 0;

void _fireAssert( bool condition, std::string name, std::string loc )
{
	tests++;
	if(!condition) 
	{
		failed++;
		Util::log(name + " - " + loc);
	}
}
#define _str(x) #x
#define _ass( cond, error ) \
	_fireAssert( cond, error, "("__FILE__" at line " + Util::toString(__LINE__) + ")");

void runTests()
{

}
