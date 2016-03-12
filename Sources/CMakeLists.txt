CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(SeriousEngine)

ADD_DEFINITIONS(-DPLATFORM_UNIX=1)
ADD_DEFINITIONS(-D_MT=1)

# Looks like the engine depends on these projects at some point.
# FIXME: For Windows, these are in Tools.Win32
FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

ADD_SUBDIRECTORY("Ecc")

# There are some things we currently can't build because of WinAPI dependencies
IF (WIN32)
  ADD_SUBDIRECTORY("WorldEditor")
  ADD_SUBDIRECTORY("SeriousSkaStudio")
  ADD_SUBDIRECTORY("EngineGui")
  ADD_SUBDIRECTORY("RCon")
  ADD_SUBDIRECTORY("GameGUIMP")
  ADD_SUBDIRECTORY("Modeler")
ENDIF(WIN32)

ADD_SUBDIRECTORY("Engine")

ADD_SUBDIRECTORY("Depend")
ADD_SUBDIRECTORY("GameMP")
ADD_SUBDIRECTORY("MakeFONT")
ADD_SUBDIRECTORY("DecodeReport")
ADD_SUBDIRECTORY("DedicatedServer")
ADD_SUBDIRECTORY("LWSkaExporter")
ADD_SUBDIRECTORY("Shaders")
ADD_SUBDIRECTORY("SeriousSam")
