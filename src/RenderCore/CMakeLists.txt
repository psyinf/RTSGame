set (LIB_NAME RenderCore)
file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
file(GLOB_RECURSE cpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${LIB_NAME} STATIC
	${header_files}			${cpp_files}
)
add_library(RTSGame::${LIB_NAME} ALIAS ${LIB_NAME})

target_link_libraries(${LIB_NAME}
	PUBLIC
		OPENSCENEGRAPH::osg
		OPENSCENEGRAPH::osgTerrain
)

target_include_directories(${LIB_NAME} INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)
