##%% Compilation %%##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

# OS Specific
IF(APPLE)
    set(OSFLAG m)
elseif(WIN32)
    set(OSFLAG w)
else()
    set(OSFLAG l)
endif()

# Set bitness
set(BITNESS 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITNESS 64)
endif()

# Default option is Release
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Specify target shared library
add_library(${MY_LIBRARY_NAME} SHARED
    ffi.c
)

# Specify include directories
target_include_directories(${MY_LIBRARY_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

if(APPLE)
    # FFI header exists under /usr/include/ffi for ver<=10.13
    target_include_directories(${MY_LIBRARY_NAME} PRIVATE
        $<$<STREQUAL:"$ENV{FFI_INSTALL_DIR}","">: /usr/include/ffi>
        $<$<NOT:$<STREQUAL:"$ENV{FFI_INSTALL_DIR}","">>: $ENV{FFI_INSTALL_DIR}/include>
    )
elseif(WIN32)
    # Windows use installed dlfcn-win32 and installed libffi
    target_include_directories(${MY_LIBRARY_NAME} PRIVATE
        $ENV{DL_INSTALL_DIR}/include/
        $ENV{FFI_INSTALL_DIR}/include/
    )
else()
    #Linux Nothing to do
endif()

# Download k.h
file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/c/c/k.h" "${PROJECT_SOURCE_DIR}/include/k.h")

# Link 
if(WIN32)
    # Search dl library
    find_library(DL_LIBRARY
        REQUIRED
        NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}dl
        HINTS $ENV{DL_INSTALL_DIR}/lib
    )
    # Search ffi library
    find_library(FFI_LIBRARY
        REQUIRED
        NAMES libffi
        HINTS $ENV{FFI_INSTALL_DIR}/bin
    )
    # Get q dependent library
    file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/${OSFLAG}${BITNESS}/q.lib" "${CMAKE_BINARY_DIR}/q.lib")
    # Link dependent libararies
    target_link_libraries(${MY_LIBRARY_NAME} PRIVATE
        "${CMAKE_BINARY_DIR}/q.lib"
        ${DL_LIBRARY}
        ${FFI_LIBRARY}
    )
else()
    if(APPLE)
        # Search ffi library from user install directory or /usr/lib by default (For OSX ver<=10.13)
        if(DEFINED FFI_INSTALL_DIR)
            find_library(FFI_LIBRARY
                NAMES libffi
                HINTS $ENV{FFI_INSTALL_DIR}/lib
            )
        endif()
        # Link flag
        set_target_properties(${MY_LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    # Link dependent libararies
    target_link_libraries(${MY_LIBRARY_NAME} PRIVATE
        libdl.so
        libffi.so
    )
endif()

# Compile option
target_compile_options(${MY_LIBRARY_NAME} PRIVATE
    # kdb+ version
    -DKXVER=3 -std=c99

    # Compiler Option
    $<$<PLATFORM_ID:Windows>: ${CMAKE_C_FLAGS} /W3 /D WIN32_LEAN_AND_MEAN /D>
    $<$<PLATFORM_ID:Darwin>: ${CMAKE_C_FLAGS} -DSYSFFI -fPIC -Wno-strict-aliasing>
    $<$<PLATFORM_ID:Linux>: ${CMAKE_C_FLAGS} -fPIC -D_GNU_SOURCE -Wno-strict-aliasing>

    # Config option
    $<$<CONFIG:Release>: -O3 -DNDEBUG>
    $<$<CONFIG:Debug>: -O0 -g>

)

# Shared library prefix and suffix
# ex.) ffikdb.so for linux
# Suffix must be `.so` for Mac
if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()

set_target_properties(${MY_LIBRARY_NAME} PROPERTIES SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES PREFIX "")

##%% Installation %%##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

# Copy built shared object after build instead of during installation
add_custom_command(TARGET ${MY_LIBRARY_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${MY_LIBRARY_NAME}>" ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/lib/${MY_LIBRARY_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    DEPENDS ${MY_LIBRARY_NAME}
)

# Install package at release and copy shared object to QHOME
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION $ENV{QHOME}/${OSFLAG}${BITNESS}/ CONFIGURATIONS Release)
