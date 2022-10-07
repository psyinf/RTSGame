file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
file(GLOB_RECURSE cpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(GameCore STATIC
	${header_files}			${cpp_files}
)
add_library(RTSGame::Core ALIAS GameCore)

target_link_libraries(GameCore
	PUBLIC
		Boost::boost
		Boost::headers
		OPENSCENEGRAPH::osg
		OPENSCENEGRAPH::osgTerrain
)

target_include_directories(GameCore INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)
