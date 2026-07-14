cmake_minimum_required(VERSION 3.20)

project(lvc
    VERSION 1.0.0
    DESCRIPTION "Literally Version Control"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# LVCAPI folder is expected to be at ../api/LVCAPI.
# Can be overridden with: cmake -S App -B build-app -DLVCAPI=/path/to/LVCAPI

set(LVCAPI
    "${CMAKE_CURRENT_LIST_DIR}/../api/LVCAPI"
    CACHE PATH "Path to the built LVCAPI output directory"
)

set(LVCAPI_INCLUDE_DIR "${LVCAPI}")

set(LVC_APP_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/bin")

if(WIN32)
    find_file(LVC_DLL
        NAMES lvc.dll
        PATHS "${LVCAPI}"
        NO_DEFAULT_PATH
        REQUIRED
    )

    find_library(LVC_IMPLIB
        NAMES lvc
        PATHS "${LVCAPI}"
        NO_DEFAULT_PATH
        REQUIRED
    )
elseif(UNIX)
    find_library(LVC_SHARED_LIBRARY
        NAMES lvc liblvc
        PATHS "${LVCAPI}"
        NO_DEFAULT_PATH
        REQUIRED
    )
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

file(GLOB_RECURSE LVC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(lvc
    ${LVC_SOURCES}
)

set_target_properties(lvc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${LVC_APP_OUTPUT_DIR}"
)

foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${CONFIG_TYPE}" CONFIG_TYPE_UPPER)

    set_target_properties(lvc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${LVC_APP_OUTPUT_DIR}"
    )
endforeach()

target_include_directories(lvc
    PRIVATE
        "${LVCAPI_INCLUDE_DIR}"
)

target_compile_options(lvc PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        /WX
        /O2
    >

    $<$<CXX_COMPILER_ID:GNU>:
        -Werror
        -O3
    >

    $<$<CXX_COMPILER_ID:Clang>:
        -Werror
        -O3
    >

    $<$<CXX_COMPILER_ID:AppleClang>:
        -Werror
        -O3
    >

    $<$<CXX_COMPILER_ID:IntelLLVM>:
        -Werror
        -O3
    >
)

if(WIN32)
    target_compile_definitions(lvc PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )

    target_link_libraries(lvc PRIVATE
        "${LVC_IMPLIB}"
    )

    add_custom_command(TARGET lvc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${LVC_APP_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${LVC_DLL}"
                "${LVC_APP_OUTPUT_DIR}/lvc.dll"
        COMMENT "Copying lvc.dll to app bin folder"
    )
elseif(UNIX)
    target_link_libraries(lvc PRIVATE
        "${LVC_SHARED_LIBRARY}"
    )

    get_filename_component(LVC_SHARED_LIBRARY_FILENAME
        "${LVC_SHARED_LIBRARY}"
        NAME
    )

    set_target_properties(lvc PROPERTIES
        BUILD_RPATH "${LVCAPI}"
        INSTALL_RPATH "$ORIGIN"
    )

    add_custom_command(TARGET lvc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${LVC_APP_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${LVC_SHARED_LIBRARY}"
                "${LVC_APP_OUTPUT_DIR}/${LVC_SHARED_LIBRARY_FILENAME}"
        COMMENT "Copying lvc shared library to app bin folder"
    )
endif()

install(TARGETS lvc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE
)
