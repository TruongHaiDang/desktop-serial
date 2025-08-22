cmake_minimum_required(VERSION 3.21)
project("Desktop Serial" VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

set(AUTOUIC ON)
set(AUTORCC ON)
set(AUTOMOC ON)

set(EXEC_NAME "desktop-serial")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

include_directories(include)
include_directories(ui)

add_subdirectory(src)
add_subdirectory(ui)
