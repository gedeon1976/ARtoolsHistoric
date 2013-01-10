##########################################
#		H264 video streaming server
#		
#		copyright(2012) Henry Portilla
#				IOC-UPC		
#
###########################################

project(H264Server)
cmake_minimum_required(VERSION 2.8)


# define the path to make the cmake modules available
add_subdirectory(cmake)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
message("CMake Modules path=  " ${CMAKE_MODULE_PATH})

# set output directories
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# find library packages
find_package(Qt4 REQUIRED)
find_package(FFMPEG REQUIRED)
if (WIN32)
	find_package(Pthreads REQUIRED)
endif(WIN32)

# add live555 libraries
add_subdirectory(live/liveMedia)
add_subdirectory(live/groupsock)
add_subdirectory(live/UsageEnvironment)
add_subdirectory(live/BasicUsageEnvironment)
find_package(Live555 REQUIRED)

#add h264bitstream library
add_subdirectory(h264bitstream)
find_package(H264BITSTREAM REQUIRED)

# add source code
add_subdirectory(src)