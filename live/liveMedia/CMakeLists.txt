#####################################################
#
#       Main Cmake file 
#       Live media live555 library
#        
#
#       Project         ARTools
#       author          Henry Portilla
#       place           Barcelona, Spain
#       date            june/25/2007
#       Special 
#       Grant
#
#####################################################
#       Include header and source directories
#####################################################
INCLUDE_DIRECTORIES(./include 
                   ../UsageEnvironment/include
                   ../groupsock/include )

FILE(GLOB INCLUDE_SRC *.cpp *.c)
#####################################################
#       Add flags to compiler
#####################################################
if(UNIX)
   ADD_DEFINITIONS(-DSOCKLEN_T=socklen_t
      -D_LARGEFILE_SOURCE=1
      -D_FILE_OFFSET_BITS=64
      -DBSD=1)
elseif(UNIX)
   if(WIN32)
       ADD_DEFINITIONS(-DSOCKLEN_T=int 
                       -DUSE_OUR_BZERO=1 -D__MINGW32__)
   endif(WIN32)
endif(UNIX)
#####################################################
#       build the library
#####################################################
ADD_LIBRARY(liveMedia STATIC ${INCLUDE_SRC}) 
