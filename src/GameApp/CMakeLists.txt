set(APP_NAME GameApp)

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
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" 
			FILES ${HEADER_FILES} ${CPP_FILES}
			)

target_include_directories(${APP_NAME} PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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