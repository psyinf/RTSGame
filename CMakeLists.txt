cmake_minimum_required(VERSION 2.8)
project (GameTest)
SET(ES_3RD_PARTY_DIR  "NOT FOUND")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{ES_CMAKE_MODULES})
include(Setup)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2 /D NOMINMAX")
#HACK: there is something wrong with the osgDB linkage
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:Multiple")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:Multiple")
#HACK: there is something wrong with the osgDB linkage
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:Multiple")

#for simple relative paths in sub elements
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_definitions( "/wd4503" ) 
add_definitions( "/wd4996" )  #unsafe getenv etc.



SET(CMAKE_DEBUG_POSTFIX "_d")

add_subdirectory(src/GameCore)
add_subdirectory(src/GameApp)
#Install resources
INSTALL(DIRECTORY data DESTINATION bin)


SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)



