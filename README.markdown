Opencraft is an Open Source Cube based adventure, have fun removing and placing blocks in your own 3D cube world.

Licence
-------

All content unless otherwise mentioned is licensed under the ZLib Licence.

Copyright (c) 2011 - Daniel Lloyd Evans

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.

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
