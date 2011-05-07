#include "prerequisites.h"
#include "OpencraftCore.h"

int main( int argc, char** argv ) {
	OpencraftCore core;

	core.go(&argc, argv);

	return EXIT_SUCCESS;
}