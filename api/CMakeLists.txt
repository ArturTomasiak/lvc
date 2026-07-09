cmake_minimum_required(VERSION 3.20)

project(lvc
    VERSION 1.0.0
    DESCRIPTION "LVC C API implemented in C++"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE LVC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE LVC_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_library(lvc SHARED
    ${LVC_SOURCES}
    ${LVC_HEADERS}
)

set_target_properties(lvc PROPERTIES
    LINKER_LANGUAGE CXX
)

target_include_directories(lvc
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/internal"
)

target_compile_definitions(lvc
    PRIVATE
        LVC_MAKE_DLL
)

if(NOT WIN32)
    set_target_properties(lvc PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
    )
endif()

set_target_properties(lvc PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
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
endif()

set(LVCAPI_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/LVCAPI")

set_target_properties(lvc PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LVCAPI_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${LVCAPI_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${LVCAPI_OUTPUT_DIR}"
)

foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${CONFIG_TYPE}" CONFIG_TYPE_UPPER)

    set_target_properties(lvc PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${LVCAPI_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${LVCAPI_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} "${LVCAPI_OUTPUT_DIR}"
    )
endforeach()

set(LVC_PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/lvc.hpp")

add_custom_command(TARGET lvc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LVCAPI_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LVC_PUBLIC_HEADER}"
            "${LVCAPI_OUTPUT_DIR}/lvc.hpp"
    COMMENT "Copying lvc.hpp to LVCAPI output folder"
)

include(GNUInstallDirs)

install(TARGETS lvc
    EXPORT lvcTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT lvcTargets
    FILE lvcTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lvc
)