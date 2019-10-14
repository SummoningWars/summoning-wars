# - Try to find ogg
# Once done this will define
#
#  OGG_FOUND - system has ogg
#  OGG_INCLUDE_DIR
#  OGG_LIBRARY
#
# $OGGDIR is an environment variable used
# for finding ogg.
#
# Several changes and additions by Fabian 'x3n' Landau
# Most of all rewritten by Adrian Friedli
# Debug versions and simplifications by Reto Grieder
#                 > www.orxonox.net <

INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(HandleLibraryTypes)

SET (OGG_SEARCH_PATH ${PROJECT_SOURCE_DIR}/dependencies/libogg ${PROJECT_SOURCE_DIR}/../dependencies/libogg $ENV{OGGDIR})

FIND_PATH(OGG_INCLUDE_DIR ogg/ogg.h
  PATHS ${OGG_SEARCH_PATH}
  PATH_SUFFIXES include
)

MESSAGE (STATUS "Searching OGG_SEARCH_PATH: ${OGG_SEARCH_PATH}")

IF (NOT OGG_STATIC)
  FIND_FILE (OGG_BINARY_REL 
      NAMES "ogg.dll" "libogg.dll"
      HINTS ${OGG_SEARCH_PATH}
      PATH_SUFFIXES "" Release relwithdebinfo minsizerel
  )
        
  FIND_FILE (OGG_BINARY_DBG 
      NAMES "oggd.dll" "ogg_d.dll" "liboggd.dll" "libogg_d.dll" 
      HINTS ${OGG_SEARCH_PATH}
      PATH_SUFFIXES "" debug
  )
ENDIF ()


IF (WIN32)
  #Make a special selection for the scenario of using visual studio 2010, since this will
  #be the most frequent at this stage.
  SET (TMP_SUFF Win32/Release)
  IF (MSVC10)
    SET (TMP_SUFF ${TMP_SUFF} VS2010/Win32/Release Win32/VS2010/Win32/Release )
  ENDIF (MSVC10)
  IF (MSVC11)
    SET (TMP_SUFF ${TMP_SUFF} VS2012/Win32/Release Win32/VS2012/Win32/Release )
  ENDIF (MSVC11)
  
  FIND_LIBRARY(OGG_LIBRARY_OPTIMIZED
      NAMES libogg libogg_static libogg-static-mt
      PATHS ${OGG_SEARCH_PATH}
      PATH_SUFFIXES lib lib/Release Release ${TMP_SUFF}
  )
ELSE (WIN32)
  FIND_LIBRARY(OGG_LIBRARY_OPTIMIZED
    NAMES ogg libogg libogg_static libogg-static-mt
    PATH_SUFFIXES lib lib/Release
  )
ENDIF(WIN32)


IF(WIN32)
  #Make a special selection for the scenario of using visual studio 2010, since this will
  #be the most frequent at this stage.
  SET (TMP_SUFF Win32/Debug)
  IF (MSVC10)
    SET (TMP_SUFF ${TMP_SUFF} VS2010/Win32/Debug Win32/VS2010/Win32/Debug )
  ENDIF (MSVC10)
  IF (MSVC11)
    SET (TMP_SUFF ${TMP_SUFF} VS2012/Win32/Debug Win32/VS2012/Win32/Debug )
  ENDIF (MSVC11)
  
  FIND_LIBRARY(OGG_LIBRARY_DEBUG
    NAMES liboggd libogg_static libogg_d liboggD libogg_D libogg-static-mt-debug
    PATHS ${OGG_SEARCH_PATH}
    PATH_SUFFIXES lib lib/Debug Debug ${TMP_SUFF}
  )
ELSE (WIN32)
  FIND_LIBRARY(OGG_LIBRARY_DEBUG
    NAMES ogg liboggd
    PATH_SUFFIXES lib lib/Debug
  )
ENDIF(WIN32)


# Handle the REQUIRED argument and set OGG_FOUND
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Ogg DEFAULT_MSG
  OGG_LIBRARY_OPTIMIZED
  OGG_INCLUDE_DIR
)

# Collect optimized and debug libraries
HANDLE_LIBRARY_TYPES(OGG)

MARK_AS_ADVANCED(
  OGG_INCLUDE_DIR
  OGG_LIBRARY_OPTIMIZED
  OGG_LIBRARY_DEBUG
)
