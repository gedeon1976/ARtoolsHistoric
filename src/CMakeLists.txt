
# enable modules

# qt includes

include(${QT_USE_FILE})
set(LIBIOCCOMM_INCLUDE_DIR  "${CMAKE_SOURCE_DIR}/libcomm")
set(LIBIOCCOMM_LIBRARIES  "${LIBIOCCOMM_INCLUDE_DIR}/libioccomm.a")
message("libioccomm: "${LIBIOCCOMM_LIBRARIES})
include_directories(${LIBIOCCOMM_INCLUDE_DIR})
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} 
${COIN3D_INCLUDE_DIRS} ${QUARTER_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIR} ${LIVE555_INCLUDE_DIRS} ${Boost_INCLUDE_DIR}) 
message("live555: "${LIVE555_INCLUDE_DIRS})
# generating UI's
file(GLOB Forms_UIS  RELATIVE ${CMAKE_SOURCE_DIR}/src *.ui)
qt4_wrap_ui(Forms_UIS_H ${Forms_UIS})
qt4_wrap_cpp(SOURCES_MOC ${Forms_UIS_H})

# set boost libraries
message("boost : "${Boost_INCLUDE_DIR})
message("boost libraries: "${Boost_LIBRARIES})
message("boost libraries : "${Boost_LIBRARY_DIRS})
message("boost version: "${Boost_VERSION})
message("boost lib: "${Boost_LIB_VERSION})

set(Boost_FILESYSTEM_LIBRARY  "${Boost_LIBRARY_DIRS}/libboost_filesystem-mt.so")
message("boost : "${Boost_FILESYSTEM_LIBRARY})


set(Boost_THREAD_LIBRARY  "${Boost_LIBRARY_DIRS}/libboost_thread-mt.so")
message("boost : "${Boost_THREAD_LIBRARY})

set(Boost_IOSTREAMS_LIBRARY  "${Boost_LIBRARY_DIRS}/libboost_iostreams-mt.so")
message("boost : "${Boost_IOSTREAMS_LIBRARY})

set(Boost_SERIALIZATION_LIBRARY  "${Boost_LIBRARY_DIRS}/libboost_serialization-mt.so")
message("boost : "${Boost_SERIALIZATION_LIBRARY})

set(BOOST_LIBRARIES ${Boost_FILESYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} 
${Boost_IOSTREAMS_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})


set(ARtools_SRCS ARtools.cpp client.cpp stereovideo.cpp hapticConnection.cpp SoStereoTexture.cpp  main.cpp ${SOURCES_MOC} ${Forms_UIS})
qt4_automoc(${ARtools_SRCS})
add_executable(ARtools ${ARtools_SRCS})
target_link_libraries(ARtools ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} 
${QT_LIBRARIES} ${SOQT_LIBRARY_RELEASE} ${COIN3D_LIBRARIES} ${QUARTER_LIBRARIES} ${OPENGL_LIBRARIES} ${FFMPEG_LIBRARIES} 
${LIVE555_LIBRARIES} ${BOOST_LIBRARIES} ${LIBIOCCOMM_LIBRARIES})
 
