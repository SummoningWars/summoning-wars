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
 #    Tries to return all GCC compiler flags set for a target. This also
 #    all definitions of the current directory.
 #

INCLUDE(SeparateFlags)

FUNCTION(GET_GCC_COMPILER_FLAGS _target _flagsvar)

  # General flags
  STRING(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type_upper)
  SET(_flag_str "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${_build_type_upper}}")

  # Include directories
  GET_DIRECTORY_PROPERTY(_include_dirs INCLUDE_DIRECTORIES)
  IF(_include_dirs)
    FOREACH(_directory ${_include_dirs})
      SET(_flag_str "${_flag_str} -I${_directory}")
    ENDFOREACH(_directory)
  ENDIF()

  # For shared libraries linked with gcc, we have to add -fPIC
  GET_TARGET_PROPERTY(_target_type ${_target} TYPE)
  IF(NOT MINGW AND ${_target_type} STREQUAL SHARED_LIBRARY)
    SET(_flag_str "${_flag_str} -fPIC")
  ENDIF()

  # Target compile flags
  GET_TARGET_PROPERTY(_target_flags ${_target} COMPILE_FLAGS)
  IF(_target_flags)
    SET(_flag_str "${_flag_str} ${_target_flags}")
  ENDIF()

  # Definitions from target and directory
  GET_DIRECTORY_PROPERTY(_directory_defs                 COMPILE_DEFINITIONS)
  GET_DIRECTORY_PROPERTY(_directory_defs_build_type      COMPILE_DEFINITIONS_${_build_type_upper})
  GET_TARGET_PROPERTY(_target_defs            ${_target} COMPILE_DEFINITIONS)
  GET_TARGET_PROPERTY(_target_defs_build_type ${_target} COMPILE_DEFINITIONS_${_build_type_upper})
  GET_TARGET_PROPERTY(_target_def_symbol      ${_target} DEFINE_SYMBOL)
  # Prefix them all with a "-D" if the property was found
  FOREACH(_def ${_directory_defs} ${_directory_defs_build_type} ${_target_defs}
               ${_target_defs_build_type} ${_target_def_symbol})
    IF(_def)
      SET(_flag_str "${_flag_str} -D${_def}")
    ENDIF(_def)
  ENDFOREACH(_def)

  SEPARATE_FLAGS("${_flag_str}" _flags)
  LIST(REMOVE_DUPLICATES _flags)
  SET(${_flagsvar} ${_flags} PARENT_SCOPE)

ENDFUNCTION(GET_GCC_COMPILER_FLAGS)
