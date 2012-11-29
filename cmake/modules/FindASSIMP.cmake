# - Try to find Assimp
# Once done this will define
#
#  ASSIMP_FOUND - system has Assimp
#  ASSIMP_INCLUDE_DIR - the Assimp include directory
#  ASSIMP_LIBRARIES - Link these to use Assimp
#

SET(ASSIMP "assimp")

FIND_PATH(ASSIMP_INCLUDE_DIR NAMES assimp/ai_assert.h
  PATHS
  /usr/include
  /usr/local/include
  /opt/local/include
  NO_DEFAULT_PATH
)
 

FIND_LIBRARY(LIBASSIMP
  NAMES 
  ${ASSIMP}
  PATHS
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  NO_DEFAULT_PATH
)

SET (ASSIMP_LIBRARIES
  ${LIBASSIMP} 
)

IF(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)
   SET(ASSIMP_FOUND TRUE)
ENDIF(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)

IF(ASSIMP_FOUND)
  MARK_AS_ADVANCED(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARIES )
ENDIF(ASSIMP_FOUND)

