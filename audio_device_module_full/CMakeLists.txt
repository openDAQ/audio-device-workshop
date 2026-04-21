cmake_minimum_required(VERSION 3.25)
set(REPO_NAME audio_device_module)
set(REPO_OPTION_PREFIX AUDIO_DEVICE_MODULE)

add_subdirectory(cmake)

opendaq_read_file_contents("${CMAKE_CURRENT_LIST_DIR}/module_version" module_version)
opendaq_get_version_major_minor_patch("${module_version}" ${REPO_OPTION_PREFIX}_VERSION)

project(${REPO_NAME} VERSION ${${REPO_OPTION_PREFIX}_VERSION} LANGUAGES CXX)

# options
opendaq_setup_common_build_options()
opendaq_setup_project_specific_build_options(${REPO_OPTION_PREFIX})

if (NOT PROJECT_IS_TOP_LEVEL)
	message(FATAL_ERROR "Module needs to be the top-level project!" )
endif()

opendaq_common_compile_targets_settings()
opendaq_setup_compiler_flags(${REPO_OPTION_PREFIX})

find_package(${OPENDAQ_SDK_NAME} 3.31.0 GLOBAL)
if (NOT ${OPENDAQ_SDK_NAME}_FOUND)
	include(FetchContent)
	set(OPENDAQ_ENABLE_TESTS OFF CACHE BOOL "")

	opendaq_get_custom_fetch_content_params(${OPENDAQ_SDK_NAME} FC_PARAMS)

	FetchContent_Declare(
		${OPENDAQ_SDK_NAME}
		GIT_REPOSITORY https://github.com/openDAQ/openDAQ.git
		GIT_TAG 1f7e703f0cbb25c9b47866caef9328dc68744cea
		GIT_PROGRESS   ON
		${FC_PARAMS}
	)
	FetchContent_MakeAvailable(${OPENDAQ_SDK_NAME})
endif()

add_subdirectory(external)
add_subdirectory(src)