# Find CEGUI includes and library
#
# This module defines
#  CEGUIOGRE_INCLUDE_DIRS
#  CEGUIOGRE_LIBRARY_OPTIMIZED, the library to link against to use CEGUIOGRE Release.
#  CEGUIOGRE_LIBRARY_DEBUG, the library to link against to use CEGUIOGRE Debug.
#  CEGUIOGRE_FOUND, If false, do not try to use CEGUI
#
# Input:
#  ENV{CEGUIDIR}, CEGUI path, optional
#
# Created by Matt Williams to find OGRE libraries
# Copyright © 2007, Matt Williams
#
# Modified by Nicolas Schlumberger to find CEGUI libraries
# and make it run on the Tardis-Infrastucture of the ETH Zurich
# Copyright 2007, Nicolas Schlumberger
#
# Redistribution and use is allowed according to the terms of the BSD license.
#
# Several changes and additions by Fabian 'x3n' Landau
# Lots of simplifications by Adrian Friedli and Reto Grieder
# Version checking and CEGUILua finding by Reto Grieder
#                 > www.orxonox.net <

INCLUDE(DetermineVersion)
INCLUDE(FindPackageHandleAdvancedArgs)
INCLUDE(HandleLibraryTypes)

FIND_PATH(CEGUIOGRE_INCLUDE_DIRS CEGUIOgreRenderer.h
  PATHS $ENV{CEGUIDIR}
  PATH_SUFFIXES include/RendererModules/Ogre cegui/include/RendererModules/Ogre include/CEGUI/RendererModules/Ogre CEGUI.framework/Headers
)
FIND_LIBRARY(CEGUIOGRE_LIBRARY_OPTIMIZED
  NAMES CEGUIOgreRenderer CEGUI
  PATHS $ENV{CEGUIDIR}
  PATH_SUFFIXES lib bin
)
FIND_LIBRARY(CEGUIOGRE_LIBRARY_DEBUG
  NAMES
  CEGUIOgreRendererd CEGUIOgreRenderer_d CEGUIOgreRendererD CEGUIOgreRenderer_D
  PATHS $ENV{CEGUIDIR}
  PATH_SUFFIXES lib bin
)

# Collect optimized and debug libraries
HANDLE_LIBRARY_TYPES(CEGUIOGRE)

FIND_PACKAGE_HANDLE_ADVANCED_ARGS(CEGUIOGRE DEFAULT_MSG
  CEGUIOGRE_LIBRARY_OPTIMIZED
  CEGUIOGRE_INCLUDE_DIRS
)

MESSAGE(STATUS "Found CEGUI include dir: ${CEGUIOGRE_INCLUDE_DIRS}")

MARK_AS_ADVANCED(
  CEGUIOGRE_INCLUDE_DIRS
  CEGUIOGRE_LIBRARY_OPTIMIZED
  CEGUIOGRE_LIBRARY_DEBUG
)
