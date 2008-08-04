#####################################################
#
#	Main Cmake file 
#
# 	Project		ARTools
#	author		Henry Portilla
#	place		Barcelona, Spain
#	date		june/22/2007
#	Special 
#	Grant
#
####################################################
#	Project information
####################################################

PROJECT(ARTools)

####################################################
#	load  libraries
#	1. QT3
#	2. Coin
#	3. SoQt
#	4. avcodec/avformat
####################################################

FIND_PACKAGE(Qt3 REQUIRED)
INCLUDE(${CMAKE_SOURCE_DIR}/Modules/FindCoin.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/Modules/FindSoQt.cmake)

FIND_LIBRARY(AV_CODEC_LIBRARY avcodec
		usr/lib
		usr/local/lib
		usr/share)
#	Show a message to the user
IF(NOT AV_CODEC_LIBRARY)
	MESSAGE("Error: avcodec library was not found ") 
ENDIF(NOT AV_CODEC_LIBRARY)

FIND_LIBRARY(AV_FORMAT_LIBRARY avformat
		usr/lib
		usr/local/lib
		usr/share)
#	Show a message to the user
IF(NOT AV_FORMAT_LIBRARY)
	MESSAGE("Error: avformat library was not found ") 
ENDIF(NOT AV_FORMAT_LIBRARY)


####################################################
#      Add subdirectories within the build directory
#	1. src
#	2. lib555  library
####################################################
ADD_SUBDIRECTORY(lib555/liveMedia)
ADD_SUBDIRECTORY(lib555/groupsock)
ADD_SUBDIRECTORY(lib555/UsageEnvironment)
ADD_SUBDIRECTORY(lib555/BasicUsageEnvironment)
#ADD_SUBDIRECTORY(openvidia)
ADD_SUBDIRECTORY(src)

MESSAGE(STATUS "SRC_dir: " ${CMAKE_SOURCE_DIR})
MESSAGE(STATUS "BIN_dir: " ${CMAKE_BINARY_DIR})