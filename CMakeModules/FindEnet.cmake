# - Locate Enet
# This module defines
# Enet_LIBRARY
# Enet_FOUND, if false, do not try to link to OpenAL
# Enet_INCLUDE_DIR, where to find the headers
#
# Created by Nicholas Cosens, based on the FineALUT.cmake module by Bryan Donlan
 
FIND_PATH(Enet_INCLUDE_DIR enet.h
  /usr/local/include/enet
  /usr/local/include
  /usr/include/enet
  /usr/include
  /sw/include/enet # Fink
  /sw/include
  /opt/local/include/enet # DarwinPorts
  /opt/local/include
  /opt/csw/include/enet # Blastwave
  /opt/csw/include
  /opt/include/enet
  /opt/include
  )


FIND_LIBRARY(Enet_LIBRARY
  NAMES enet
  PATHS
  /usr/local/lib
  /usr/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  )
 
SET(Enet_FOUND "NO")
IF(Enet_LIBRARY)
  SET(Enet_FOUND "YES")
ENDIF(Enet_LIBRARY)
