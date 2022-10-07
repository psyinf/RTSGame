cmake_minimum_required(VERSION 3.21)#
project(RTSGame VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_POSTFIX _d)
set(CMAKE_RELWITHDEBINFO_POSTFIX _rd)

set(PROJECT_NAMESPACE "RTSGame")			# namespace for target aliases in this project
set(PROJECT_EXPORT_NAME "RTSGame")			# export name to be used by install() and downstream find_package()
set(PROJECT_INSTALL_PREFIX "RTSGame")		# folder 'prefix' for installed includes and libs

# Run conan install automatically, or display warning if conan is not installed
include(cmake/conan_config.cmake)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(cmake/helpers.cmake)

install(EXPORT ${PROJECT_EXPORT_NAME} 
	FILE
		${PROJECT_EXPORT_NAME}Targets.cmake
	NAMESPACE
		${PROJECT_NAMESPACE}::
	DESTINATION
		${CMAKE_INSTALL_ARCHIVEDIR}/cmake/${PROJECT_EXPORT_NAME}
	COMPONENT
		${DEVELOP_COMPONENT}
)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/cmake/"
)


add_subdirectory(src)

add_custom_target( conan-install ALL COMMAND conan install ${CMAKE_SOURCE_DIR}/conanfile.py -if ${CMAKE_INSTALL_PREFIX} -s build_type=$<CONFIG> DEPENDS install)       

#for now
install(DIRECTORY
		data/
	DESTINATION
		${CMAKE_INSTALL_BINDIR}/data
	COMPONENT
		${RUNTIME_COMPONENT}
)


