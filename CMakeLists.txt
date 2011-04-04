project(ARtools)
cmake_minimum_required(VERSION 2.6)

#define the path to the cmake modules
add_subdirectory(cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
# set output directories

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# find library packages
find_package(Boost REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Coin3D REQUIRED)
find_package(Qt4 REQUIRED)
find_package(SoQt4 REQUIRED)
#find_package(Quarter REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(Live555 REQUIRED)
if (WIN32)
  find_package(Pthreads REQUIRED)
endif (WIN32)


# add subdirectories

add_subdirectory(live/liveMedia)
add_subdirectory(live/groupsock)
add_subdirectory(live/UsageEnvironment)
add_subdirectory(live/BasicUsageEnvironment)
add_subdirectory(libcomm)
add_subdirectory(libmt)
add_subdirectory(src)

