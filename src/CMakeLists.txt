cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project (RTSGame CXX)
include(helpers)


find_package(Boost REQUIRED atomic chrono date_time program_options serialization signals thread timer)
find_package(SDK REQUIRED)
find_package(Qt5 REQUIRED Core Widgets)
find_package(OSG REQUIRED)
find_package(RENDER REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_named_subdirectory(GameCore Core)
add_named_subdirectory(GameApp App)