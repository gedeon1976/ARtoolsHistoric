# - Find Quarter (Open Inventor)
#   Quarter is a lightweigth glue library that provides integration
#   between Coin3D and Nokia Qt 
#
# This module defines the following variables
#  QUARTER_FOUND         - system has Quarter 
#  QUARTER_INCLUDE_DIRS  - where the Quarter include directory can be found
#  QUARTER_LIBRARIES     - Link to this to use Quarter libraries
#
#  Based on cmake files from Kitware Inc

IF (WIN32)

ELSE (WIN32)
  
  find_path(QUARTER_INCLUDE_DIRS Quarter/Quarter.h
	  usr/local)
  find_library(QUARTER_LIBRARIES Quarter)

ENDIF (WIN32)

# handle the QUIETLY and REQUIRED arguments and set QUARTER_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Quarter DEFAULT_MSG QUARTER_LIBRARIES QUARTER_INCLUDE_DIRS)

MARK_AS_ADVANCED(QUARTER_LIBRARIES QUARTER_INCLUDE_DIRS )


