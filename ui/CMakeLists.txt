configure_file(
    "${CMAKE_SOURCE_DIR}/ui/configure.h.in"
    "${CMAKE_SOURCE_DIR}/ui/configure.h"
)

file(GLOB_RECURSE UI_FILES "./*ui") # Read all ui files


