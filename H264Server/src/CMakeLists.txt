# add definitions to avoid boost qt signals conflict
add_definitions(-DQT_NO_KEYWORDS)
# add also -D__STDC_CONSTANT_MACROS to CXX Flags

# add include directories
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${FFMPEG_INCLUDE_DIRS} ${LIVE555_INCLUDE_DIRS}
		    ${H264BITSTREAM_INCLUDE_DIRS})

# generate UI's files *.h and *.cpp
message("CMAKE_SOURCE_DIR:" ${CMAKE_SOURCE_DIR})
file(GLOB Forms_UIS RELATIVE ${CMAKE_SOURCE_DIR}/src *.ui)
if(QT4_FOUND)
  qt4_wrap_ui(Forms_UIS_H ${Forms_UIS})
  qt4_wrap_cpp(SOURCES_MOC ${Forms_UIS_H})
endif(QT4_FOUND)


# set source files
set(H264Server_SRCS blueCherryCard.cpp H264Server.cpp main.cpp ${SOURCES_MOC} ${Forms_UIS})
qt4_automoc(${H264Server_SRCS})
add_executable(H264Server ${H264Server_SRCS})

#generate executable files
if(UNIX)
	target_link_libraries(H264Server ${FFMPEG_avcodec_LIBRARY} 
		${FFMPEG_avformat_LIBRARY} ${FFMPEG_avutil_LIBRARY} ${FFMPEG_swscale_LIBRARY} ${QT_QTGUI_LIBRARY} 
		${QT_LIBRARIES}  ${LIVE555_LIBRARIES} ${H264BITSTREAM_LIBRARIES})
		message("Linking with: " ${FFMPEG_avcodec_LIBRARY})
		
endif(UNIX)
if(WIN32)
	target_link_libraries(H264Server ${QT_QTGUI_LIBRARY} 
		${QT_LIBRARIES} ${FFMPEG_LIBRARIES} ${H264BITSTREAM_LIBRARIES})#${LIVE555_LIBRARIES}
endif(WIN32)