cmake_minimum_required(VERSION 2.6)
project(SUMWARS)


OPTION(NO_TINYXML "Do not use in-source TinyXML Lib" OFF)
OPTION(NO_ENET "Do not use in-source ENET Lib" OFF)
OPTION(BUILD_TOOLS "Builds editor tools" ON)
OPTION(UPDATE_SUBVERSION_REVISION "Option to turn off automatic revision checking" ON)
OPTION(POST_BUILD_COPY "After a build, copy the executable to the project folder (Win32)" OFF)

IF( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE "RelWithDebInfo" )
ENDIF()
MESSAGE("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})


IF(UPDATE_SUBVERSION_REVISION)
    FIND_PROGRAM(Subversion_SVN_EXECUTABLE svn)

    SET(Project_WC_REVISION "unknown")

#IF(Subversion_SVN_EXECUTABLE)
#    EXECUTE_PROCESS(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#                    COMMAND ${Subversion_SVN_EXECUTABLE} info
#                    RESULT_VARIABLE ret_var
#                    OUTPUT_VARIABLE output_var
#    )
#    STRING(REGEX REPLACE "^(.*\n)?Revision: ([^\n]+).*"
#        "\\2" Project_WC_REVISION "${output_var}")
#ENDIF(Subversion_SVN_EXECUTABLE)

ENDIF(UPDATE_SUBVERSION_REVISION)

SET(${PROJECT_NAME}_VERSION_MAJOR "0")
SET(${PROJECT_NAME}_VERSION_MINOR "5")
SET(${PROJECT_NAME}_VERSION_PATCH "5")
SET(${PROJECT_NAME}_VERSION_REVISION ${Project_WC_REVISION})
SET(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}.${${PROJECT_NAME}_VERSION_REVISION}")
MESSAGE("${PROJECT_NAME}_VERSION: " ${${PROJECT_NAME}_VERSION})


CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/src/gui/version.h.in" "${PROJECT_SOURCE_DIR}/src/gui/version.h")

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

IF(WIN32)
  SET(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
ENDIF(WIN32)

IF(UNIX)
  SET(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
ENDIF(UNIX)

IF (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF()

SET(CMAKE_DEBUG_POSTFIX "_d")

if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
endif()


FIND_PACKAGE(OGRE REQUIRED)
IF(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
  MESSAGE(SEND_ERROR "You need Ogre 1.7 Cthugha to build this.")
ENDIF()

IF (WIN32)
 FIND_PACKAGE(BoostOGRE REQUIRED)
 FIND_PACKAGE(GNUWIN REQUIRED)
ENDIF (WIN32)
FIND_PACKAGE(OIS REQUIRED)
FIND_PACKAGE(Lua51 REQUIRED)
FIND_PACKAGE(CEGUI REQUIRED)
FIND_PACKAGE(CEGUIOGRE REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(ALUT REQUIRED)
FIND_PACKAGE(Ogg REQUIRED)
FIND_PACKAGE(Vorbis REQUIRED)
FIND_PACKAGE(PhysFS REQUIRED)
IF(BUILD_TOOLS)
  FIND_PACKAGE(POCO REQUIRED)
ENDIF()
IF(NO_TINYXML)
  FIND_PACKAGE(TinyXML REQUIRED)
ENDIF()
IF(NO_ENET)
  FIND_PACKAGE(Enet REQUIRED)
ENDIF()


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/core)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui/contenteditor)
INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIRS})
IF (WIN32)
  Message(STATUS "Adding boost include: ${BoostOGRE_INCLUDE_DIR}")
  INCLUDE_DIRECTORIES(${BoostOGRE_INCLUDE_DIR})
  Message(STATUS "Adding gnu include: ${GNUWIN_INCLUDE_DIR}")
  INCLUDE_DIRECTORIES(${GNUWIN_INCLUDE_DIR})
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${OIS_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CEGUI_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CEGUIOGRE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ALUT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OGG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VORBIS_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PHYSFS_INCLUDE_DIR})
IF(NO_TINYXML)
  INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIR})
ELSE()
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/tinyxml)
ENDIF()
IF(NO_ENET)
  INCLUDE_DIRECTORIES(${ENET_INCLUDE_DIR})
ELSE()
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/enet/include)
ENDIF()
IF(BUILD_TOOLS)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui/debugpanel)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gui/debugpanel/filebrowser)
  INCLUDE_DIRECTORIES(${POCO_INCLUDE_DIR})
  MESSAGE("Activated ingame build tools.")
ENDIF()
IF(!WIN32)
  INCLUDE_DIRECTORIES(${PTHREADS_INCLUDE_DIR})
ENDIF()
IF(WIN32)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/dirent)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/boost)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/gettext/include/)
  ADD_DEFINITIONS( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4800 /wd4244 /wd4305 /nologo" )
  MESSAGE("WARNING: Compiler warnings 4800, 4244 and 4305 disabled!")
ENDIF()

if(UNIX)
  set(CFG_FILES_DIR "${CMAKE_INSTALL_PREFIX}/share/games/sumwars")
else()
  set(CFG_FILES_DIR ".")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/gui/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/config.h)

SET(HEADER_FILES
  ./src/gui/config.h
  ./src/core/nlfg.h
  ./src/core/worldobject.h
  ./src/core/itemloader.h
  ./src/core/itemlist.h
  ./src/core/region.h
  ./src/core/objectloader.h
  ./src/core/player.h
  ./src/core/monster.h
  ./src/core/itemfactory.h
  ./src/core/translatablestring.h
  ./src/core/event.h
  ./src/core/charconv.h
  ./src/core/stdstreamconv.h
  ./src/core/gameobject.h
  ./src/core/projectile.h
  ./src/core/treasure.h
  ./src/core/dialogue.h
  ./src/core/playerbase.h
  ./src/core/objectfactory.h
  ./src/core/gettext.h
  ./src/core/dropitem.h
  ./src/core/party.h
  ./src/core/minimap.h
  ./src/core/logger.h
  ./src/core/item.h
  ./src/core/networkstruct.h
  ./src/core/gridunit.h
  ./src/core/quest.h
  ./src/core/eventsystem.h
  ./src/core/creature.h
  ./src/core/random.h
  ./src/core/action.h
  ./src/core/creaturestruct.h
  ./src/core/servernetwork.h
  ./src/core/nlfgservernetwork.h
  ./src/core/nlfgclientnetwork.h
  ./src/core/nlfgnetworkpacket.h
  ./src/core/pathfind.h
  ./src/core/damage.h
  ./src/core/waypoint.h
  ./src/core/document.h
  ./src/core/debug.h
  ./src/core/unionfindtree.h
  ./src/core/world.h
  ./src/core/fixedbase.h
  ./src/core/fixedobject.h
  ./src/core/templateloader.h
  ./src/core/elementattrib.h
  ./src/core/worldloader.h
  ./src/core/geometry.h
  ./src/core/scriptobject.h
  ./src/core/spawnpoint.h
  ./src/core/fraction.h
  ./src/core/command.h
  ./src/core/mapgenerator.h
  ./src/core/options.h
  ./src/core/sumwarshelper.h
  ./src/gui/minimapwindow.h
  ./src/gui/worldmap.h
  ./src/gui/mainmenu.h
  ./src/gui/ogg.h
  ./src/gui/charcreate.h
  ./src/gui/ceguiutility.h
  ./src/gui/tooltipmanager.h
  ./src/gui/music.h
  ./src/gui/controlpanel.h
  ./src/gui/inventory.h
  ./src/gui/tradewindow.h
  ./src/gui/dialoguewindow.h
  ./src/gui/partyinfo.h
  ./src/gui/networkwindows.h
  ./src/gui/creditswindow.h
  ./src/gui/mainwindow.h
  ./src/gui/application.h
  ./src/gui/skilltree.h
  ./src/gui/chatline.h
  ./src/gui/charinfo.h
  ./src/gui/itemwindow.h
  ./src/gui/savegamelist.h
  ./src/gui/scene.h
  ./src/gui/graphicmanager.h
  ./src/gui/messageboxes.h
  ./src/gui/graphicobjectrenderinfo.h
  ./src/gui/graphicobject.h
  ./src/gui/questinfo.h
  ./src/gui/sound.h
  ./src/gui/tooltip.h
  ./src/gui/optionswindow.h
  ./src/gui/clipboard.h
)

IF(NO_TINYXML)
  SET(SRCS_TINYXML "")
ELSE()
  SET(SRCS_TINYXML
    ./src/tinyxml/tinystr.cpp
    ./src/tinyxml/tinyxmlerror.cpp
    ./src/tinyxml/tinyxml.cpp
    ./src/tinyxml/tinyxmlparser.cpp)
ENDIF()

IF(NO_ENET)
  SET(SRCS_ENET "")
ELSE()
  SET(SRCS_ENET
    ./src/enet/callbacks.c
    ./src/enet/compress.c
    ./src/enet/host.c
    ./src/enet/list.c
    ./src/enet/packet.c
    ./src/enet/peer.c
    ./src/enet/protocol.c
    ./src/enet/unix.c
    ./src/enet/win32.c
    )
ENDIF()

SET(SRCS
  ${SRCS_TINYXML}
  ${SRCS_ENET}
  ${HEADER_FILES}
  ./src/core/nlfg.cpp
  ./src/core/worldobject.cpp
  ./src/core/itemloader.cpp
  ./src/core/itemlist.cpp
  ./src/core/region.cpp
  ./src/core/objectloader.cpp
  ./src/core/player.cpp
  ./src/core/monsterbase.cpp
  ./src/core/monster.cpp
  ./src/core/itemfactory.cpp
  ./src/core/translatablestring.cpp
  ./src/core/event.cpp
  ./src/core/charconv.cpp
  ./src/core/stdstreamconv.cpp
  ./src/core/gameobject.cpp
  ./src/core/projectile.cpp
  ./src/core/treasure.cpp
  ./src/core/dialogue.cpp
  ./src/core/playerbase.cpp
  ./src/core/objectfactory.cpp
  ./src/core/gettext.cpp
  ./src/core/dropitem.cpp
  ./src/core/party.cpp
  ./src/core/minimap.cpp
  ./src/core/logger.cpp
  ./src/core/item.cpp
  ./src/core/networkstruct.cpp
  ./src/core/gridunit.cpp
  ./src/core/quest.cpp
  ./src/core/eventsystem.cpp
  ./src/core/creature.cpp
  ./src/core/random.cpp
  ./src/core/action.cpp
  ./src/core/creaturestruct.cpp
  ./src/core/servernetwork.cpp
  ./src/core/nlfgservernetwork.cpp
  ./src/core/nlfgclientnetwork.cpp
  ./src/core/nlfgnetworkpacket.cpp
  ./src/core/pathfind.cpp
  ./src/core/damage.cpp
  ./src/core/waypoint.cpp
  ./src/core/document.cpp
  ./src/core/debug.cpp
  ./src/core/unionfindtree.cpp
  ./src/core/world.cpp
  ./src/core/fixedbase.cpp
  ./src/core/fixedobject.cpp
  ./src/core/templateloader.cpp
  ./src/core/elementattrib.cpp
  ./src/core/worldloader.cpp
  ./src/core/geometry.cpp
  ./src/core/scriptobject.cpp
  ./src/core/spawnpoint.cpp
  ./src/core/fraction.cpp
  ./src/core/command.cpp
  ./src/core/mapgenerator.cpp
  ./src/core/options.cpp
  ./src/core/sumwarshelper.cpp
  ./src/gui/minimapwindow.cpp
  ./src/gui/worldmap.cpp
  ./src/gui/mainmenu.cpp
  ./src/gui/ogg.cpp
  ./src/gui/charcreate.cpp
  ./src/gui/ceguiutility.cpp
  ./src/gui/tooltipmanager.cpp
  ./src/gui/music.cpp
  ./src/gui/controlpanel.cpp
  ./src/gui/inventory.cpp
  ./src/gui/tradewindow.cpp
  ./src/gui/dialoguewindow.cpp
  ./src/gui/partyinfo.cpp
  ./src/gui/networkwindows.cpp
  ./src/gui/creditswindow.cpp
  ./src/gui/mainwindow.cpp
  ./src/gui/application.cpp
  ./src/gui/skilltree.cpp
  ./src/gui/chatline.cpp
  ./src/gui/charinfo.cpp
  ./src/gui/itemwindow.cpp
  ./src/gui/savegamelist.cpp
  ./src/gui/scene.cpp
  ./src/gui/graphicmanager.cpp
  ./src/gui/main_gui.cpp
  ./src/gui/messageboxes.cpp
  ./src/gui/graphicobjectrenderinfo.cpp
  ./src/gui/graphicobject.cpp
  ./src/gui/questinfo.cpp
  ./src/gui/sound.cpp
  ./src/gui/tooltip.cpp
  ./src/gui/optionswindow.cpp
  ./src/gui/clipboard.cpp
)
 
IF(BUILD_TOOLS)
  SET(HEADER_FILES
    ${HEADER_FILES}
    ./src/gui/debugpanel/benchmarktab.h
    ./src/gui/debugpanel/debugpanel.h
    ./src/gui/debugpanel/debugcameratab.h
    ./src/gui/debugpanel/iconeditortab.h
    ./src/gui/debugpanel/guidebugtab.h
    ./src/gui/debugpanel/luascripttab.h
    ./src/gui/debugpanel/textfileeditwindow.h
    ./src/gui/debugpanel/filebrowser/filebrowser.h
    #./src/gui/debugpanel/reloadtab.h
    ./src/gui/contenteditor/contenteditor.h
    ./src/gui/contenteditor/contenteditortab.h
    ./src/gui/contenteditor/fixedobjecteditor.h
    ./src/gui/contenteditor/renderinfoeditor.h
    ./src/gui/contenteditor/itemeditor.h
    ./src/gui/contenteditor/monstereditor.h
    )

  SET(SRCS
    ${SRCS}
    ${HEADER_FILES}
    ./src/gui/debugpanel/benchmarktab.cpp
    ./src/gui/debugpanel/debugpanel.cpp
    ./src/gui/debugpanel/debugcameratab.cpp
    ./src/gui/debugpanel/iconeditortab.cpp
    ./src/gui/debugpanel/guidebugtab.cpp
    ./src/gui/debugpanel/luascripttab.cpp
    ./src/gui/debugpanel/textfileeditwindow.cpp
    ./src/gui/debugpanel/filebrowser/filebrowser.cpp
    #./src/gui/debugpanel/reloadtab.cpp
    ./src/gui/contenteditor/contenteditor.cpp
    ./src/gui/contenteditor/contenteditortab.cpp
    ./src/gui/contenteditor/fixedobjecteditor.cpp
    ./src/gui/contenteditor/renderinfoeditor.cpp
    ./src/gui/contenteditor/itemeditor.cpp
    ./src/gui/contenteditor/monstereditor.cpp
    )
ENDIF()

# If building for windows based systems, the resource file (containing 
# the manifest, icon and other resources) should also be included
IF (WIN32)
  SET(SRCS 
    ${SRCS}
    ./src/winicon/Sumwars.rc
    )
  SET(HEADER_FILES
    ${HEADER_FILES}
    ./src/winicon/resource.h
    )
ENDIF (WIN32)

ADD_EXECUTABLE(sumwars ${SRCS})


#${CEGUILUA_LIBRARY}


SET(LINK_LIBS
  ${OGRE_LIBRARIES}
  ${OGRE_Terrain_LIBRARY}
  ${OIS_LIBRARIES} 
  ${CEGUI_LIBRARY} 
  ${CEGUIOGRE_LIBRARY}
  ${LUA_LIBRARIES} 
  ${OPENAL_LIBRARY} 
  ${ALUT_LIBRARY} 
  ${OGG_LIBRARY}
  ${VORBIS_LIBRARIES} 
  ${PHYSFS_LIBRARY}
  )
IF(NO_TINYXML)
  SET(LINK_LIBS ${LINK_LIBS} ${TINYXML_LIBRARIES})
ENDIF()
IF(NO_ENET)
  SET(LINK_LIBS ${LINK_LIBS} ${Enet_LIBRARY})
ENDIF()
IF(BUILD_TOOLS)
  SET(LINK_LIBS
    ${LINK_LIBS}
    ${POCO_LIBRARIES}
    )
  ADD_DEFINITIONS(-DBUILD_TOOLS)
ENDIF()
IF(WIN32)
  SET(LINK_LIBS
    ws2_32.lib
    winmm.lib
    ${LINK_LIBS}
    ${GNUWIN_LIBRARY}
    ${BoostOGRE_LIBRARIES}
    )
ENDIF()

TARGET_LINK_LIBRARIES(sumwars ${LINK_LIBS})

IF(POST_BUILD_COPY)
  ADD_CUSTOM_COMMAND(TARGET sumwars
    POST_BUILD
    COMMAND copy $(OutDir)$(TargetFileName) $(ProjectDir)$(TargetFileName)
  )
ENDIF(POST_BUILD_COPY)


# install the application
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/sumwars
  DESTINATION games)

install(FILES packaging/sumwars.desktop
  DESTINATION share/applications)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/SumWarsIcon_16x16.png"
  DESTINATION share/icons/hicolor/16x16/
  RENAME sumwars.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/SumWarsIcon_24x24.png"
  DESTINATION share/icons/hicolor/24x24/
  RENAME sumwars.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/SumWarsIcon_32x32.png"
  DESTINATION share/icons/hicolor/32x32/
  RENAME sumwars.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/SumWarsIcon_48x48.png"
  DESTINATION share/icons/hicolor/48x48/
  RENAME sumwars.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/SumWarsIcon_64x64.png"
  DESTINATION share/icons/hicolor/64x64/
  RENAME sumwars.png)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/SumWarsIcon_128x128.png"
  DESTINATION share/icons/hicolor/128x128/
  RENAME sumwars.png)

execute_process(COMMAND sed "s|INSTALL_PATH|${CFG_FILES_DIR}|g" "${CMAKE_CURRENT_SOURCE_DIR}/resources.cfg.in" OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources.cfg")
install(FILES authors.txt
  DESTINATION share/games/sumwars)
install(FILES resources.cfg ogre.cfg plugins.cfg
  DESTINATION share/games/sumwars)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_CURRENT_SOURCE_DIR}/resources"
  DESTINATION share/games/sumwars
  PATTERN .svn EXCLUDE
  PATTERN icon EXCLUDE)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/translation"
  DESTINATION share/games/sumwars
  FILES_MATCHING PATTERN "*.mo"
  PATTERN .svn EXCLUDE)
