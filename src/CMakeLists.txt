file(GLOB_RECURSE SOURCES "./*.cpp") # Read source files with a .cpp extension.
file(GLOB_RECURSE HEADERS "../inc/*.h") # Read header files with a .h extension.

qt_add_executable(${EXEC_NAME} ${SOURCES} ${HEADERS}) # Add executable file follow the Qt6 standard
target_link_libraries(${EXEC_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
    "${EXEC_NAME}_UI"
)
