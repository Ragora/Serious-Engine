CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(SeriousEngine)

ADD_DEFINITIONS(-DPLATFORM_UNIX=1)
ADD_DEFINITIONS(-D_MT=1)

IF(WIN32)
    SET (CMAKE_BUILD_TYPE None CACHE STRING "Set build type to None for windows") # Keep it blank for windows build?
ELSE(WIN32)
    IF(NOT CMAKE_BUILD_TYPE)
	SET (CMAKE_BUILD_TYPE Debug CACHE STRING 
	    "Choose the type of build, options are : None Debug Release RelWithDebInfo MinSizeRel." 
	    FORCE)
    ENDIF(NOT CMAKE_BUILD_TYPE)
ENDIF(WIN32)

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # Use clang/clang++

    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_FLAGS                "-Wall -std=c++11")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")

    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # USE_CLANG) # Use gcc/g++

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

ENDIF()

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
