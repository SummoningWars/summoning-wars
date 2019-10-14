#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find OIS
# Once done, this will define
#
#  OIS_FOUND - system has OIS
#  OIS_INCLUDE_DIRS - the OIS include directories 
#  OIS_LIBRARIES - link these to use OIS

include(FindPkgMacros)
findpkg_begin(OIS)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(OIS_HOME)
getenv_path(OGRE_BYATIS)
getenv_path(OGRE_HOME)
getenv_path(OGRE_DEPENDENCIES_DIR)

# construct search paths
set(OIS_PREFIX_PATH ${OIS_HOME} ${ENV_OIS_HOME} 
  ${OGRE_BYATIS}/sdk ${ENV_OGRE_BYATIS}/sdk/include ${ENV_OGRE_BYATIS}/sdk/lib ${ENV_OGRE_BYATIS})
create_search_paths(OIS)
# redo search if prefix path changed
clear_if_changed(OIS_PREFIX_PATH
  OIS_LIBRARY_FWK
  OIS_LIBRARY_REL
  OIS_LIBRARY_DBG
  OIS_INCLUDE_DIR
)

set(OIS_LIBRARY_NAMES OIS)
get_debug_names(OIS_LIBRARY_NAMES)

use_pkgconfig(OIS_PKGC OIS)

findpkg_framework(OIS)

find_path(OIS_INCLUDE_DIR 
  NAMES OIS.h 
  HINTS ${OIS_INC_SEARCH_PATH} ${OIS_PREFIX_PATH} ${OIS_PKGC_INCLUDE_DIRS} ${ENV_OGRE_HOME}/Dependencies/include
  ${ENV_OGRE_DEPENDENCIES_DIR}
  PATH_SUFFIXES OIS include includes
)

find_library(OIS_LIBRARY_REL 
  NAMES ${OIS_LIBRARY_NAMES} 
  HINTS ${OIS_LIB_SEARCH_PATH} ${OIS_PREFIX_PATH} ${OIS_PKGC_LIBRARY_DIRS} ${ENV_OGRE_HOME}/Dependencies/lib/Release 
  ${ENV_OGRE_DEPENDENCIES_DIR}
  PATH_SUFFIXES "" release relwithdebinfo minsizerel lib/release
)
find_library(OIS_LIBRARY_DBG 
  NAMES ${OIS_LIBRARY_NAMES_DBG} 
  HINTS ${OIS_LIB_SEARCH_PATH} ${OIS_PREFIX_PATH} ${OIS_PKGC_LIBRARY_DIRS} ${ENV_OGRE_HOME}/Dependencies/lib/Debug 
  ${ENV_OGRE_DEPENDENCIES_DIR}
  PATH_SUFFIXES "" debug lib/debug
)
make_library_set(OIS_LIBRARY)

IF (NOT OIS_STATIC)
  FIND_FILE (OIS_BINARY_REL 
      NAMES "ois.dll" 
      HINTS ${OIS_LIB_SEARCH_PATH} ${OIS_PREFIX_PATH} ${OIS_PKGC_LIBRARY_DIRS} ${ENV_OGRE_HOME}/Dependencies/lib/Release
      ${ENV_OGRE_HOME}/bin/Release
      ${ENV_OGRE_DEPENDENCIES_DIR}
      PATH_SUFFIXES "" Release relwithdebinfo minsizerel bin/Release
  )
        
  FIND_FILE (OIS_BINARY_DBG 
      NAMES "ois.dll" "oisd.dll" 
      HINTS ${OIS_LIB_SEARCH_PATH} ${OIS_PREFIX_PATH} ${OIS_PKGC_LIBRARY_DIRS} ${ENV_OGRE_HOME}/Dependencies/lib/Debug
      ${ENV_OGRE_HOME}/bin/Debug
      ${ENV_OGRE_DEPENDENCIES_DIR}
      PATH_SUFFIXES "" debug bin/debug
  )
ENDIF ()


findpkg_finish(OIS)
add_parent_dir(OIS_INCLUDE_DIRS OIS_INCLUDE_DIR)

