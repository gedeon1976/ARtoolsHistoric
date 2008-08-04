FIND_PATH(COIN_INCLUDE_DIR Inventor/So.h
    ${CMAKE_INCLUDE_PATH}
    $ENV{COIN3DDIR}/include
    /usr/local/coin/include
    /usr/local/Coin/include
    /usr/local/include
    /usr/include
    $ENV{ProgramFiles}/Coin3D-2/include
)

IF (NOT COIN_INCLUDE_DIR_FOUND)
    IF (COIN_INCLUDE_DIR)
        MESSAGE(STATUS "Looking for Coin3D headers -- found " ${COIN_INCLUDE_DIR}/Inventor/So.h)
        SET(COIN_INCLUDE_DIR_FOUND 1 CACHE INTERNAL "Coin3D headers found")
    ELSE (COIN_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR 
            "-- Looking for Coin3D headers -- not found\n"
            "Please install Coin3D http://www.coin3d.org/ or adjust CMAKE_INCLUDE_PATH\n"
            "e.g. cmake -DCMAKE_INCLUDE_PATH=/path-to-Coin/include ..."
        )
    ENDIF (COIN_INCLUDE_DIR)
ENDIF (NOT COIN_INCLUDE_DIR_FOUND)

FIND_LIBRARY(COIN_LIBRARY
    NAMES Coin coin2
    PATHS
    ${CMAKE_LIBRARY_PATH}
    $ENV{COIN3DDIR}/lib
    /usr/local/coin/lib
    /usr/local/lib
    /usr/lib
    $ENV{ProgramFiles}/Coin3D-2/lib
)

IF (NOT COIN_LIBRARY_FOUND)
    IF (COIN_LIBRARY)
        MESSAGE(STATUS "Looking for Coin3D library -- found " ${COIN_LIBRARY})
        SET(COIN_LIBRARY_FOUND 1 CACHE INTERNAL "Coin3D library found")
    ELSE (COIN_LIBRARY)
        MESSAGE(FATAL_ERROR 
            "-- Looking for Coin3D library -- not found\n"
            "Please install Coin3D http://www.coin3d.org/ or adjust CMAKE_LIBRARY_PATH\n"
            "e.g. cmake -DCMAKE_LIBRARY_PATH=/path-to-Coin/lib ..."
        )
    ENDIF (COIN_LIBRARY)
ENDIF (NOT COIN_LIBRARY_FOUND)

MARK_AS_ADVANCED(
    COIN_INCLUDE_DIR
    COIN_LIBRARY
) 
# 
# 
# #############################################################################
# #
# # $Id: FindCOIN.cmake,v 1.4 2007/03/22 09:24:04 asaunier Exp $
# #
# # Copyright (C) 1998-2006 Inria. All rights reserved.
# #
# # This software was developed at:
# # IRISA/INRIA Rennes
# # Projet Lagadic
# # Campus Universitaire de Beaulieu
# # 35042 Rennes Cedex
# # http://www.irisa.fr/lagadic
# #
# # This file is part of the ViSP toolkit
# #
# # This file may be distributed under the terms of the Q Public License
# # as defined by Trolltech AS of Norway and appearing in the file
# # LICENSE included in the packaging of this file.
# #
# # Licensees holding valid ViSP Professional Edition licenses may
# # use this file in accordance with the ViSP Commercial License
# # Agreement provided with the Software.
# #
# # This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# # WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
# #
# # Contact visp@irisa.fr if any conditions of this licensing are
# # not clear to you.
# #
# # Description:
# # Try to find Coin library .
# # Once run this will define: 
# #
# # COIN_FOUND
# # COIN_LIBRARIES
# # COIN_LIBRARY_DEBUG
# # COIN_LIBRARY_RELEASE
# #
# # Authors:
# # Fabien Spindler
# #
# #############################################################################
# 
# IF(UNIX OR WIN32) 
#   
# IF(WIN32)
#  FIND_LIBRARY(COIN_LIBRARY_RELEASE
#     NAMES coin2 #only shared libraries under windows
#     PATHS
#      "$ENV{COINDIR}/lib"	  
#     )
# 
#  FIND_LIBRARY(COIN_LIBRARY_DEBUG
#     NAMES coin2d #only shared libraries under windows
#     PATHS
#     "$ENV{COINDIR}/lib"	  
#     )
#  MARK_AS_ADVANCED(
#       COIN_LIBRARY_RELEASE
#       COIN_LIBRARY_DEBUG
#  )
# 
# ELSE(WIN32)
#   FIND_LIBRARY(COIN_LIBRARY
#     NAMES Coin #only shared libraries under windows
#     PATHS
#      "$ENV{COINDIR}/lib"	  
#     )
#   
#   #MESSAGE(STATUS "DBG COIN_LIBRARY=${COIN_LIBRARY}")
# ENDIF(WIN32)
#   
#   ## --------------------------------
#   
#   IF(COIN_LIBRARY OR COIN_LIBRARY_RELEASE OR COIN_LIBRARY_DEBUG)
#     	IF(WIN32)
# 	 	IF(COIN_LIBRARY_RELEASE AND NOT COIN_LIBRARY_DEBUG)
# 			 SET(COIN_LIBRARY_RELEASE ${COIN_LIBRARY_RELEASE})			 
# 		ENDIF(COIN_LIBRARY_RELEASE AND NOT COIN_LIBRARY_DEBUG)
# 		IF(COIN_LIBRARY_DEBUG AND NOT COIN_LIBRARY_RELEASE)
# 			 SET(COIN_LIBRARY_DEBUG ${COIN_LIBRARY_DEBUG})			 
# 		ENDIF(COIN_LIBRARY_DEBUG AND NOT COIN_LIBRARY_RELEASE)
# 		IF(COIN_LIBRARY_RELEASE AND COIN_LIBRARY_DEBUG)
# 			 SET(COIN_LIBRARY_RELEASE ${COIN_LIBRARY_RELEASE})
# 			 SET(COIN_LIBRARY_DEBUG ${COIN_LIBRARY_DEBUG})			
# 		ENDIF(COIN_LIBRARY_RELEASE AND COIN_LIBRARY_DEBUG)
# 	ELSE(WIN32)
# 		SET(COIN_LIBRARIES ${COIN_LIBRARY})
# 	    MARK_AS_ADVANCED(
# 	        COIN_LIBRARIES
#      		COIN_LIBRARY
#     	    )
# 
# 	ENDIF(WIN32)
#     SET(COIN_FOUND TRUE)
#   ELSE(COIN_LIBRARY OR COIN_LIBRARY_RELEASE OR COIN_LIBRARY_DEBUG)
#     SET(COIN_FOUND FALSE)
#     #MESSAGE("Coin library not found.")
#   ENDIF(COIN_LIBRARY OR COIN_LIBRARY_RELEASE OR COIN_LIBRARY_DEBUG)
# 
#   #MESSAGE(STATUS "COIN_FOUND : ${COIN_FOUND}")
# 
# ELSE(UNIX OR WIN32)
# 	SET(COIN_FOUND FALSE)
# ENDIF(UNIX OR WIN32) 
