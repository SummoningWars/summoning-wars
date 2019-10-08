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
 #

INCLUDE(GetGCCCompilerFlags)

MACRO(PRECOMPILED_HEADER_FILES_PRE_TARGET _target_name _header_file_arg _sourcefile_var)

  GET_FILENAME_COMPONENT(_pch_header_file ${_header_file_arg} ABSOLUTE)
  GET_FILENAME_COMPONENT(_pch_header_filename ${_pch_header_file} NAME)
  GET_FILENAME_COMPONENT(_pch_header_filename_we ${_pch_header_file} NAME_WE)

  IF(NOT EXISTS ${_pch_header_file})
    MESSAGE(FATAL_ERROR "Specified precompiled headerfile '${_header_file_arg}' does not exist.")
  ENDIF()

  # Extract arguments from ARGN
  FOREACH(_arg ${ARGN})
    IF(NOT "${_arg}" STREQUAL "EXCLUDE")
      IF(NOT _arg_second)
        # Source files with PCH support
        SET(_included_files ${_included_files} ${_arg})
      ELSE()
        # Source files to be excluded from PCH support (easier syntax this way)
        SET(_excluded files ${_excluded_files} ${_arg})
      ENDIF()
    ELSE()
      SET(_arg_second TRUE)
    ENDIF()
  ENDFOREACH(_arg)

  # Use ${_sourcefile_var} if no files were specified explicitely
  IF(NOT _included_files)
    SET(_source_files ${${_sourcefile_var}})
  ELSE()
    SET(_source_files ${_included_files})
  ENDIF()

  # Exclude files (if specified)
  FOREACH(_file ${_excluded_files})
    LIST(FIND _source_files ${_file} _list_index)
    IF(_list_index GREATER -1)
      LIST(REMOVE_AT _source_files _list_index)
    ELSE()
      MESSAGE(FATAL_ERROR "Could not exclude file ${_file} in target ${_target_name}")
    ENDIF()
  ENDFOREACH(_file)

  LIST(FIND ${_sourcefile_var} ${_pch_header_file} _list_index)
  IF(_list_index EQUAL -1) # Header file could already be included with GET_ALL_HEADER_FILES
    LIST(APPEND ${_sourcefile_var} ${_pch_header_file})
  ENDIF()
  SOURCE_GROUP("PCH" FILES ${_pch_header_file})

  IF(MSVC)

    # Write and add one source file, which generates the precompiled header file
    SET(_pch_source_file "${CMAKE_CURRENT_BINARY_DIR}/${_pch_header_filename_we}.cc")
    IF(NOT EXISTS ${_pch_source_file})
      FILE(WRITE ${_pch_source_file} "#include \"${_pch_header_file}\"\n")
    ENDIF()
    SET_SOURCE_FILES_PROPERTIES(_pch_source_file PROPERTIES GENERATED TRUE)
    LIST(APPEND ${_sourcefile_var} ${_pch_source_file})
    SOURCE_GROUP("PCH" FILES ${_pch_source_file})

    SET(_pch_file "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${_pch_header_filename}.pch")
    # Set compile flags for generated source file
    SET_SOURCE_FILES_PROPERTIES(${_pch_source_file} PROPERTIES COMPILE_FLAGS "/c /Yc\"${_pch_header_file}\" /Fp\"${_pch_file}\"")
    # Set Compile flags for the other source files
    FOREACH(_file ${_source_files})
      GET_SOURCE_FILE_PROPERTY(_is_header ${_file} HEADER_FILE_ONLY)
      IF(NOT _is_header)
        GET_SOURCE_FILE_PROPERTY(_old_flags ${_file} COMPILE_FLAGS)
        IF(NOT _old_flags)
          SET(_old_flags "")
        ENDIF()
        SET_SOURCE_FILES_PROPERTIES(${_file} PROPERTIES COMPILE_FLAGS "${_old_flags} /FI\"${_pch_header_file}\" /Yu\"${_pch_header_file}\" /Fp\"${_pch_file}\"")
      ENDIF(NOT _is_header)
    ENDFOREACH(_file)

  ELSEIF(CMAKE_COMPILER_IS_GNU)

    SET(_pch_file "${CMAKE_CURRENT_BINARY_DIR}/${_pch_header_filename}.gch")
    SET(_pch_dep_helper_file "${CMAKE_CURRENT_BINARY_DIR}/${_target_name}PCHDependencyHelper.h")

    # Append the gch-dir to make sure gcc finds the pch file
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

    # Get compiler flags of the source files (target does not yet exist!)
    # This is just the best possible opportunity to address this dependency issue
    GET_GCC_COMPILER_FLAGS(${_target_name} _pch_gcc_flags)
    # Make sure we recompile the pch file even if only the flags change
    IF(NOT "${_pch_gcc_flags}" STREQUAL "${_INTERNAL_${_target_name}_PCH_GCC_FLAGS}" OR NOT EXISTS "${_pch_dep_helper_file}")
      SET(_INTERNAL_${_target_name}_PCH_GCC_FLAGS "${_pch_gcc_flags}" CACHE INTERNAL "")
      FILE(WRITE ${_pch_dep_helper_file} "/* ${_pch_gcc_flags} */")
    ENDIF()

    # Set Compile flags for the other source files
    FOREACH(_file ${_source_files})
      GET_SOURCE_FILE_PROPERTY(_is_header ${_file} HEADER_FILE_ONLY)
      IF(NOT _is_header)
        GET_SOURCE_FILE_PROPERTY(_old_flags ${_file} COMPILE_FLAGS)
        IF(NOT _old_flags)
          SET(_old_flags "")
        ENDIF()
        SET_SOURCE_FILES_PROPERTIES(${_file} PROPERTIES
          COMPILE_FLAGS "${_old_flags} -include ${_pch_header_filename}"
          OBJECT_DEPENDS "${_pch_header_file};${_pch_file}"
        )
      ENDIF(NOT _is_header)
    ENDFOREACH(_file)

  ENDIF()

ENDMACRO(PRECOMPILED_HEADER_FILES_PRE_TARGET)

FUNCTION(PRECOMPILED_HEADER_FILES_POST_TARGET _target_name)
    # This macro is only necessary for GCC
    IF(CMAKE_COMPILER_IS_GNU)

      # Workaround for distcc
      IF(CMAKE_CXX_COMPILER_ARG1)
        # remove leading space in compiler argument
        STRING(REGEX REPLACE "^ +" "" _pch_cmake_cxx_compiler_arg1 "${CMAKE_CXX_COMPILER_ARG1}")
      ENDIF()

      # Get compiler flags of the source files again (target exists this time)
      GET_GCC_COMPILER_FLAGS(${_target_name} _pch_gcc_flags)

      # Compile the header file
      ADD_CUSTOM_COMMAND(
        OUTPUT ${_pch_file}
        COMMAND ${CMAKE_CXX_COMPILER}
        ARGS ${pchsupport_compiler_cxx_arg1} ${_pch_gcc_flags} -c -x c++-header -o ${_pch_file} ${_pch_header_file}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${_pch_dep_helper_file}
        IMPLICIT_DEPENDS CXX ${_pch_header_file}
        VERBATIM
      )

    ENDIF(CMAKE_COMPILER_IS_GNU)
ENDFUNCTION(PRECOMPILED_HEADER_FILES_POST_TARGET)
