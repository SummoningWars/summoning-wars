# ----------------------------------------------------------------------------------------------------------------------
# This file is part of the CMake build system for Summoning Wars (http://sumwars.org/)
#
# The contents of this file are placed in the public domain. Feel free to make use of it in any way you like.
# ----------------------------------------------------------------------------------------------------------------------
# 
# The purpose of this script is to obtain the version of the boost library used either as stand-alone, or delivered with
# the OGRE SDK.
#
# Author: Augustin Preda.
# Revision history:
# version 1, 2012.01.13: First commit of file. Single macro: boost_ogre_get_version
# version 2, 2012.01.13: Also reading  the boost version (not only the boost_lib version.
#-------------------------------------------------------------------

include(PreprocessorUtils)

macro(boost_ogre_get_version HEADER)
  file(READ ${HEADER} TEMP_VAR_CONTENTS)
  # Read the value assigned to "BOOST_LIB_VERSION" and store it into the variable BoostOGRE_LIB_VERSION
  get_preprocessor_entry(TEMP_VAR_CONTENTS BOOST_LIB_VERSION BoostOGRE_LIB_VERSION)
  # Read the value assigned to "BOOST_VERSION" and store it into the variable BoostOGRE_VERSION
  get_preprocessor_entry(TEMP_VAR_CONTENTS BOOST_VERSION BoostOGRE_VERSION)
endmacro()
