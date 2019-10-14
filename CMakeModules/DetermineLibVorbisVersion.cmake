# ----------------------------------------------------------------------------------------------------------------------
# This file is part of the CMake build system for Summoning Wars (http://sumwars.org/)
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way you like.
# ----------------------------------------------------------------------------------------------------------------------
# 
# The purpose of this script is to determine the version of the vorbisfile library.
# This script provides a set of functions to access the configured vorbisfile installation and read the spec file.
# 
# E.g.: To store the ogre thread provider into the variable "LIBVORBIS_VERSION", use the function call:
#   DETERMINE_LIBVORBIS_VERSION(LIBVORBIS_VERSION ${MY_LIBVORBIS_FOLDER}/libvorbis.spec)
#
# Author: Augustin Preda.
#     Based on the "DetermineVersion.cmake" file by  Reto Grieder.
# Revision history:
# version 1, 2012.01.29: First commit of file. Single function: DETERMINE_LIBVORBIS_VERSION
# 
# ----------------------------------------------------------------------------------------------------------------------


# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# - DETERMINE_LIBVORBIS_VERSION
# 
# ----------------------------------------------------------------------------------------------------------------------


# ----------------------------------------------------------------------------------------------------------------------
# Determine the OGRE Thread provider configured in the header file.
# Parameters:
#   - _tp_var: Output parameter. The variable in which to store the LIBVORBIS_VERSION read from the spec file.
#   - _file: The name and path of the file to search into. This should typically be the "libvorbis.spec" file.
#
# ----------------------------------------------------------------------------------------------------------------------
FUNCTION(DETERMINE_LIBVORBIS_VERSION _tp_var _file)
  IF(EXISTS ${_file})
    FILE(READ ${_file} _file_content)
    SET(_parts ${ARGN})
    LIST(LENGTH _parts _parts_length)
    IF(NOT ${_parts_length} EQUAL 3)
      SET(_parts MAJOR MINOR PATCH)
    ENDIF()

    FOREACH(_part ${_parts})
      STRING(REGEX MATCH "Version: +([0-9]+)" _match ${_file_content})
      IF(_match)
        SET(${_tp_var} ${CMAKE_MATCH_1})
        SET(${_tp_var} ${CMAKE_MATCH_1} PARENT_SCOPE)
      ELSE()
        SET(${_tp_var} "0")
        SET(${_tp_var} "0" PARENT_SCOPE)
      ENDIF()
    ENDFOREACH(_part)
    SET(${_tp_var}_VERSION "${${_tp_var}_VERSION}" PARENT_SCOPE)
  ENDIF(EXISTS ${_file})
ENDFUNCTION(DETERMINE_LIBVORBIS_VERSION)
