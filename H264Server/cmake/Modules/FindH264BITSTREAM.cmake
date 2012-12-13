# - Find h264bitstream (based on h264bitstream analysis library)
#   the h264bitstream library is a tool used to read and write the
#	h264 headers and analyze the different NAL units types SPS,PPS,slices, slices types(P,B,I)
#	for the H.264/AVC standard
#
#  This module defines the following variables
#  H264BITSTREAM_FOUND         - system has H264bitstream
#  H264BITSTREAM_INCLUDE_DIRS  - where the h264bitstream includes directories can be found
#  H264BITSTREAM_LIBRARIES     - Link to this to use h264bitstream library

#  Based on cmake files from Kitware Inc

  #Include the headers of the libraries  
  
  find_path(H264_INC h264_stream.h
          ${CMAKE_SOURCE_DIR}/h264bitstream/include)
  message("dir: "${CMAKE_SOURCE_DIR})

  set(H264BITSTREAM_INCLUDE_DIRS ${H264_INC})
  
 IF (UNIX) 
 
  #Search for the h264bitstream library directory
  
  FIND_PATH(H264_MEDIA libh264bitstream.a ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
  
  set(H264_LIB ${H264_MEDIA}/libh264bitstream.a)
  
  message ("build dir:" ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
  
  set(H264BITSTREAM_LIBRARIES ${H264_LIB})
  
 
 ELSE (UNIX)  
  # Search for the live555 libraries directories

  find_library(H264_LIB h264bitstream 
    PATH ${LIBRARY_OUTPUT_PATH}/lib)
  message ("build dir:" ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
  
  set(H264BITSTREAM_LIBRARIES ${H264_LIB})
  
ENDIF (UNIX)

# handle the QUIETLY and REQUIRED arguments and set LIVE555_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(H264BITSTREAM DEFAULT_MSG H264BITSTREAM_LIBRARIES H264BITSTREAM_INCLUDE_DIRS)

MARK_AS_ADVANCED(H264BITSTREAM_LIBRARIES H264BITSTREAM_INCLUDE_DIRS )