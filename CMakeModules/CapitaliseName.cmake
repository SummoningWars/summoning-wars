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
 #  Examples:
 #    "orxonox"  --> "Orxonox"
 #    "ORXONOX"  --> "ORXONOX"
 #    "1orxonox" --> "1orxonox"
 #

FUNCTION(CAPITALISE_NAME _name _output_var)
  STRING(SUBSTRING "${_name}" 0 1 _first_letter)
  STRING(TOUPPER "${_first_letter}" _first_letter)
  STRING(REGEX REPLACE "^.(.*)$" "\\1" _rest "${_name}")
  SET(${_output_var} "${_first_letter}${_rest}" PARENT_SCOPE)
ENDFUNCTION(CAPITALISE_NAME)
