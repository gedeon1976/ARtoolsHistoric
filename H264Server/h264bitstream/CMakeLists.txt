 
#####################################################
#
#       Main Cmake file 
#       h264bitstream library
#        
#
#       Project         H264Server
#       author          Henry Portilla
#       place           Barcelona, Spain
#       date            dec/2012
#       Special 
#       Grant
#
#####################################################
#       Include header and source directories
#####################################################
INCLUDE_DIRECTORIES(./include ../h264bitstream/include)
FILE(GLOB INCLUDE_SRC *.cpp *.c)
#####################################################
#       Add flags to compiler
#####################################################
if(UNIX)
   ADD_DEFINITIONS(-std=c99)
elseif(UNIX)
   if(WIN32)
       ADD_DEFINITIONS(-std=c99)
   endif(WIN32)
endif(UNIX)

#####################################################
#       build the library
#####################################################
ADD_LIBRARY(h264bitstream STATIC ${INCLUDE_SRC})