/*
	Copyright (C) 2011  Daniel Lloyd Evans

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "prerequisites.h"
#include "MagnetiteCore.h"
#include "World.h"
#include "BaseBlock.h"
#include "WorldChunk.h"

// Don't mind this.
Util::LogLevel Util::_loglevel = Util::Info;

int main( int argc, char** argv ) {
		
	if( false ) {
		Util::log("Magnetite - Not Quite Unit Testing");

		Util::log("== Chunk Tests");

		MagnetiteCore core;

		core.newWorld("test-world");

		/*WorldChunk* origin = core.getWorld()->getChunk( 0, 0, 0 );
		WorldChunk* left = core.getWorld()->getChunk( 0, 0, -1 );
		WorldChunk* back = core.getWorld()->getChunk( -1, 0, 0 );

		Util::setLogLevel( Util::Verbose );
		BaseBlock* b = left->getBlockAt(0,0,16);
		if( b != NULL && b->getZ() == 0 )
			Util::log("\t+Z Jumps Ok");
		else
			Util::log("\t+Z Jumps ERROR");

		b = origin->getBlockAt(0,0,-1);
		if( b != NULL && b->getZ() == 15 )
			Util::log("\t-Z Jumps Ok");
		else
			Util::log("\t-Z Jumps ERROR");

		b = back->getBlockAt(16,0,0);
		if( b != NULL && b->getX() == 0 )
			Util::log("\t+X Jumps Ok");
		else
			Util::log("\t+X Jumps ERROR");

		b = origin->getBlockAt(-1,0,0);
		if( b != NULL && b->getX() == 15 )
			Util::log("\t-X Jumps Ok");
		else
			Util::log("\t-X Jumps ERROR");
		Util::setLogLevel( Util::Info );
		*/

		return 0;
	}
	MagnetiteCore core;

	core.init(&argc, argv);

	core.startGame("default");

	core.go();

	return EXIT_SUCCESS;
}