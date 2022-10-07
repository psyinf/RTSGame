cmake_minimum_required(VERSION 3.5 FATAL_ERROR)


include(helpers)

find_package(Boost ${MIN_BOOST_VERSION} CONFIG REQUIRED)
find_package(Qt5 CONFIG REQUIRED Core Widgets)
find_package(OpenSceneGraph CONFIG REQUIRED)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_named_subdirectory(DIRECTORY GameCore FOLDER Core)
add_named_subdirectory(DIRECTORY GameApp FOLDER App)
add_named_subdirectory(DIRECTORY TerrainTest FOLDER App)