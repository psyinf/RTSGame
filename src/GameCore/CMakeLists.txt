file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
file(GLOB_RECURSE cpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
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
	PRIVATE
		nlohmann_json::nlohmann_json
		yaml-cpp::yaml-cpp
		magic_enum::magic_enum
)

target_include_directories(GameCore
	INTERFACE
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_INSTALL_PREFIX}/${LIB_NAME}>
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)