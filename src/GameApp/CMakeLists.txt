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


install(TARGETS ${APP_NAME}
        CONFIGURATIONS Debug Release RelWithDebInfo 
        RUNTIME DESTINATION bin)