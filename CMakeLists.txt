cmake_minimum_required(VERSION 3.25) # Specify the minimum version of CMake required.
project(DesktopSerial VERSION 1.0.0 LANGUAGES C CXX) # Specify the languages are used in this project.

set(CMAKE_CXX_STANDARD 17) # C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Require standard
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -O2")

set(CMAKE_AUTORCC ON) # Auto processing qrc file
set(CMAKE_AUTOUIC ON) # Auto processing ui file
set(CMAKE_AUTOMOC ON) # Auto processing macro

set(EXEC_NAME "DesktopSerial")

# Find Qt6 library and import 3 basic components. Stop if not found.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

include_directories(inc)
include_directories(ui)
add_subdirectory(src)
add_subdirectory(ui)
