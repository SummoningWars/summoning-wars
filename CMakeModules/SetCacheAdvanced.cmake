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
 #    Write to the cache by force, but only if the user hasn't changed the value
 #    Additional argument is the value (may also be a list)

FUNCTION(SET_CACHE_ADVANCED _varname _type _docstring)
  SET(_value ${ARGN})
  IF(NOT "${_type}" MATCHES "^(STRING|BOOL|PATH|FILEPATH)$")
    MESSAGE(FATAL_ERROR "${_type} is not a valid CACHE entry type")
  ENDIF()

  IF(NOT DEFINED _INTERNAL_${_varname} OR "${_INTERNAL_${_varname}}" STREQUAL "${${_varname}}")
    SET(${_varname} "${_value}" CACHE ${_type} "${_docstring}" FORCE)
    SET(_INTERNAL_${_varname} "${_value}" CACHE INTERNAL "Do not edit in any case!")
  ENDIF()
ENDFUNCTION(SET_CACHE_ADVANCED)
