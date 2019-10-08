# - Locate GnuWin32
# This module defines
#  GNUWIN_LIBRARY
#  GNUWIN_INCLUDE_DIR, where to find the headers
#
# Created by Augustin Preda. This is mainly intended to be used on Windows, where you would typically find GnuWin32.
# (last change: 2011.10.28)
#
INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(HandleLibraryTypes)

SET (GNU_SRCPATH $ENV{PROGRAMFILES}/GnuWin32 $ENV{GNUWINDIR} ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)
FIND_PATH(GNUWIN_INCLUDE_DIR libintl.h
  PATHS ${GNU_SRCPATH}
  PATH_SUFFIXES include
)

FIND_LIBRARY(GNUWIN_LIBRARY
  NAMES libintl.lib
  PATHS ${GNU_SRCPATH}
  PATH_SUFFIXES lib libs
)

# Handle the REQUIRED argument
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GNUWIN DEFAULT_MSG
    GNUWIN_LIBRARY
    GNUWIN_INCLUDE_DIR
)

# HANDLE_LIBRARY_TYPES(GNUWIN)

MARK_AS_ADVANCED(
    GNUWIN_INCLUDE_DIR
    GNUWIN_LIBRARY
)
