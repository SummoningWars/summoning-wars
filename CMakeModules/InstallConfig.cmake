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
 #    Configures the installation (paths, rpaths, options)
 #

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT) # Variable provided by CMake
  IF("$ENV{ORXONOX_DEV}" OR TARDIS)
    SET(_install_prefix_changed 1)
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE)
  ENDIF()
ENDIF()

SET(_info_text "Puts all installed files in subfolders of the install prefix path. That root folder can then be moved, copied and renamed as you wish. The executable will not write to folders like ~/.orxonox or \"Applictation Data\"")
IF(UNIX AND NOT _install_prefix_changed)
  OPTION(INSTALL_COPYABLE "${_info_text}" FALSE)
ELSE()
  OPTION(INSTALL_COPYABLE "${_info_text}" TRUE)
ENDIF()

# Default installation paths
SET(RUNTIME_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${DEFAULT_RUNTIME_PATH})
SET(LIBRARY_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${DEFAULT_LIBRARY_PATH})
SET(ARCHIVE_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${DEFAULT_ARCHIVE_PATH})
SET(MODULE_INSTALL_DIRECTORY  ${CMAKE_INSTALL_PREFIX}/${DEFAULT_MODULE_PATH})
SET(DOC_INSTALL_DIRECTORY     ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DOC_PATH})
SET(DATA_INSTALL_DIRECTORY    ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH})
SET(CONFIG_INSTALL_DIRECTORY  ${CMAKE_INSTALL_PREFIX}/${DEFAULT_CONFIG_PATH})
SET(LOG_INSTALL_DIRECTORY     ${CMAKE_INSTALL_PREFIX}/${DEFAULT_LOG_PATH})

IF(NOT INSTALL_COPYABLE)
  IF(UNIX) # Apple too?
    # Using absolute paths
    SET(RUNTIME_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
    SET(LIBRARY_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/orxonox)
    SET(ARCHIVE_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib/orxonox/static)
    SET(MODULE_INSTALL_DIRECTORY  ${CMAKE_INSTALL_PREFIX}/lib/orxonox/modules)
    SET(DOC_INSTALL_DIRECTORY     ${CMAKE_INSTALL_PREFIX}/share/doc/orxonox)
    SET(DATA_INSTALL_DIRECTORY    ${CMAKE_INSTALL_PREFIX}/share/orxonox)
  ENDIF()

  # Leave empty because it is user and therefore runtime dependent
  SET(CONFIG_INSTALL_DIRECTORY)
  SET(LOG_INSTALL_DIRECTORY)
ENDIF()

################## Unix rpath ###################

# When building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# The RPATH to be used when installing
IF(INSTALL_COPYABLE)
  SET(CMAKE_INSTALL_RPATH ${DEFAULT_LIBRARY_PATH})
ELSE()
  SET(CMAKE_INSTALL_RPATH ${LIBRARY_INSTALL_DIRECTORY})
ENDIF()

# Add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
