cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project (RTSGame CXX)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/CMakeModules/"
	"$ENV{ES_CMAKE_MODULES}/"
)
include(Setup)

SDK_add_prefix_paths(USED_ENVIRONMENTS SDK RENDER)
SDK_setup_defaults()
SDK_Setup_boost()




# the flag and definition changes should really be set in the targets
set(ENH_INSTRUCTION_SET AVX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:${ENH_INSTRUCTION_SET} /D NOMINMAX /MP /Gm- /Ob2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /DNDEBUG /arch:${ENH_INSTRUCTION_SET} /MP /Gm-")

add_subdirectory(src)

