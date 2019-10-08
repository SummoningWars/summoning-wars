# Find CEGUI includes and library
#
# This module defines
#  CEGUI_INCLUDE_DIR
#  CEGUI_LIBRARY, the library to link against to use CEGUI.
#  CEGUI_FOUND, If false, do not try to use CEGUI
#  CEGUI_VERSION, the version as string "x.y.z"
#  CEGUILUA_LIBRARY, Script module library
#  CEGUILUA_USE_INTERNAL_LIBRARY, True if CEGUILUA_LIBRARY was not defined here
#
# Input:
#  ENV{CEGUIDIR}, CEGUI path, optional
#  FIND CEGUILUA_INTERNAL_SUPPORT, List of all CEGUILua version supported
#                                  in the source repository
#  CEGUILUA_USE_EXTERNAL_LIBRARY, Force finding of CEGUILua
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

FIND_PATH(CEGUI_INCLUDE_DIR CEGUI.h
  PATHS $ENV{CEGUIDIR}
  PATH_SUFFIXES /include /cegui/include /include/CEGUI CEGUI.framework/Headers
)

FIND_LIBRARY(CEGUI_LIBRARY_OPTIMIZED
  NAMES CEGUIBase CEGUI
  PATHS $ENV{CEGUIDIR}
  PATH_SUFFIXES lib bin
)
FIND_LIBRARY(CEGUI_LIBRARY_DEBUG
  NAMES
    CEGUIBased CEGUIBase_d CEGUIBaseD CEGUIBase_D
    CEGUId CEGUI_d CEGUID CEGUI_D
  PATHS $ENV{CEGUIDIR}
  PATH_SUFFIXES lib bin
)


# Inspect CEGUIVersion.h for the version number
DETERMINE_VERSION(CEGUI ${CEGUI_INCLUDE_DIR}/CEGUIVersion.h)

# Handle the REQUIRED argument and set CEGUI_FOUND
# Also checks the version requirements if given
FIND_PACKAGE_HANDLE_ADVANCED_ARGS(CEGUI DEFAULT_MSG "${CEGUI_VERSION}"
  CEGUI_LIBRARY_OPTIMIZED
  CEGUI_INCLUDE_DIR
)

# Collect optimized and debug libraries
HANDLE_LIBRARY_TYPES(CEGUI)

MARK_AS_ADVANCED(
  CEGUI_INCLUDE_DIR
  CEGUI_LIBRARY_OPTIMIZED
  CEGUI_LIBRARY_DEBUG
)

#LIST(FIND CEGUILUA_INTERNAL_SUPPORT "${CEGUI_VERSION}" _find_result)
#IF(CEGUILUA_USE_EXTERNAL_LIBRARY OR _find_result EQUAL -1)
  # Also try to find the CEGUILua libraries.
  # There would already be libraries in src/ for versions 0.5 and 0.6
#FIND_LIBRARY(CEGUILUA_LIBRARY_OPTIMIZED
#	NAMES CEGUILuaScriptModule	
#	PATHS $ENV{CEGUIDIR}
#	PATH_SUFFIXES lib bin
#)
#FIND_LIBRARY(CEGUILUA_LIBRARY_DEBUG
#	NAMES CEGUILuaScriptModuled CEGUILuaScriptModule_d
#	PATHS $ENV{CEGUIDIR}
#	PATH_SUFFIXES lib bin
#)

#SET(CEGUILua_FIND_REQUIRED ${CEGUI_FIND_REQUIRED})
  # Handle the REQUIRED argument and set CEGUILUA_FOUND
#	FIND_PACKAGE_HANDLE_STANDARD_ARGS(CEGUILua DEFAULT_MSG
#	CEGUILUA_LIBRARY_OPTIMIZED
#)

#Collect optimized and debug libraries
#HANDLE_LIBRARY_TYPES(CEGUILUA)

#MARK_AS_ADVANCED(
#	CEGUILUA_LIBRARY_OPTIMIZED
#	CEGUILUA_LIBRARY_DEBUG
#)

#ELSE(CEGUILUA_USE_EXTERNAL_LIBRARY OR _find_result EQUAL -1)
#	SET(CEGUILUA_USE_INTERNAL_LIBRARY TRUE)
#ENDIF(CEGUILUA_USE_EXTERNAL_LIBRARY OR _find_result EQUAL -1)

