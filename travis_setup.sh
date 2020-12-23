#!/bin/bash

# Download dlfcn_win
mkdir cbuild
wget https://github.com/dlfcn-win32/dlfcn-win32/archive/v1.2.0.tar.gz
tar xvf v1.2.0.tar.gz -C ./cbuild --strip-components=1

# Build and install to cbuild/install
cd cbuild
mkdir install
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" --config Release -DCMAKE_INSTALL_PREFIX=../install .. -DBUILD_TESTS:BOOL=OFF
cmake --build . --config Release --target install

cd ../../;
