Opencraft is an Open Source Cube based adventure, have fun removing and placing blocks in your own 3D cube world.

Licence
-------

Opencraft is licensed under the GPLv3 license. Unless otherwise indicated.
 
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

Building
--------

Requires sfml (Simple Fast Media Layer): [sfml-dev.org](http://www.sfml-dev.org/)

### On Windows

+ Add an Environment Variable named SFMLDIR, set it to the SFML folder.

+ Run CMake

### On Ubuntu

#### Installing prerequisites

+ SFML
  - [Debian and Ubuntu repositories for smfl](http://www.sfml-dev.org/wiki/en/tutorials/getdebpackage)
  - Add these to /etc/apt/sources.list
    * `deb http://ppa.launchpad.net/christoph-egger/ubuntu hardy main`
    * `deb-src http://ppa.launchpad.net/christoph-egger/ubuntu hardy main`
  - Run `aptitude update && apt-get libsfml-dev libcsfml-dev`
+ OpenGL
  - Run `aptitude update && apt-get mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev`
+ Freeglut
  - Download, make and install [freeglut](http://freeglut.sourceforge.net/index.php#download).

#### Building

+ Run cmake
+ Run make in the build folder
