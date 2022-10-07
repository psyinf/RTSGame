function(add_named_subdirectory)
	set(options)
	set(oneValueArgs DIRECTORY TARGET FOLDER)
	set(multiValueArgs)
	cmake_parse_arguments(add_named_subdirectory
		"${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	add_subdirectory(${add_named_subdirectory_DIRECTORY})
	if ("${add_named_subdirectory_TARGET}" STREQUAL "")
		#guess target
		#message("setting up: " ${add_named_subdirectory_DIRECTORY})
		set(add_named_subdirectory_TARGET ${add_named_subdirectory_DIRECTORY})
	endif()
	set_target_properties(${add_named_subdirectory_TARGET} PROPERTIES FOLDER ${add_named_subdirectory_FOLDER})
endfunction()