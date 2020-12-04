# CMake version
cmake_minimum_required(VERSION 3.1)

# Set project name
project(ffikdb C)

# Set library name
set(MY_LIBRARY_NAME ffikdb)

# Set default compile flag as Debug
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# Sub directory to look into
add_subdirectory(src)

# Install package at release
install(FILES README.md LICENSE DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME} CONFIGURATIONS Release)
install(DIRECTORY examples DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME} CONFIGURATIONS Release)
install(DIRECTORY q DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME} CONFIGURATIONS Release)
