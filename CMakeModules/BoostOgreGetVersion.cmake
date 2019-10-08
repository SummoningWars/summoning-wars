#-------------------------------------------------------------------
# This file is part of the CMake build system for Summoning Wars
# The purpose of this script is to obtain the version of the boost
# library used in the OGRE SDK.
# For the latest info, see http://sumwars.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
# Latest update: 2011.10.30 (Augustin Preda).
#-------------------------------------------------------------------

include(PreprocessorUtils)

macro(boost_ogre_get_version HEADER)
  file(READ ${HEADER} TEMP_VAR_CONTENTS)
  get_preprocessor_entry(TEMP_VAR_CONTENTS BOOST_LIB_VERSION BoostOGRE_LIB_VERSION)
endmacro()
