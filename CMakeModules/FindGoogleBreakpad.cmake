# -*- cmake -*-
	
# - Find Google BreakPad
# Find the Google BreakPad includes and library
# This module defines
#  BREAKPAD_EXCEPTION_HANDLER_INCLUDE_DIR, where to find exception_handler.h, etc.
#  BREAKPAD_EXCEPTION_HANDLER_LIBRARIES, the libraries needed to use Google BreakPad.
#  BREAKPAD_EXCEPTION_HANDLER_FOUND, If false, do not try to use Google BreakPad.
# also defined, but not for general use are
#  BREAKPAD_EXCEPTION_HANDLER_LIBRARY, where to find the Google BreakPad library.
	
FIND_PATH(BREAKPAD_INCLUDE_DIR
        src/google_breakpad/common/breakpad_types.h
        PATHS
        $ENV{BREAKPAD_ROOT}
)

IF(NOT GoogleBreakpad_FIND_COMPONENTS)
        SET(GoogleBreakpad_FIND_COMPONENTS common exception_handler crash_generation_client crash_report_sender)
ENDIF(NOT GoogleBreakpad_FIND_COMPONENTS)

IF(CMAKE_TRACE)
        MESSAGE(STATUS "BREAKPAD_ROOT=${BREAKPAD_ROOT}")
        MESSAGE(STATUS "BREAKPAD_INCLUDE_DIR=${BREAKPAD_INCLUDE_DIR}")
ENDIF(CMAKE_TRACE)

IF(BREAKPAD_INCLUDE_DIR)
        SET(BREAKPAD_FOUND TRUE)
        FOREACH(COMPONENT ${GoogleBreakpad_FIND_COMPONENTS})
                string(TOUPPER ${COMPONENT} UPPERCOMPONENT)
                FIND_LIBRARY(BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE
                        NAMES ${COMPONENT}
                        PATHS
                                ${BREAKPAD_ROOT}/src/client/windows/Release/lib
                                ${BREAKPAD_INCLUDE_DIR}/src/client/windows/Release/lib
                        )
                FIND_LIBRARY(BREAKPAD_${UPPERCOMPONENT}_LIBRARY_DEBUG
                        NAMES ${COMPONENT}
                        PATHS
                                ${BREAKPAD_ROOT}/src/client/windows/Debug/lib
                                ${BREAKPAD_INCLUDE_DIR}/src/client/windows/Debug/lib
                        )
                IF(BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE AND BREAKPAD_${UPPERCOMPONENT}_LIBRARY_DEBUG)
                        SET(BREAKPAD_${UPPERCOMPONENT}_FOUND TRUE)
                        SET(BREAKPAD_${UPPERCOMPONENT}_LIBRARY optimized ${BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE} debug ${BREAKPAD_${UPPERCOMPONENT}_LIBRARY_DEBUG})
                        set(BREAKPAD_${UPPERCOMPONENT}_LIBRARY ${BREAKPAD_${UPPERCOMPONENT}_LIBRARY} CACHE FILEPATH "The breakpad ${UPPERCOMPONENT} library")
                ELSE(BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE AND BREAKPAD_${UPPERCOMPONENT}_LIBRARY_DEBUG)
                        SET(BREAKPAD_FOUND FALSE)
                        SET(BREAKPAD_${UPPERCOMPONENT}_FOUND FALSE)
                        SET(BREAKPAD_${UPPERCOMPONENT}_LIBRARY "${BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE-NOTFOUND}")
                ENDIF(BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE AND BREAKPAD_${UPPERCOMPONENT}_LIBRARY_DEBUG)
                IF(CMAKE_TRACE)
                        MESSAGE(STATUS "Looking for ${UPPERCOMPONENT}")
                        MESSAGE(STATUS "BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE=${BREAKPAD_${UPPERCOMPONENT}_LIBRARY_RELEASE}")
                        MESSAGE(STATUS "BREAKPAD_INCLUDE_DIR=${BREAKPAD_INCLUDE_DIR}")
                ENDIF(CMAKE_TRACE)
        ENDFOREACH(COMPONENT)
ENDIF(BREAKPAD_INCLUDE_DIR)
IF(BREAKPAD_FOUND)
        IF(CMAKE_TRACE)
                MESSAGE(STATUS "Looking for dump-symbols in: ${BREAKPAD_INCLUDE_DIR}/tools/windows/binaries" )
        ENDIF(CMAKE_TRACE)
        FIND_PROGRAM(BREAKPAD_DUMPSYMS_EXE
                dump_syms.exe NAMES dump_syms dumpsyms
                PATHS
                        ENV
                        PATH
                        ${BREAKPAD_ROOT}/tools/windows/binaries
                        ${BREAKPAD_INCLUDE_DIR}/tools/windows/binaries
                )
        IF(CMAKE_TRACE)
                MESSAGE(STATUS "Looking for dump-symbols in: ${BREAKPAD_INCLUDE_DIR}/tools/windows/binaries: ${BREAKPAD_DUMPSYMS_EXE}" )
        ENDIF(CMAKE_TRACE)
        IF(BREAKPAD_DUMPSYMS_EXE)
                SET(BREAKPAD_DUMPSYMS_EXE_FOUND TRUE)
        ELSE(BREAKPAD_DUMPSYMS_EXE)
                SET(BREAKPAD_DUMPSYMS_EXE_FOUND FALSE)
                SET(BREAKPAD_FOUND FALSE)
        ENDIF(BREAKPAD_DUMPSYMS_EXE)
ENDIF(BREAKPAD_FOUND) 