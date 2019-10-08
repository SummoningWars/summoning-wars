 #
 #             ORXONOX - the hottest 3D action shooter ever to exist
 #                             > www.orxonox.net <
 #
 #        This program is free software; you can redistribute it and/or
 #         modify it under the terms of the GNU General Public License
 #        as published by the Free Software Foundation; either version 2
 #            of the License, or (at your option) any later version.
 #
 #       This program is distributed in the hope that it will be useful,
 #        but WITHOUT ANY WARRANTY; without even the implied warranty of
 #        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #                 GNU General Public License for more details.
 #
 #   You should have received a copy of the GNU General Public License along
 #      with this program; if not, write to the Free Software Foundation,
 #     Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 #
 #
 #  Author:
 #    Reto Grieder
 #  Description:
 #    Sets the right compiler and linker flags for the Microsoft Compiler.
 #

INCLUDE(FlagUtilities)

################### Compiler Version ####################

# We make use of variadic macros, which is only supported by MSVC 8 and above
IF(MSVC_VERSION LESS 1400)
  MESSAGE(FATAL_ERROR "Microsoft Visual Studio versions below 8 (2005) are not supported because of missing compiler extensions.")
ENDIF()


######################## Options ########################

# Currently VLD has a problem with MSVC9 although it actually is supported
IF(MSVC80)
  OPTION(VISUAL_LEAK_DETECTOR_ENABLE "Memory leak detector" off)
ENDIF()
# Make sure the value is "on" or "off" for vld.ini
IF(VISUAL_LEAK_DETECTOR_ENABLE)
  SET(VISUAL_LEAK_DETECTOR_ENABLE on)
ELSE()
  SET(VISUAL_LEAK_DETECTOR_ENABLE off)
ENDIF()

# Orxonox only supports MSVC 8 and above, which gets asserted above
SET(PCH_COMPILER_SUPPORT TRUE)


#################### Compiler Flags #####################

# -MD    Minimal Rebuild
# -RTC1  Both basic runtime checks
# -MD[d] Multithreaded [debug] DLL
# -Zi    Program Database
# -ZI    Program Database for Edit & Continue
# -WX    General warning Level X
# -wdX   Disable specific warning X
# -wnX   Set warning level of specific warning X to level n

# Overwrite CMake default flags first. Be careful with this
# Only add (not set) the general compiler flags.
# CMake default flags : -DWIN32 -D_WINDOWS -W3 -Zm1000
# additionally for CXX: -EHsc -GR
ADD_COMPILER_FLAGS("-D__WIN32__ -D_WIN32"      CACHE)
ADD_COMPILER_FLAGS("-D_CRT_SECURE_NO_WARNINGS" CACHE)

# Overwrite CMake default flags here.
SET_COMPILER_FLAGS("-MDd -Od -Zi -D_DEBUG -MP -RTC1" Debug          CACHE)
SET_COMPILER_FLAGS("-MD  -O2     -DNDEBUG -MP"       Release        CACHE)
SET_COMPILER_FLAGS("-MD  -O2 -Zi -DNDEBUG -MP"       RelWithDebInfo CACHE)
SET_COMPILER_FLAGS("-MD  -O1     -DNDEBUG -MP"       MinSizeRel     CACHE)
ADD_COMPILER_FLAGS("-D_SECURE_SCL=0" MSVC9 ReleaseAll CACHE)

# Use Link time code generation for Release config if ORXONOX_RELEASE is defined
IF(ORXONOX_RELEASE)
  ADD_COMPILER_FLAGS("-GL" ReleaseAll CACHE)
ENDIF()


####################### Warnings ########################

# Increase warning level if requested
IF(EXTRA_COMPILER_WARNINGS)
  REMOVE_COMPILER_FLAGS("-W1 -W2 -W3" CACHE)
  ADD_COMPILER_FLAGS   ("-W4" CACHE)
ELSE()
  REMOVE_COMPILER_FLAGS("-W1 -W2 -W4" CACHE)
  ADD_COMPILER_FLAGS   ("-W3" CACHE)
ENDIF()

# "<type> needs to have dll-interface to be used by clients'
# Happens on STL member variables which are not public
ADD_COMPILER_FLAGS("-w44251" CACHE)
ADD_COMPILER_FLAGS("-w44275" CACHE) # For inheritance

# Multiple assignment operators specified
ADD_COMPILER_FLAGS("-w44522" CACHE)

# Forcing values to bool
ADD_COMPILER_FLAGS("-w44800" CACHE)

# This warns about truncation to 255 characters in debug/browse info
# ADD_COMPILER_FLAGS("-w44786 -w44503" CACHE)

# conversion from 'double' to 'float', possible loss of data
# conversion from 'ogg_int64_t' to 'long', possible loss of data
# ADD_COMPILER_FLAGS("-w44244" CACHE)

# "conversion from 'size_t' to 'unsigned int', possible loss of data
# ADD_COMPILER_FLAGS("-w44267" CACHE)

# "truncation from 'double' to 'float'
# ADD_COMPILER_FLAGS("-w44305" CACHE)

# "non dll-interface class used as base for dll-interface class"
# ADD_COMPILER_FLAGS("-w44275" CACHE)

# "C++ Exception Specification ignored"
# This is because MSVC 6 did not implement all the C++ exception
# specifications in the ANSI C++ draft.
# ADD_COMPILER_FLAGS("-w44290" CACHE)

# "no suitable definition provided for explicit template
# instantiation request" Occurs in VC7 for no justifiable reason.
# ADD_COMPILER_FLAGS("-w44661" CACHE)

# Deprecation warnings when using CRT calls in VC8
# These show up on all C runtime lib code in VC8, disable since they clutter
# the warnings with things we may not be able to do anything about (e.g.
# generated code from nvparse etc). I doubt very much that these calls
# will ever be actually removed from VC anyway, it would break too much code.
# Note: Probably handled by "-DCRT_SECURE_NO_WARNINGS"
# ADD_COMPILER_FLAGS("-w44996" CACHE)

# "conditional expression constant"
# ADD_COMPILER_FLAGS("-w4201" CACHE)


##################### Linker Flags ######################

# CMake default flags: -MANIFEST -STACK:10000000 -machine:I386
# and INCREMENTAL and DEBUG for debug versions
ADD_LINKER_FLAGS("-OPT:REF -OPT:ICF" Release MinSizeRel CACHE)
ADD_LINKER_FLAGS("-OPT:NOWIN98" MSVC80 CACHE)
REMOVE_LINKER_FLAGS("-INCREMENTAL:NO"  RelWithDebInfo CACHE)
ADD_LINKER_FLAGS   ("-INCREMENTAL:YES" RelWithDebInfo CACHE)

# Use Link time code generation for Release config if ORXONOX_RELEASE is defined
IF(ORXONOX_RELEASE)
  REMOVE_LINKER_FLAGS("-INCREMENTAL:YES" ReleaseAll CACHE)
  ADD_LINKER_FLAGS   ("-INCREMENTAL:NO"  ReleaseAll CACHE)
  ADD_LINKER_FLAGS   ("-LTCG"            ReleaseAll CACHE)
ENDIF()
