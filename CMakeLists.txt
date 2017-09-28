cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project (RTSGame CXX)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/CMakeModules/"
	"$ENV{ES_CMAKE_MODULES}/"
)


include(Setup)

IF (${CMAKE_CL_64})
	set(ARCH_EXT "_64")
ELSE()
	set(ARCH_EXT "")
ENDIF(${CMAKE_CL_64})

if (MSVC14)
	set(COMPILER_EXT "_vc140")
endif()



set(ENV_RENDER_DIR ES_RENDER_DEV${ARCH_EXT}${COMPILER_EXT})
set(ENV_SDK_DIR ES_SDK_DEV${ARCH_EXT}${COMPILER_EXT})
message (sf ${ARCH_EXT}${COMPILER_EXT})
include(Setup)

set(CMAKE_PREFIX_PATH 	$ENV{ES_3RDPARTY_DIR}/
						$ENV{${ENV_SDK_DIR}}/
						$ENV{${ENV_RENDER_DIR}}/
)


cmake_policy (SET CMP0020 NEW)
cmake_policy (SET CMP0022 NEW)
cmake_policy (SET CMP0043 NEW)

# use FindModule of CMake, have to provide some hints, can be removed when Boost-config.cmake exists
set(BOOST_ROOT "$ENV{ES_3RDPARTY_DIR}/boost")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/vc140/lib64")
set(Boost_USE_STATIC_LIBS ON)

message( STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message( STATUS "CMAKE_CL_64: ${CMAKE_CL_64}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "AutogenTargets")

# the flag and definition changes should really be set in the targets
set(ENH_INSTRUCTION_SET AVX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:${ENH_INSTRUCTION_SET} /D NOMINMAX /MP /Gm- /Ob2")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /DNDEBUG /arch:${ENH_INSTRUCTION_SET} /MP /Gm-")

add_definitions( "/wd4221" ) #no public symbols 
add_definitions( "/wd4355" ) #this in initializer list
add_definitions( "/wd4396" ) #the inline specifier cannot be used when a friend declaration refers to a specialization of a function template
add_definitions( "/wd4503" ) #decorated name length
add_definitions( "/wd4996" ) #security CRT warning


set(CMAKE_DEBUG_POSTFIX "_d")
set(VC_ABI "")

add_subdirectory(src)

