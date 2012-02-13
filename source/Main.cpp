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
#include "Tests.h"

// Don't mind this.
Util::LogLevel Util::_loglevel = Util::Info;


int main( int argc, char** argv ) {
	
	std::string game = "default";
	
	if( argc > 1 ) {
		char* str = argv[1];
		if( strstr( str, "--test" ) != NULL ) {

			runTests();

			return 0;
		}
		if( strstr( str, "--game" ) != NULL && argc > 2) {
			game = std::string(argv[2]);
		}
	}
	MagnetiteCore core;

	core.init(&argc, argv);

	core.startGame(game);

	core.go();

	return EXIT_SUCCESS;
}
