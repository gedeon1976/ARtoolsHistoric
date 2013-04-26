# add definitions to avoid boost qt signals conflict
# add also -D__STDC_CONSTANT_MACROS to CXX Flags for FFMPEG LIBRARIES
#add_definitions(-DQT_NO_KEYWORDS)
set(EXTRA_OPTIONS "-D__STDC_CONSTANT_MACROS")
# enable modules

# qt includes

# Chooses dynamic linkage on Windows for Coin3D and SoQt
#(needs to be explicitly specified) from Adolfo Rodriguez Cmake

if(WIN32)
  add_definitions(-DCOIN_DLL -DSOQT_DLL)
endif(WIN32)

include(${QT_USE_FILE})
set(LIBIOCCOMM_INCLUDE_DIR  "${CMAKE_SOURCE_DIR}/libcomm")
set(HAPTIC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/haptic")
set(mt_DIR "${CMAKE_SOURCE_DIR}/libmt")
set(IMAGE_PROCESS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/imageProcess")
# if (UNIX)
    # set(LIBIOCCOMM_LIBRARIES  "${LIBIOCCOMM_INCLUDE_DIR}/libioccomm.a")
  # else (UNIX)
    # set(LIBIOCCOMM_LIBRARIES  "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/libioccomm.lib")
# endif (UNIX)

# message("libioccomm: "${LIBIOCCOMM_LIBRARIES})
# include_directories(${LIBIOCCOMM_INCLUDE_DIR}) ${Boost_INCLUDE_DIR}

if (UNIX)
  include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} 
  ${COIN3D_INCLUDE_DIRS} ${QUARTER_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIR} 
  ${LIVE555_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIR} ${mt_DIR} ${IMAGE_PROCESS_INCLUDE_DIR} 
  ${OPENHAPTICS_INCLUDE_DIR}) 
  message("live555: "${LIVE555_INCLUDE_DIRS})

else (UNIX)
  include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${PTHREADS_INCLUDE_DIRS}
  ${COIN3D_INCLUDE_DIRS} ${QUARTER_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS} 
  ${LIVE555_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${mt_DIR} ${IMAGE_PROCESS_INCLUDE_DIR} 
  ${OPENHAPTICS_INCLUDE_DIR}) 
  message("live555 Dir: "${LIVE555_INCLUDE_DIRS})

endif (UNIX)

# qt resource file
set(ARtools_RCS ARtools.qrc)
qt4_add_resources(ARtools_RCS_GEN ${ARtools_RCS})

# generating UI's
message("Cmake Src Dir: "${CMAKE_SOURCE_DIR})
file(GLOB Forms_UIS  RELATIVE ${CMAKE_SOURCE_DIR}/src *.ui)
if (QT4_FOUND)
  qt4_wrap_ui(Forms_UIS_H ${Forms_UIS})
  qt4_wrap_cpp(SOURCES_MOC ${Forms_UIS_H})
endif(QT4_FOUND)

# set boost libraries
message("boost : "${Boost_INCLUDE_DIR})
message("boost libraries: "${Boost_LIBRARIES})
message("boost libraries : "${Boost_LIBRARY_DIRS})
message("boost version: "${Boost_VERSION})
message("boost lib: "${Boost_LIB_VERSION})

if (UNIX)
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
  
else (UNIX)

    # This is used with boost 1.42 multithread debug and visual studio 2008
    
   find_library(Boost_DATETIME_LIBRARY libboost_date_time-vc90-mt-gd
                  PATH ${Boost_LIBRARY_DIRS})
   find_library(Boost_SYSTEM_LIBRARY libboost_system-vc90-mt-gd
                  PATH ${Boost_LIBRARY_DIRS})
   find_library(Boost_THREAD_LIBRARY libboost_thread-vc90-mt-gd
                  PATH ${Boost_LIBRARY_DIRS})
   find_library(Boost_IOSTREAMS_LIBRARY libboost_iostreams-vc90-mt-gd
                  PATH ${Boost_LIBRARY_DIRS})
   find_library(Boost_SERIALIZATION_LIBRARY libboost_serialization-vc90-mt-gd
                  PATH ${Boost_LIBRARY_DIRS})
   find_library(Boost_REGEX_LIBRARY libboost_regex-vc90-mt-gd
                  PATH ${Boost_LIBRARY_DIRS})
                  
   set(BOOST_LIBRARIES ${Boost_DATETIME_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} 
    ${Boost_IOSTREAMS_LIBRARY} ${Boost_SERIALIZATION_LIBRARY} ${Boost_REGEX_LIBRARY})

endif (UNIX)

set(ARtools_SRCS ${CMAKE_SOURCE_DIR}/haptic/haptic.cpp ${CMAKE_SOURCE_DIR}/imageProcess/pointer_3D.cpp 
ARtools.cpp DummySink.cpp StreamClientState.cpp ourRTSPClient.cpp client.cpp stereovideo.cpp hapticConnection.cpp LICFs.cpp  EDlines.cpp SoStereoTexture.cpp  
mainWidget.cpp ARScene.cpp main.cpp ${SOURCES_MOC} ${Forms_UIS})
qt4_automoc(${ARtools_SRCS})
add_executable(ARtools ${ARtools_SRCS} ${ARtools_RCS_GEN})


if (UNIX) 
  target_link_libraries(ARtools ${QUARTER_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} 
  ${QT_LIBRARIES} ${SOQT_LIBRARY_DEBUG} ${COIN3D_LIBRARIES} ${OPENGL_LIBRARIES} ${FFMPEG_LIBRARIES} 
  ${LIVE555_LIBRARIES} ${OpenCV_LIBS}  ${OPENHAPTICS_LIBRARIES})
endif (UNIX)
if (WIN32)
  target_link_libraries(ARtools ${PTHREADS_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} 
  ${QT_LIBRARIES} ${SOQT_LIBRARY_DEBUG} ${COIN3D_LIBRARIES} ${OPENGL_LIBRARIES} ${FFMPEG_LIBRARIES} 
  ${LIVE555_LIBRARIES} ${OpenCV_LIBRARIES}  ${OPENHAPTICS_LIBRARIES})
endif (WIN32)


 
