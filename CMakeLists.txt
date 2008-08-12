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
#	1. QT4
#	2. Coin
#	3. SoQt
#	4. avcodec/avformat
####################################################

FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_OPENGL_TRUE)
INCLUDE(${QT_USE_FILE})
INCLUDE(${CMAKE_SOURCE_DIR}/Modules/FindCoin.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/Modules/FindSoQt4.cmake)
# qooliv library
SET(qooliv_DIR ${CMAKE_INSTALL_PREFIX}/lib/qooliv)
FIND_PACKAGE(qooliv REQUIRED)
INCLUDE(${qooliv_USE_FILE})

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