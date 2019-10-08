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
 #    Sets the compiler/linker flags. After the flags you can specify more args:
 #    Release, Debug, RelWithDebInfo, MinSizeRel: Build configs (inclusive)
 #    ReleaseAll: Sets the flags of all three release builds
 #    CACHE: Values are witten with SET_CACHE_ADVANCED
 #    FORCE: When writing to the cache, the values are set anyway
 #    Any variable names (like WIN32, MSVC, etc.): Condition (combined with AND)
 #    You can suffix the condition with a NOT if you wish
 #  Function names:
 #    [ADD/SET/REMOVE]_[COMPILER/LINKER]_FLAGS
 #  Caution: -If you use CACHE after calling the macro without CACHE, the value
 #            Will not be written unless FORCE is specified.
 #          - Also be aware to always specify the flags in quotes.
 #  Example:
 #    REMOVE_COMPILER_FLAGS("/Gm "asdf" -q"test -foo" CXX ReleaseAll NOT UNIX)
 #    This will only remove the CXX (C++) flags on a non Unix system for the
 #    Release, RelWithDebInfo and MinSizeRel configurations. The macros should
 #    be able to cope with "test -foo" as string argument for a flag.
 #

INCLUDE(SeparateFlags)
INCLUDE(SetCacheAdvanced)

# Compiler flags, additional arguments:
# C, CXX: Specify a language, default is both
MACRO(SET_COMPILER_FLAGS _flags)
  _INTERNAL_PARSE_FLAGS_ARGS(SET "C;CXX" "" "${_flags}" "${ARGN}")
ENDMACRO(SET_COMPILER_FLAGS)
# Add flags (flags don't get added twice)
MACRO(ADD_COMPILER_FLAGS _flags)
  _INTERNAL_PARSE_FLAGS_ARGS(APPEND "C;CXX" "" "${_flags}" "${ARGN}")
ENDMACRO(ADD_COMPILER_FLAGS)
# Remove flags
MACRO(REMOVE_COMPILER_FLAGS _flags)
  _INTERNAL_PARSE_FLAGS_ARGS(REMOVE_ITEM "C;CXX" "" "${_flags}" "${ARGN}")
ENDMACRO(REMOVE_COMPILER_FLAGS)


# Linker flags, additional arguments:
# EXE, SHARED, MODULE: Specify a linker mode, default is all three
MACRO(SET_LINKER_FLAGS _flags)
  _INTERNAL_PARSE_FLAGS_ARGS(SET "EXE;SHARED;MODULE" "_LINKER" "${_flags}" "${ARGN}")
ENDMACRO(SET_LINKER_FLAGS)
# Add flags (flags don't get added twice)
MACRO(ADD_LINKER_FLAGS _flags)
  _INTERNAL_PARSE_FLAGS_ARGS(APPEND "EXE;SHARED;MODULE" "_LINKER" "${_flags}" "${ARGN}")
ENDMACRO(ADD_LINKER_FLAGS)
# Remove flags
MACRO(REMOVE_LINKER_FLAGS _flags)
  _INTERNAL_PARSE_FLAGS_ARGS(REMOVE_ITEM "EXE;SHARED;MODULE" "_LINKER" "${_flags}" "${ARGN}")
ENDMACRO(REMOVE_LINKER_FLAGS)


# Internal macro, do not use
# Parses the given additional arguments and sets the flags to the
# corresponding variables.
MACRO(_INTERNAL_PARSE_FLAGS_ARGS _mode _keys _key_postfix _flags)
  SET(_langs)
  SET(_build_types)
  SET(_cond TRUE)
  SET(_invert_condition FALSE)
  SET(_write_to_cache FALSE)
  SET(_force FALSE)
  STRING(REPLACE ";" "|" _key_regex "${_keys}")
  SET(_key_regex "^(${_key_regex})$")

  FOREACH(_arg ${ARGN})
    IF(_arg MATCHES "${_key_regex}")
      LIST(APPEND _langs "${_arg}")
    ELSEIF(   _arg MATCHES "^(Debug|Release|MinSizeRel|RelWithDebInfo)$"
           OR _arg MATCHES "^(DEBUG|RELEASE|MINSIZEREL|RELWITHDEBINFO)$")
      STRING(TOUPPER "${_arg}" _upper_arg)
      LIST(APPEND _build_types ${_upper_arg})
    ELSEIF(_arg STREQUAL "ReleaseAll")
      LIST(APPEND _build_types RELEASE MINSIZEREL RELWITHDEBINFO)
    ELSEIF(_arg STREQUAL "CACHE")
      SET(_write_to_cache TRUE)
    ELSEIF(_arg STREQUAL "FORCE")
      SET(_force TRUE)
    ELSEIF(_arg MATCHES "^[Nn][Oo][Tt]$")
      SET(_invert_condition TRUE)
    ELSE()
      IF(_invert_condition)
        SET(_invert_condition FALSE)
        IF(${_arg})
          SET(_arg_cond FALSE)
        ELSE()
          SET(_arg_cond TRUE)
       ENDIF()
      ELSE()
        SET(_arg_cond ${${_arg}})
      ENDIF()
      IF(_cond AND _arg_cond)
        SET(_cond TRUE)
      ELSE()
        SET(_cond FALSE)
      ENDIF()
    ENDIF()
  ENDFOREACH(_arg)

  # No language specified, use all: C and CXX or EXE, SHARED and MODULE
  IF(NOT DEFINED _langs)
    SET(_langs ${_keys})
  ENDIF()

  IF(_cond)
    FOREACH(_lang ${_langs})
      SET(_varname "CMAKE_${_lang}${_key_postfix}_FLAGS")
      IF(DEFINED _build_types)
        FOREACH(_build_type ${_build_types})
          _INTERNAL_PARSE_FLAGS(${_mode} "${_flags}" ${_varname}_${_build_type} ${_write_to_cache} ${_force})
        ENDFOREACH(_build_type)
      ELSE()
        _INTERNAL_PARSE_FLAGS(${_mode} "${_flags}" ${_varname} ${_write_to_cache} ${_force})
      ENDIF()
    ENDFOREACH(_lang ${_langs})
  ENDIF(_cond)
ENDMACRO(_INTERNAL_PARSE_FLAGS_ARGS)


# Internal macro, do not use
# Modifies the flags according to the mode: set, add or remove
# Also sets flags according to the CACHE and FORCE parameter.
# If only CACHE is specified, SET_CACHE_ADVANCED() is used.
MACRO(_INTERNAL_PARSE_FLAGS _mode _flags _varname _write_to_cache _force)
  SEPARATE_FLAGS("${_flags}" _arg_flag_list)

  IF("${_mode}" STREQUAL "SET")
    # SET
    SET(_flag_list "${_arg_flag_list}")
  ELSE()
    # ADD or REMOVE
    SEPARATE_FLAGS("${${_varname}}" _flag_list)
    IF(NOT _flag_list)
      SET(_flag_list "") # LIST command requires a list in any case
    ENDIF()
    FOREACH(_flag ${_arg_flag_list})
      LIST(${_mode} _flag_list "${_flag}")
    ENDFOREACH(_flag)
  ENDIF()

  LIST(REMOVE_DUPLICATES _flag_list)
  LIST(SORT _flag_list)
  STRING(REPLACE ";" " " _flag_list "${_flag_list}")

  IF(_write_to_cache)
    IF(_force)
      SET(${_varname} "${_flag_list}" CACHE STRING "${${_varname}}" FORCE)
      SET(${_varname} "${_flag_list}" CACHE STRING "${${_varname}}" FORCE)
    ELSE()
      SET_CACHE_ADVANCED(${_varname} STRING "${${_varname}}" "${_flag_list}")
    ENDIF()
  ELSE()
    SET(${_varname} "${_flag_list}")
  ENDIF()
ENDMACRO(_INTERNAL_PARSE_FLAGS)
