CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_DEFINITIONS(-DPLATFORM_UNIX=1)
ADD_DEFINITIONS(-D_MT=1)

OPTION(USE_CLANG "Build application with clang" OFF) # OFF is the default
OPTION(USE_LTO   "Use clangs link time optimization for Release builds" OFF) # OFF is the default

IF(WIN32)
    SET (CMAKE_BUILD_TYPE None CACHE STRING "Set build type to None for windows") # Keep it blank for windows build?
ELSE(WIN32)
    IF(NOT CMAKE_BUILD_TYPE)
	SET (CMAKE_BUILD_TYPE Debug CACHE STRING 
	    "Choose the type of build, options are : None Debug Release RelWithDebInfo MinSizeRel." 
	    FORCE)
    ENDIF(NOT CMAKE_BUILD_TYPE)
ENDIF(WIN32)

IF(USE_CLANG) # Use clang/clang++

    SET (CMAKE_C_COMPILER             "/usr/bin/clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    
    IF(USE_LTO)
    	SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    ELSE(USE_LTO)
        SET (CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG")
    ENDIF(USE_LTO)

    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
    SET (CMAKE_CXX_FLAGS                "-Wall -std=c++11")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

ELSE(USE_CLANG) # Use gcc/g++

    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_FLAGS                "-Wall -std=c++11")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

ENDIF(USE_CLANG)

PROJECT(SeriousEngine)

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
