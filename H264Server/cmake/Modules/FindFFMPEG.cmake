# Find the FFmpeg library
#
# Sets
#   FFMPEG_FOUND.  If false, don't try to use ffmpeg
#   FFMPEG_INCLUDE_DIRS
#   FFMPEG_LIBRARIES

SET( FFMPEG_FOUND "NO" )

IF (WIN32)

  FIND_PATH( FFMPEG_INCLUDE_DIRS libavcodec/avcodec.h
    "/FFmpeg/include"
        )
  FIND_LIBRARY( FFMPEG_avcodec_LIBRARY avcodec
    "/FFmpeg/lib"
        )

  FIND_LIBRARY( FFMPEG_avformat_LIBRARY avformat
    "/FFmpeg/lib"
        )
  
  FIND_LIBRARY( FFMPEG_avutil_LIBRARY avutil
    "/FFmpeg/lib"
        )
        
  FIND_LIBRARY( FFMPEG_avfilter_LIBRARY avfilter
    "/FFmpeg/lib"  
        )

  FIND_LIBRARY( FFMPEG_swscale_LIBRARY swscale
    "/FFmpeg/lib"
        )
  
      IF( FFMPEG_avcodec_LIBRARY )
      IF( FFMPEG_avformat_LIBRARY )

        SET( FFMPEG_FOUND "YES" )
        SET( FFMPEG_LIBRARIES ${FFMPEG_avformat_LIBRARY} ${FFMPEG_avcodec_LIBRARY} )
        IF( FFMPEG_avutil_LIBRARY )
           SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_avutil_LIBRARY} )
        ENDIF( FFMPEG_avutil_LIBRARY )

        IF( FFMPEG_swscale_LIBRARY )
           SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_swscale_LIBRARY} )
        ENDIF( FFMPEG_swscale_LIBRARY )
                
                IF ( FFMPEG_avfilter_LIBRARY)
                        SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_avfilter_LIBRARY})
                ENDIF(FFMPEG_avfilter_LIBRARY)
    

      ENDIF( FFMPEG_avformat_LIBRARY )
      ENDIF( FFMPEG_avcodec_LIBRARY )


ENDIF(WIN32)

IF (UNIX)

  FIND_PATH( FFMPEG_AVCODEC_DIR libavcodec/avcodec.h
  /usr/include
  /usr/local/include)

  FIND_PATH( FFMPEG_AVFORMAT_DIR libavformat/avformat.h
  /usr/include
  /usr/local/include)

 FIND_PATH( FFMPEG_AVUTIL_DIR libavutil/avutil.h
  /usr/include
  /usr/local/include)

  SET(FFMPEG_INCLUDE_DIRS ${FFMPEG_AVCODEC_DIR})

  IF( FFMPEG_INCLUDE_DIRS )
    message("Found ffmpeg include dirs:" ${FFMPEG_INCLUDE_DIRS})
    FIND_PROGRAM( FFMPEG_CONFIG ffmpeg-config
      /usr/bin
      /usr/local/bin
      ${HOME}/bin
    )

    IF( FFMPEG_CONFIG )
      EXEC_PROGRAM( ${FFMPEG_CONFIG} ARGS "--libs avformat" OUTPUT_VARIABLE FFMPEG_LIBS )
      SET( FFMPEG_FOUND "YES" )
      SET( FFMPEG_LIBRARIES "${FFMPEG_LIBS}" )
  
    ELSE( FFMPEG_CONFIG )

      FIND_LIBRARY( FFMPEG_avcodec_LIBRARY libavcodec.so
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /usr/local/lib64
      )

      FIND_LIBRARY( FFMPEG_avformat_LIBRARY libavformat.so
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /usr/local/lib64
      )
  
      FIND_LIBRARY( FFMPEG_avutil_LIBRARY libavutil.so
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /usr/local/lib64
      )

         FIND_LIBRARY( FFMPEG_avfilter_LIBRARY libavfilter.so
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /usr/local/lib64
      )
          
      FIND_LIBRARY( FFMPEG_swscale_LIBRARY libswscale.so
        /usr/lib
        /usr/local/lib
        /usr/lib64
        /usr/local/lib64
        /usr/lib/x86_64-linux-gnu
      )
  
      IF( FFMPEG_avcodec_LIBRARY )
      IF( FFMPEG_avformat_LIBRARY )

        SET( FFMPEG_FOUND "YES" )

	IF( FFMPEG_swscale_LIBRARY )
           SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_swscale_LIBRARY} )
        ENDIF( FFMPEG_swscale_LIBRARY )

	IF ( FFMPEG_avfilter_LIBRARY)
           SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_avfilter_LIBRARY})
        ENDIF(FFMPEG_avfilter_LIBRARY)
	
        SET( FFMPEG_LIBRARIES ${FFMPEG_avformat_LIBRARY} ${FFMPEG_avcodec_LIBRARY} )
        IF( FFMPEG_avutil_LIBRARY )
           SET( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} ${FFMPEG_avutil_LIBRARY} )
        ENDIF( FFMPEG_avutil_LIBRARY )
    
	message("FFMPEG LIBS: "${FFMPEG_LIBRARIES})
	
      ENDIF( FFMPEG_avformat_LIBRARY )
      ENDIF( FFMPEG_avcodec_LIBRARY )

    ENDIF( FFMPEG_CONFIG )

  ENDIF( FFMPEG_INCLUDE_DIRS )

ENDIF (UNIX)

# handle the QUIETLY and REQUIRED arguments and set FFMPEG_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFMPEG DEFAULT_MSG FFMPEG_LIBRARIES FFMPEG_INCLUDE_DIRS)

MARK_AS_ADVANCED(FFMPEG_LIBRARIES FFMPEG_INCLUDE_DIRS )