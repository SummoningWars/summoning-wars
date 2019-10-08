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
 #    Separates a string of program flags like "--blubb -test -D AA".
 #    " -" or " /" denotes the start of a flag.
 #    The same sequence inside double quotation marks is being ignored.
 #    Spaces not within quotes are cleaned meaningfully.
 #  Note:
 #    This macro cannot cope with semicolons in the flag string!

FUNCTION(SEPARATE_FLAGS _flags _output_variable)
  # Visual studio (esp. IntelliSense) doesn't like dashes to specify arguments
  # Always use foward slashes instead
  IF(MSVC)
    SET(_argument_starter "/")
  ELSE()
    SET(_argument_starter "-")
  ENDIF(MSVC)

  SET(_flags_prep " ${_flags} -")
  # Separate the chunks
  STRING(REPLACE " " " ;" _flag_chunks "${_flags_prep}") # Max loop iterations
  SET(_flag_string)
  SET(_parsed_flags)
  # Loop is necessary because the regex engine is greedy
  FOREACH(_chunk ${_flag_chunks})
    SET(_flag_string "${_flag_string}${_chunk}")
    # Replace all " -" and " /" inside quotation marks
    STRING(REGEX REPLACE "^(([^\"]*\"[^\"]*\")*[^\"]*\"[^\"]*) [/-]([^\"]*\")"
           "\\1@39535493@\\3" _flag_string "${_flag_string}")
    # Extract one flag if possible
    SET(_flag)
    STRING(REGEX REPLACE "^.* [/-](.+)( [/-].*$)" "${_argument_starter}\\1" _flag "${_flag_string}")
    STRING(REGEX REPLACE "^.* [/-](.+)( [/-].*$)" "\\2"  _flag_string "${_flag_string}")
    IF(NOT _flag STREQUAL _flag_string)
      STRING(STRIP "${_flag}" _flag_stripped)
      LIST(APPEND _parsed_flags "${_flag_stripped}")
    ENDIF(NOT _flag STREQUAL _flag_string)
  ENDFOREACH(_chunk)

  # Re-replace all " -" and " /" inside quotation marks
  STRING(REGEX REPLACE "@39535493@" " -" _parsed_flags "${_parsed_flags}")
  SET(${_output_variable} "${_parsed_flags}" PARENT_SCOPE)
ENDFUNCTION(SEPARATE_FLAGS)
