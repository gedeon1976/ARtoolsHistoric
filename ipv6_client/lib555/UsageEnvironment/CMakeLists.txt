#####################################################
#
#	Main Cmake file 
#	UsageEnvironment live555 library
#	 
#
# 	Project		ARTools
#	author		Henry Portilla
#	place		Barcelona, Spain
#	date		june/25/2007
#	Special 
#	Grant
#
#####################################################
#	Include header and source directories
#####################################################
INCLUDE_DIRECTORIES(./include ../groupsock/include)
FILE(GLOB INCLUDE_SRC *.cpp *.c)
#####################################################
#	Add flags to compiler
#####################################################
ADD_DEFINITIONS(-DSOCKLEN_T=socklen_t
		-D_LARGEFILE_SOURCE=1
		-D_FILE_OFFSET_BITS=64
		-Wall -DBSD=1)
#####################################################
#	build the library
#####################################################
ADD_LIBRARY(UsageEnvironment STATIC ${INCLUDE_SRC})