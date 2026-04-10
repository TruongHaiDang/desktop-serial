cmake_minimum_required(VERSION 3.21)
project(desktop_serial VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

include(CheckIPOSupported)
set(EXEC_NAME desktop_serial)

add_subdirectory(ui)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
qt_add_executable(${EXEC_NAME} ${SOURCES} ${UI_SOURCES})
target_include_directories(${EXEC_NAME} PRIVATE inc)
target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ui)
target_link_libraries(${EXEC_NAME} PRIVATE Qt6::Core Qt6::Widgets)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${EXEC_NAME} PRIVATE -O3 -march=native)
    check_ipo_supported(RESULT result)
    if (result)
        set_property(TARGET ${EXEC_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${EXEC_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
