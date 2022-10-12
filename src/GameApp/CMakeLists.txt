set(APP_NAME GameApp)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB HEADER_FILES 	"*.h")
file(GLOB CPP_FILES 	"*.cpp")
file(GLOB UI_FILES 		"*.ui")

add_executable(${APP_NAME} 
	${HEADER_FILES}
	${CPP_FILES}
	${UI_FILES}
)


target_link_libraries(${APP_NAME}
	PRIVATE
		RTSGame::Core
		RTSGame::RenderCore
		Boost::program_options
		OPENSCENEGRAPH::osgWidget
		Qt5::Core
		Qt5::Widgets
	INTERFACE
		nlohmann_json::nlohmann_json
)

target_include_directories(GameCore
	INTERFACE
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_INSTALL_PREFIX}/${LIB_NAME}>
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" 
			FILES ${HEADER_FILES} ${CPP_FILES}
			)


set_target_properties(${APP_NAME} PROPERTIES
	AUTOMOC	ON
	AUTOUIC	ON
	AUTORCC	ON
)


		
set_target_properties(${APP_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(${APP_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX ${CMAKE_RELWITHDEBINFO_POSTFIX})




install(TARGETS ${APP_NAME}
        CONFIGURATIONS Debug Release RelWithDebInfo 
        RUNTIME DESTINATION bin)


set_target_properties(
    ${APP_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin")