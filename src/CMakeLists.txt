##%% Compilation %%##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

# Specify target shared library
add_library(${MY_LIBRARY_NAME} SHARED
    ffi.c
)

# Search dependency
set(libdl.so_DIR $ENV{LD_LIBRARY_PATH})
set(libffi.so_DIR $ENV{LD_LIBRARY_PATH})

# Download k.h
file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/c/c/k.h" "${PROJECT_SOURCE_DIR}/include/k.h")

# Dependency for platform
if(MSVC)
    file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/w64/q.lib" "${CMAKE_BINARY_DIR}/q.lib")
    target_link_libraries(${MY_LIBRARY_NAME} PRIVATE
        "${CMAKE_BINARY_DIR}/q.lib"
        "libdl.so"
        "libffi.so"
    )
else()
    target_link_libraries(${MY_LIBRARY_NAME} PRIVATE
        "libdl.so"
        "libffi.so"
    )
endif()

# Specify include directory
target_include_directories(${MY_LIBRARY_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Compile option
target_compile_options(${MY_LIBRARY_NAME} PRIVATE
    # kdb+ version
    -DKXVER=3

    # Compiler Option
    $<$<C_COMPILER_ID:Windows>: ${CMAKE_C_FLAGS} /W3 /D WIN32_LEAN_AND_MEAN /D _CRT_SECURE_NO_WARNINGS>
    $<$<C_COMPILER_ID:MacOSX>: ${CMAKE_C_FLAGS} -DSYSFFI -fPIC -Wno-strict-aliasing>
    $<$<C_COMPILER_ID:Linux>: ${CMAKE_C_FLAGS} -fPIC -D_GNU_SOURCE -Wno-strict-aliasing>

    # Config option
    $<$<CONFIG:Release>: -O3 -DNDEBUG>
    $<$<CONFIG:Debug>: -O0 -g>

)

##%% Installation %%##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

# OS Specific
IF(APPLE)
    set_target_properties(${MY_LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    set_target_properties(${MY_LIBRARY_NAME} PROPERTIES SUFFIX .so)
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

# Install release version
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION "${CMAKE_BINARY_DIR}/${OSFLAG}${BITNESS}/")

# Install package at release and copy shared object to QHOME
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION "$ENV{QHOME}/${OSFLAG}${BITNESS}/" CONFIGURATIONS Release)
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION "${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/lib/" CONFIGURATIONS Release)
