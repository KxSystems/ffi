
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was dlfcn-win32-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

####################################################################################

set_and_check(dlfcn-win32_INCLUDEDIR "${PACKAGE_PREFIX_DIR}/include")

if(NOT TARGET dlfcn-win32::dl)
  include(${CMAKE_CURRENT_LIST_DIR}/dlfcn-win32-targets.cmake)
endif()

set(dlfcn-win32_LIBRARIES dlfcn-win32::dl)
set(dlfcn-win32_INCLUDE_DIRS ${dlfcn-win32_INCLUDEDIR})
