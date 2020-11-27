#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "dlfcn-win32::dl" for configuration "Release"
set_property(TARGET dlfcn-win32::dl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(dlfcn-win32::dl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "psapi"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/dl.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS dlfcn-win32::dl )
list(APPEND _IMPORT_CHECK_FILES_FOR_dlfcn-win32::dl "${_IMPORT_PREFIX}/lib/dl.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
