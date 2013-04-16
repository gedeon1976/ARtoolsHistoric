# - Find Pthreads for windows 
#   Pthreads is a library implementation pf POSIX Unix
#   threads for win32 
#
# This module defines the following variables
#  PTHREADS_FOUND         - system has Pthreads 
#  PTHREADS_INCLUDE_DIRS  - where the Pthreads include directory can be found
#  PTHREADS_LIBRARIES     - Link to this to use Pthreads libraries
#
#  Based on cmake files from Kitware Inc

IF (WIN32)
  # the path to the libraries was predefined in the below directory
  # you can download this library on http://sourceware.org/pthreads-win32/
  
  find_path(PTHREADS_INCLUDE_DIRS pthread.h
      "/Pthreads/include")
  find_library(PTHREADS_LIBRARIES pthreadVCE2
      "/Pthreads/lib/${CMAKE_HOST_SYSTEM_PROCESSOR}")


ENDIF (WIN32)

# handle the QUIETLY and REQUIRED arguments and set PTHREADS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Pthreads DEFAULT_MSG PTHREADS_LIBRARIES PTHREADS_INCLUDE_DIRS)

MARK_AS_ADVANCED(PTHREADS_LIBRARIES PTHREADS_INCLUDE_DIRS )


