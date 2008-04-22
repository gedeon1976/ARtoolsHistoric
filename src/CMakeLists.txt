###################################################
#
#	src Cmake file 
#
# 	Project		ARTools
#	author		Henry Portilla
#	place		Barcelona, Spain
#	date		june/22/2007
#	Special 
#	Grant		GOD
#
###################################################
#	Set the paths for the libraries and 
#	the executable file
###################################################

###################################################
#	Include the headers of the libraries
###################################################
SET(LIVE555_DIR ${CMAKE_SOURCE_DIR}/lib555/liveMedia/include
${CMAKE_SOURCE_DIR}/lib555/groupsock/include
${CMAKE_SOURCE_DIR}/lib555/UsageEnvironment/include
${CMAKE_SOURCE_DIR}/lib555/BasicUsageEnvironment/include)

#SET(OPENVIDIA_DIR ${CMAKE_SOURCE_DIR}/openvidia/include)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}
		    ${COIN_INCLUDE_DIR}
		    ${SOQT_INCLUDE_DIR}
		    ${LIVE555_DIR})
		    #${OPENVIDIA_DIR})		
		
###################################################
#	Include the source files
###################################################
SET(ARTOOLS_h client.h SoStereoTexture.h)
SET(ARTOOLS_src client.cpp SoStereoTexture.cpp)
###################################################
#	Search for the live555 libraries directories
###################################################
FIND_PATH(LIVE_MEDIA libliveMedia.a ${CMAKE_BIN_DIR}/lib555/liveMedia)
FIND_PATH(LIVE_GROUP libgroupsock.a ${CMAKE_BIN_DIR}/lib555/groupsock)
FIND_PATH(LIVE_USAGE libUsageEnvironment.a ${CMAKE_BIN_DIR}/lib555/UsageEnvironment)
FIND_PATH(LIVE_BASIC libBasicUsageEnvironment.a ${CMAKE_BIN_DIR}/lib555/BasicUsageEnvironment)

#FIND_PATH(OPENVIDIA libopenvidia.a ${CMAKE_BIN_DIR}/lib555)
#MESSAGE(STATUS "dir: " ${LIVE_MEDIA})
#MESSAGE(STATUS "dir: " ${LIVE_GROUP})
#MESSAGE(STATUS "dir: " ${LIVE_USAGE})
#MESSAGE(STATUS "dir: " ${LIVE_BASIC})
#MESSAGE(STATUS "dir: " ${OPENVIDIA})
###################################################
#	Directories of libraries to link
###################################################
#${OPENVIDIA}
LINK_DIRECTORIES(${LIVE_MEDIA} ${LIVE_GROUP} ${LIVE_USAGE} ${LIVE_BASIC} )
LINK_DIRECTORIES(${AV_FORMAT_LIBRARY} ${AV_CODEC_LIBRARY})

###################################################
#	It generates the executable file
###################################################

ADD_EXECUTABLE(ARtools ${ARTOOLS_h} ${ARTOOLS_src})

###################################################
#	Link the code to the libraries
###################################################
#openvidia
TARGET_LINK_LIBRARIES(ARtools ${QT_LIBRARY} 
			      ${COIN_LIBRARY}
			      ${SOQT_LIBRARY}
			      	liveMedia 
				groupsock
				BasicUsageEnvironment
				UsageEnvironment
				GLU
				CgGL
				m
				pthread
				avformat
				avcodec)

###################################################
