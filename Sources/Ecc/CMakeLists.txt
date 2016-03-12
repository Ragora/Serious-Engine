CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Ecc)

FILE(GLOB_RECURSE ECC_INCLUDES "*.h")
# no recursing, as we then also add CMAKE-cpp stuff which we don't want.
# this also would break the build as we'd have multiple main symbols.
FILE(GLOB ECC_SOURCES "*.cpp")

INCLUDE_DIRECTORIES ("../")
ADD_EXECUTABLE (Ecc ${ECC_SOURCES} ${ECC_INCLUDES})
