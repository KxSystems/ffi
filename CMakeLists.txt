# CMake version
cmake_minimum_required(VERSION 3.1)

# Set project name
project(ffikdb C)

# Set library name
set(MY_LIBRARY_NAME ffikdb)

# Default option is Release
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Sub directory to look into
add_subdirectory(src)

# Build package at release
file(COPY README.md LICENSE DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME})
file(COPY examples/ DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/examples/)
file(COPY q/ DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/q/)

# Copy q files to QHOME
install(DIRECTORY q/ DESTINATION $ENV{QHOME}/ CONFIGURATIONS Release)