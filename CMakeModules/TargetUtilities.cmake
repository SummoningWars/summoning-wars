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
 #    Adds a library or an executable like ADD_LIBRARY/ADD_EXECUTABLE, but
 #    accepts a lot more input information. Simply supply the keywords
 #    described below in any order you wish.
 #    The output is then stored in "_arg_ARGNAME" where ARGNAME is the the
 #    name of the switch or list.
 #
 #    Switches: (when given --> TRUE, FALSE otherwise)
 #      FIND_HEADER_FILES: Searches the current directory for all header files
 #                         and adds them to the target.
 #      EXCLUDE_FROM_ALL:  Inherited from ADD_LIBRARY/ADD_EXECUTABLE
 #      ORXONOX_EXTERNAL:  Specify this for third party libraries
 #      NO_DLL_INTERFACE:  Link statically with MSVC
 #      NO_SOURCE_GROUPS:  Don't create msvc source groups
 #      STATIC/SHARED:     Inherited from ADD_LIBRARY
 #      MODULE:            For dynamic module libraries
 #      WIN32:             Inherited from ADD_EXECUTABLE
 #      PCH_NO_DEFAULT:    Do not make precompiled header files default if
 #                         specified with PCH_FILE
 #      NO_INSTALL:        Do not install the target at all
 #
 #    Lists:
 #      LINK_LIBRARIES:    Redirects to TARGET_LINK_LIBRARIES
 #      VERSION:           Set version to the binary
 #      SOURCE_FILES:      Source files for the target
 #      DEFINE_SYMBOL:     Sets the DEFINE_SYMBOL target property
 #      TOLUA_FILES:       Files with tolua interface
 #      PCH_FILE:          Precompiled header file
 #      PCH_EXCLUDE:       Source files to be excluded from PCH support
 #      OUTPUT_NAME:       If you want a different name than the target name
 #  Note:
 #    This function also installs the target!
 #  Prerequisistes:
 #    ORXONOX_DEFAULT_LINK, ORXONOX_CONFIG_FILES
 #  Parameters:
 #    _target_name, ARGN for the macro arguments
 #

INCLUDE(CMakeDependentOption)
INCLUDE(CapitaliseName)
INCLUDE(GenerateToluaBindings)
INCLUDE(ParseMacroArguments)
INCLUDE(SourceFileUtilities)
IF(PCH_COMPILER_SUPPORT)
  INCLUDE(PrecompiledHeaderFiles)
ENDIF()

MACRO(ORXONOX_ADD_LIBRARY _target_name)
  TU_ADD_TARGET(${_target_name} LIBRARY "STATIC;SHARED" ${ARGN})
ENDMACRO(ORXONOX_ADD_LIBRARY)

MACRO(ORXONOX_ADD_EXECUTABLE _target_name)
  TU_ADD_TARGET(${_target_name} EXECUTABLE "WIN32" ${ARGN})
ENDMACRO(ORXONOX_ADD_EXECUTABLE)


MACRO(TU_ADD_TARGET _target_name _target_type _additional_switches)
  CAPITALISE_NAME(${_target_name} _target_name_capitalised)

  # Specify all possible options (either switch or with add. arguments)
  SET(_switches   FIND_HEADER_FILES  EXCLUDE_FROM_ALL  ORXONOX_EXTERNAL
                  NO_DLL_INTERFACE   NO_SOURCE_GROUPS  ${_additional_switches}
                  PCH_NO_DEFAULT     NO_INSTALL        MODULE)
  SET(_list_names LINK_LIBRARIES  VERSION   SOURCE_FILES  DEFINE_SYMBOL
                  TOLUA_FILES     PCH_FILE  PCH_EXCLUDE OUTPUT_NAME)
  PARSE_MACRO_ARGUMENTS("${_switches}" "${_list_names}" ${ARGN})


  # Workaround: Source file properties get lost when leaving a subdirectory
  # Therefore an "H" after a file means we have to set it as HEADER_FILE_ONLY
  FOREACH(_file ${_arg_SOURCE_FILES})
    IF(_file STREQUAL "H")
      SET_SOURCE_FILES_PROPERTIES(${_last_file} PROPERTIES HEADER_FILE_ONLY TRUE)
    ELSE()
      SET(_last_file ${_file})
      LIST(APPEND _${_target_name}_source_files ${_file})
    ENDIF()
  ENDFOREACH(_file)

  # Assemble all header files of the library
  IF(_arg_FIND_HEADER_FILES)
    GET_ALL_HEADER_FILES(_${_target_name}_header_files)
  ENDIF()

  # Remove potential duplicates
  SET(_${_target_name}_files ${_${_target_name}_header_files} ${_${_target_name}_source_files})
  LIST(REMOVE_DUPLICATES _${_target_name}_files)

  # Generate the source groups
  IF(NOT _arg_NO_SOURCE_GROUPS)
    GENERATE_SOURCE_GROUPS(${_${_target_name}_files})

    IF(NOT _arg_ORXONOX_EXTERNAL)
      # Move the prereqs.h file to the config section
      IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_target_name_capitalised}Prereqs.h)
        SOURCE_GROUP("Config" FILES ${_target_name_capitalised}Prereqs.h)
      ENDIF()
      # Add the config files in a special source group
      LIST(APPEND _${_target_name}_files ${ORXONOX_CONFIG_FILES})
      SOURCE_GROUP("Config" FILES ${ORXONOX_CONFIG_FILES})
    ENDIF()
  ENDIF(NOT _arg_NO_SOURCE_GROUPS)

  # TOLUA_FILES
  IF(_arg_TOLUA_FILES)
    GENERATE_TOLUA_BINDINGS(${_target_name_capitalised} _${_target_name}_files
                            INPUTFILES ${_arg_TOLUA_FILES})
  ENDIF()

  # First part (pre target) of precompiled header files
  IF(PCH_COMPILER_SUPPORT AND _arg_PCH_FILE)
    # Provide convenient option to control PCH
    STRING(TOUPPER "${_target_name}" _target_name_upper)
    IF(_arg_PCH_NO_DEFAULT)
      SET(PCH_DEFAULT FALSE)
    ELSE()
      SET(PCH_DEFAULT TRUE)
    ENDIF()
    CMAKE_DEPENDENT_OPTION(PCH_ENABLE_${_target_name_upper}
      "Enable using precompiled header files for library ${_target_name}." ${PCH_DEFAULT} PCH_ENABLE OFF)

    IF(PCH_ENABLE_${_target_name_upper})
      PRECOMPILED_HEADER_FILES_PRE_TARGET(${_target_name} ${_arg_PCH_FILE} _${_target_name}_files EXCLUDE ${_arg_PCH_EXCLUDE})
    ENDIF()
  ENDIF()

  # Certain libraries don't have dllexport/dllimport and can't be linked shared with msvc
  IF(MSVC AND _arg_NO_DLL_INTERFACE)
    SET(_arg_SHARED)
    SET(_arg_STATIC STATIC)
  ENDIF()

  # No warnings needed from third party libraries
  IF(_arg_ORXONOX_EXTERNAL)
    REMOVE_COMPILER_FLAGS("-W3 -W4" MSVC)
    ADD_COMPILER_FLAGS("-w")
  ENDIF()

  # Set default linking if required
  IF(NOT _arg_SHARED AND NOT _arg_STATIC)
    IF("${ORXONOX_DEFAULT_LINK}" STREQUAL "STATIC")
      SET(_arg_STATIC STATIC)
    ELSEIF("${ORXONOX_DEFAULT_LINK}" STREQUAL "SHARED")
      SET(_arg_SHARED SHARED)
    ENDIF()
  ENDIF()

  # MODULE A
  # Always create shared libraries
  IF(_arg_MODULE)
    SET(_arg_SHARED SHARED)
    SET(_arg_STATIC)
  ENDIF()

  # Don't compile header files
  FOREACH(_file ${_${_target_name}_files})
    IF(NOT _file MATCHES "\\.(c|cc|cpp)")
      SET_SOURCE_FILES_PROPERTIES(${_file} PROPERTIES HEADER_FILE_ONLY TRUE)
    ENDIF()
  ENDFOREACH(_file)



  # Add the library/executable
  IF("${_target_type}" STREQUAL "LIBRARY")
    ADD_LIBRARY(${_target_name} ${_arg_STATIC} ${_arg_SHARED}
                ${_arg_EXCLUDE_FROM_ALL} ${_${_target_name}_files})
  ELSE()
    ADD_EXECUTABLE(${_target_name} ${_arg_WIN32} ${_arg_EXCLUDE_FROM_ALL}
                   ${_${_target_name}_files})
  ENDIF()



  # Change library prefix to "lib"
  IF(MSVC AND ${_target_type} STREQUAL "LIBRARY")
    SET_TARGET_PROPERTIES(${_target_name} PROPERTIES
      PREFIX "lib"
    )
  ENDIF()

  # MSVC hack to exclude external library sources from the intellisense database
  # (IntelliSense stops working when adding "-Zm1000" as compile flag. "/Zm1000"
  # would not work because of the slash)
  IF(_arg_ORXONOX_EXTERNAL AND MSVC)
    SET_TARGET_PROPERTIES(${_target_name} PROPERTIES COMPILE_FLAGS "-Zm1000")
  ENDIF()

  # MODULE B
  IF (_arg_MODULE)
    SET_TARGET_PROPERTIES(${_target_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_MODULE_OUTPUT_DIRECTORY} # Windows
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_MODULE_OUTPUT_DIRECTORY} # Unix
    )
    ADD_MODULE(${_target_name})
  ENDIF()

  # LINK_LIBRARIES
  IF(_arg_LINK_LIBRARIES)
    TARGET_LINK_LIBRARIES(${_target_name} ${_arg_LINK_LIBRARIES})
  ENDIF()

  # DEFINE_SYMBOL
  IF(_arg_DEFINE_SYMBOL)
    SET_TARGET_PROPERTIES(${_target_name} PROPERTIES DEFINE_SYMBOL ${_arg_DEFINE_SYMBOL})
  ENDIF()

  # VERSION
  IF(_arg_VERSION)
    SET_TARGET_PROPERTIES(${_target_name} PROPERTIES VERSION ${_arg_VERSION})
  ELSEIF(NOT _arg_ORXONOX_EXTERNAL)
    SET_TARGET_PROPERTIES(${_target_name} PROPERTIES VERSION ${ORXONOX_VERSION})
  ENDIF()

  # OUTPUT_NAME
  IF(_arg_OUTPUT_NAME)
    SET_TARGET_PROPERTIES(${_target_name} PROPERTIES OUTPUT_NAME  ${_arg_OUTPUT_NAME})
  ENDIF()

  # Second part of precompiled header files
  IF(PCH_COMPILER_SUPPORT AND PCH_ENABLE_${_target_name_upper} AND _arg_PCH_FILE)
    PRECOMPILED_HEADER_FILES_POST_TARGET(${_target_name} ${_arg_PCH_FILE})
  ENDIF()

  IF(NOT _arg_STATIC AND NOT _arg_NO_INSTALL)
    IF(_arg_MODULE)
      INSTALL(TARGETS ${_target_name}
        RUNTIME DESTINATION ${MODULE_INSTALL_DIRECTORY}
        LIBRARY DESTINATION ${MODULE_INSTALL_DIRECTORY}
      )
    ELSE()
      INSTALL(TARGETS ${_target_name}
        RUNTIME DESTINATION ${RUNTIME_INSTALL_DIRECTORY}
        LIBRARY DESTINATION ${LIBRARY_INSTALL_DIRECTORY}
      )
    ENDIF()
  ENDIF()

ENDMACRO(TU_ADD_TARGET)


# Creates a helper file with name <name_of_the_library>${ORXONOX_MODULE_EXTENSION}
# This helps finding dynamically loadable modules at runtime

FUNCTION(ADD_MODULE _target)
  # We use the properties to get the name because the librarys name may differ from
  # the target name (for example orxonox <-> liborxonox)

  GET_TARGET_PROPERTY(_target_loc ${_target} LOCATION)
  GET_FILENAME_COMPONENT(_target_name ${_target_loc} NAME_WE)

  IF(CMAKE_CONFIGURATION_TYPES)
    FOREACH(_config ${CMAKE_CONFIGURATION_TYPES})
      SET(_module_filename ${CMAKE_MODULE_OUTPUT_DIRECTORY}/${_config}/${_target_name}${ORXONOX_MODULE_EXTENSION})

      FILE(WRITE ${_module_filename})

      INSTALL(
        FILES ${_module_filename}
        DESTINATION ${MODULE_INSTALL_DIRECTORY}
        CONFIGURATIONS ${_config}
      )
    ENDFOREACH()
  ELSE()
    SET(_module_filename ${CMAKE_MODULE_OUTPUT_DIRECTORY}/${_target_name}${ORXONOX_MODULE_EXTENSION})

    FILE(WRITE ${_module_filename})

    INSTALL(
      FILES ${_module_filename}
      DESTINATION ${MODULE_INSTALL_DIRECTORY}
    )
  ENDIF()
ENDFUNCTION(ADD_MODULE)
