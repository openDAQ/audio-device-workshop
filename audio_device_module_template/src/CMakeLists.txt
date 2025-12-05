set(LIB_NAME audio_device_module)
set(TARGET_FOLDER_NAME ${LIB_NAME})
set(MODULE_HEADERS_DIR ../include/${TARGET_FOLDER_NAME})

set(SRC_Include common.h
                module_dll.h
                audio_device_module_impl.h
                audio_device_impl.h
                audio_channel_impl.h
                miniaudio_utils.h
)

set(SRC_Srcs module_dll.cpp
             audio_device_module_impl.cpp
             audio_device_impl.cpp
             audio_channel_impl.cpp
             miniaudio_utils.cpp
)

prepend_include(${TARGET_FOLDER_NAME} SRC_Include)

source_group("common" FILES ${MODULE_HEADERS_DIR}/audio_device_module_common.h
                            ${MODULE_HEADERS_DIR}/audio_device_module_impl.h
                            ${MODULE_HEADERS_DIR}/miniaudio_utils.h
                            ${MODULE_HEADERS_DIR}/module_dll.h
                            audio_device_module_impl.cpp
                            miniaudio_utils.cpp
                            module_dll.cpp
)

source_group("audio_device" FILES ${MODULE_HEADERS_DIR}/audio_channel_impl.h
                                  ${MODULE_HEADERS_DIR}/audio_device_impl.h
                                  audio_device_impl.cpp
                                  audio_channel_impl.cpp
)


add_library(${LIB_NAME} SHARED ${SRC_Include}
                               ${SRC_Srcs}
)

add_library(${SDK_TARGET_NAMESPACE}::${LIB_NAME} ALIAS ${LIB_NAME})

if (MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /bigobj)
endif()

target_link_libraries(${LIB_NAME}
        PUBLIC
            daq::opendaq
        PRIVATE
            miniaudio::miniaudio
)

if (CMAKE_COMPILER_IS_GNUCXX AND MINGW)
    target_compile_options(${LIB_NAME} PRIVATE -Wno-error)
endif()

target_include_directories(${LIB_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                                              $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
                                              $<INSTALL_INTERFACE:include>
)

opendaq_set_module_properties(${LIB_NAME} ${PROJECT_VERSION_MAJOR})
create_version_header(${LIB_NAME})

add_executable(app main.cpp)
target_link_libraries(app PUBLIC daq::opendaq)
add_dependencies(app ${LIB_NAME})

target_compile_definitions(app PRIVATE AUDIO_MODULE_DIR="$<TARGET_FILE_DIR:app>")