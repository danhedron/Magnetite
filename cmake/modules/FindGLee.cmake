# - Find GLee
# Finds the native GLee headers and libraries
#
#  GLEE_INCLUDE_DIR -  where to find V8.h, etc.
#  GLEE_LIBRARIES    - List of libraries when using V8.
#  GLEE_FOUND        - True if V8 found.

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header file.
FIND_PATH( GLEE_INCLUDE_DIR NAMES GLee.h
	PATHS /usr/local/include
	$ENV{H3D_EXTERNAL_ROOT}/include  
	DOC "Path in which the file GLee.h is located." )

MARK_AS_ADVANCED(GLEE_INCLUDE_DIR)

# Look for the library.
# Does this work on UNIX systems? (LINUX)
FIND_LIBRARY( GLEE_LIBRARY NAMES GLee
	/usr/lib/
	/usr
	DOC "Path to the GLee library." )
MARK_AS_ADVANCED(GLEE_LIBRARY)

# Copy the results to the output variables.
IF(GLEE_INCLUDE_DIR AND GLEE_LIBRARY)
	SET(GLEE_FOUND 1)
	SET(GLEE_LIBRARIES ${GLEE_LIBRARY})
	SET(GLEE_INCLUDE_DIR ${GLEE_INCLUDE_DIR})
ELSE(GLEE_INCLUDE_DIR AND GLEE_LIBRARY)
	SET(GLEE_FOUND 0)
	SET(GLEE_LIBRARIES)
	SET(GLEE_INCLUDE_DIR)
ENDIF(GLEE_INCLUDE_DIR AND GLEE_LIBRARY)

# Report the results.
IF(NOT GLEE_FOUND)
	SET(GLEE_DIR_MESSAGE
		"GLEE was not found. Make sure GLEE_LIBRARY and GLEE_INCLUDE_DIR are set.")
	IF(GLEE_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "${GLEE_DIR_MESSAGE}")
	ELSEIF(NOT GLEE_FIND_QUIETLY)
		MESSAGE(STATUS "${GLEE_DIR_MESSAGE}")
	ENDIF(GLEE_FIND_REQUIRED)
ENDIF(NOT GLEE_FOUND)
