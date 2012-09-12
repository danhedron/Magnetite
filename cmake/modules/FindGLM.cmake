# - Find GLM
# Finds the GLM headers.
#
#  GLM_INCLUDE_DIR -  Where GLM is
#  GLM_FOUND        - True if GLM was found.

# Look for the header file.
FIND_PATH( GLM_INCLUDE_DIR NAMES glm/
	PATHS /usr/local/include
	/usr/include
	DOC "Path in which the file glm/glm.h is located." )
MARK_AS_ADVANCED(GLM_INCLUDE_DIR)

# Copy the results to the output variables.
IF(GLM_INCLUDE_DIR)
	SET(GLM_FOUND 1)
	SET(GLM_INCLUDE_DIR ${GLM_INCLUDE_DIR})
ELSE(GLM_INCLUDE_DIR)
	SET(GLM_FOUND 0)
	SET(GLM_INCLUDE_DIR)
ENDIF(GLM_INCLUDE_DIR)

# Report the results.
IF(NOT GLM_FOUND)
	SET(GLM_DIR_MESSAGE
		"GLM was not found. Make sure GLM_INCLUDE_DIR is set.")
	IF(GLM_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "${GLM_DIR_MESSAGE}")
	ELSEIF(NOT GLM_FIND_QUIETLY)
		MESSAGE(STATUS "${GLM_DIR_MESSAGE}")
	ENDIF(GLM_FIND_REQUIRED)
ENDIF(NOT GLM_FOUND)
