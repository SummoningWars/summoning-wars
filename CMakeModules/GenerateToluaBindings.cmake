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
 #    Adrian Friedli
 #  Description:
 #    Sets the CMake options that will generate Tolua++ bindings.
 #  Parameters:
 #    _tolua_package - Name of the package, e.g. "Core"
 #    _target_source_files - Variable name of the target source files, the
 #                           script will add the generated files to this list.
 #    ARGN - The header files in the style "INTPUTFILES Foo.h Bar.h"
 #  Global Variables: (need to be set before)
 #    TOLUA_PARSER_SOURCE - Lua file with the parser source code
 #    TOLUA_PARSER_DEPENDENCIES - All the dependent lua files
 #    RUNTIME_LIBRARY_DIRECTORY - Working directory
 #

FUNCTION(GENERATE_TOLUA_BINDINGS _tolua_package _target_source_files)
  SET(_tolua_inputfiles ${ARGN})
  LIST(REMOVE_ITEM _tolua_inputfiles "INPUTFILES")

  SET(_tolua_pkgfile "${CMAKE_CURRENT_BINARY_DIR}/tolua.pkg")
  SET(_tolua_cxxfile "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/ToluaBind${_tolua_package}.cc")
  SET(_tolua_hfile   "${CMAKE_BINARY_DIR}/src/toluabind/${CMAKE_CFG_INTDIR}/ToluaBind${_tolua_package}.h")

  SET(${_target_source_files}
    ${${_target_source_files}}
    ${_tolua_cxxfile}
    ${_tolua_hfile}
    PARENT_SCOPE
  )
  SOURCE_GROUP("Tolua" FILES ${_tolua_cxxfile} ${_tolua_hfile})
  # Disable annoying GCC warnings
  IF(CMAKE_COMPILER_IS_GNU)
    SET_SOURCE_FILES_PROPERTIES(${_tolua_cxxfile} PROPERTIES COMPILE_FLAGS "-w")
  ENDIF()
  # Tolua binding speedup if required
  IF(ORXONOX_RELEASE)
    SET_SOURCE_FILES_PROPERTIES(${_tolua_cxxfile} PROPERTIES COMPILE_FLAGS "-DTOLUA_RELEASE")
  ENDIF()

  # Create temporary package file and implicit dependencies
  FILE(REMOVE ${_tolua_pkgfile})
  FOREACH(_tolua_inputfile ${_tolua_inputfiles})
    FILE(APPEND ${_tolua_pkgfile} "\$cfile \"${_tolua_inputfile}\"\n")
    LIST(APPEND _implicit_dependencies CXX ${_tolua_inputfile})
  ENDFOREACH(_tolua_inputfile)

  ADD_CUSTOM_COMMAND(
    OUTPUT ${_tolua_cxxfile} ${_tolua_hfile}
    COMMAND tolua++app_orxonox -n ${_tolua_package}
                               -w ${CMAKE_CURRENT_SOURCE_DIR}
                               -o ${_tolua_cxxfile}
                               -H ${_tolua_hfile}
                               -s ${TOLUA_PARSER_SOURCE}
                                  ${_tolua_pkgfile}
    DEPENDS           ${TOLUA_PARSER_DEPENDENCIES}
    IMPLICIT_DEPENDS  ${_implicit_dependencies}
    WORKING_DIRECTORY ${RUNTIME_LIBRARY_DIRECTORY}
    COMMENT "Generating tolua bind files for package ${_tolua_package}"
  )
ENDFUNCTION(GENERATE_TOLUA_BINDINGS)
