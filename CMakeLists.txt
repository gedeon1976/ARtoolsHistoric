project(ARtools)
cmake_minimum_required(VERSION 2.6)

#define the path to the cmake modules
add_subdirectory(cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

# find library packages
find_package(Qt4 REQUIRED)
find_package(Coin3D REQUIRED)
find_package(Quarter REQUIRED)
find_package(OpenGL REQUIRED)
find_package(FFMPEG REQUIRED) 
find_package(Live555 REQUIRED)

# add subdirectories
add_subdirectory(live/liveMedia)
add_subdirectory(live/groupsock)
add_subdirectory(live/UsageEnvironment)
add_subdirectory(live/BasicUsageEnvironment)
add_subdirectory(src)

