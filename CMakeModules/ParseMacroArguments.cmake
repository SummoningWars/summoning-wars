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
 #    Parses a list of macro arguments which are separated by keywords and
 #    writes the result to the _arg_KEYWORD variable. Switches are set to their
 #    literal name if found and the lists contain their elements.
 #    The order of the arguments is arbitrary.
 #  Note:
 #    You have to specify the switches and list_names as a
 #    semicolon separated list or all hell breaks loose!
 #  Example:
 #    The arguments  BLUBB_FILES foo.cc bar.c NO_ASDF  will do the following:
 #    SET(_arg_NO_ASDF _arg_NO_ASDF)
 #    SET(_arg_BLUBB_FILES foo.cc bar.c)
 #    But NO_FOO will not be set at all if it was specified as a switch
 #

MACRO(PARSE_MACRO_ARGUMENTS _switches _list_names)

  # Using LIST(FIND ...) speeds up the process
  SET(_keywords ${_switches} ${_list_names})

  # Reset all arguments
  FOREACH(_arg ${_switches} ${_list_names})
    SET(_arg_${_arg})
  ENDFOREACH(_arg)

  # Parse all the arguments and set the corresponding variable
  # If the option is just a switch, set the variable to its name for later use
  FOREACH(_arg ${ARGN})

    # Is the argument a keyword?
    LIST(FIND _keywords ${_arg} _keyword_index)
    IF(NOT _keyword_index EQUAL -1)

      # Another optimisation
      SET(_arg_found FALSE)
      # Switches
      FOREACH(_switch ${_switches})
        IF(${_switch} STREQUAL ${_arg})
          SET(_arg_${_switch} ${_switch})
          SET(_arg_found TRUE)
          # Avoid interpreting arguments after this one as options args for the previous one
          SET(_storage_var)
          BREAK()
        ENDIF()
      ENDFOREACH(_switch)

      # Input options
      IF(NOT _arg_found)
        FOREACH(_list_name ${_list_names})
          IF(${_list_name} STREQUAL ${_arg})
            SET(_storage_var _arg_${_list_name})
            BREAK()
          ENDIF()
        ENDFOREACH(_list_name)
      ENDIF(NOT _arg_found)

    ELSE()

      # Arguments of an input option (like source files for SOURCE_FILES)
      IF(_storage_var)
        # Store in variable define above in the foreach loop
        SET(${_storage_var} ${${_storage_var}} ${_arg})
      ELSE()
        MESSAGE(FATAL_ERROR "ORXONOX_ADD_${_target_type} was given a non compliant argument: ${_arg}")
      ENDIF(_storage_var)

    ENDIF()

  ENDFOREACH(_arg)
ENDMACRO(PARSE_MACRO_ARGUMENTS)
