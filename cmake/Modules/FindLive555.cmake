# - Find live555 (live555 streaming libraries)
#   the live555 libraries are a set of libraries used to connect
#   to a remote server using flow trasmission protocols as RTSP, RTP, SDP
#
# This module defines the following variables
#  LIVE555_FOUND         - system has live555
#  LIVE555_INCLUDE_DIRS  - where the live555 includes directories can be found
#  LIVE555_LIBRARIES     - Link to this to use live555 libraries

#  Based on cmake files from Kitware Inc

  #Include the headers of the libraries  
  
  find_path(LIVE_MEDIA_INC liveMedia.hh
	  ${CMAKE_SOURCE_DIR}/live/liveMedia/include)
  message("dir: "${CMAKE_SOURCE_DIR})
  find_path(LIVE_GROUP_INC Groupsock.hh
	  ${CMAKE_SOURCE_DIR}/live/groupsock/include)
  find_path(LIVE_BASIC_INC BasicUsageEnvironment.hh
	    ${CMAKE_SOURCE_DIR}/live/BasicUsageEnvironment/include)
  find_path(LIVE_USAGE_INC UsageEnvironment.hh
	  ${CMAKE_SOURCE_DIR}/live/UsageEnvironment/include)
  set(LIVE555_INCLUDE_DIRS ${LIVE_MEDIA_INC} ${LIVE_GROUP_INC} ${LIVE_BASIC_INC} ${LIVE_USAGE_INC})
  
 IF (UNIX) 
 
  #       Search for the live555 libraries directories

  FIND_PATH(LIVE_MEDIA libliveMedia.a ${CMAKE_SOURCE_DIR}/live/liveMedia)
  FIND_PATH(LIVE_GROUP libgroupsock.a ${CMAKE_SOURCE_DIR}/live/groupsock)
  FIND_PATH(LIVE_BASIC libBasicUsageEnvironment.a ${CMAKE_SOURCE_DIR}/live/BasicUsageEnvironment)
  FIND_PATH(LIVE_USAGE libUsageEnvironment.a ${CMAKE_SOURCE_DIR}/live/UsageEnvironment)
  
  set(LIVE_MEDIA_LIB ${LIVE_MEDIA}/libliveMedia.a)
  set(LIVE_GROUP_LIB ${LIVE_GROUP}/libgroupsock.a)
  set(LIVE_BASIC_LIB ${LIVE_BASIC}/libBasicUsageEnvironment.a)
  set(LIVE_USAGE_LIB ${LIVE_USAGE}/libUsageEnvironment.a)
  
  set(LIVE555_LIBRARIES ${LIVE_MEDIA_LIB} ${LIVE_GROUP_LIB} ${LIVE_BASIC_LIB} ${LIVE_USAGE_LIB})

 
 ELSE (UNIX)  
  # Search for the live555 libraries directories

  find_library(LIVE_MEDIA_LIB liveMedia 
    PATH ${LIBRARY_OUTPUT_PATH}/Debug)
  find_library(LIVE_GROUP_LIB groupsock 
    PATH ${LIBRARY_OUTPUT_PATH}/Debug)
  find_library(LIVE_BASIC_LIB BasicUsageEnvironment 
    PATH ${LIBRARY_OUTPUT_PATH}/Debug)
  find_library(LIVE_USAGE_LIB UsageEnvironment 
    PATH ${LIBRARY_OUTPUT_PATH}/Debug)
  
  message ("build dir:" ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
  
  
  set(LIVE555_LIBRARIES ${LIVE_MEDIA_LIB} ${LIVE_GROUP_LIB} ${LIVE_BASIC_LIB} ${LIVE_USAGE_LIB})

  
ENDIF (UNIX)

# handle the QUIETLY and REQUIRED arguments and set LIVE555_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Live555 DEFAULT_MSG LIVE555_LIBRARIES LIVE555_INCLUDE_DIRS)

MARK_AS_ADVANCED(LIVE555_LIBRARIES LIVE555_INCLUDE_DIRS )