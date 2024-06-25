# ffikdb installation instructions

## Requirements

- Operating system: Linux, macOS 10.12+, Windows 7+
- kdb+ v3.5 or higher
- [dlfcn-win32](https://github.com/dlfcn-win32/dlfcn-win32) (for Windows)
- libffi 3.1+ as per instructions. RHEL/CentOS 6/7 provided libffi 3.0.5 can be used as well.

## Installation

### Pre-built Binary

Download the appropriate release archive from [releases](../../releases/latest) page. 

Then unpack and install by following command according to your platform type.

environment     | action
----------------|---------------------------------------------------------------------------------------
Linux           | `tar xzvf ffi_linux-v*.tar.gz -C $QHOME --strip 1`
macOS           | `tar xzvf ffi_osx-v*.tar.gz -C $QHOME --strip 1`
Windows         | Open the archive and copy content of the `ffi` folder (`ffi\*`) to `%QHOME%` or `c:\q`


### Building and installing from source

#### Third-Party Library Installation

ffikdb uses `libdl` and `libffi` to utilize foreign functions. However, as Windows does not have POSIX-like `libdl` user needs to install `dlfcn-win32` in advance. As for `libffi` OS had it by default before but it seems `libffi` package also needs to be installed for recent machines.

#### Windows

##### 1. Install `dlfcn-win32`

The library built in this step will be used to link to final ffikdb library. To do so you need to set `DL_INSTALL_DIR` to the install directory of `dlfcn-win32`.

```bat

> git clone https://github.com/dlfcn-win32/dlfcn-win32.git
> cd dlfcn-win32
dlfcn-win32> mkdir build
dlfcn-win32> mkdir install
dlfcn-win32> set DL_INSTALL_DIR=%cd%\install
dlfcn-win32> cd build
build> cmake --config Release -DCMAKE_INSTALL_PREFIX=..\install .. -DBUILD_TESTS:BOOL=OFF
build> cmake --build . --config Release --target install

```

In order to make the built dll library visibe to kdb+, you need to create a symlink in `QHOME/w64` to the dll library.

```bat

build> cd %QHOME%\w64
w64> MKLINK dl.dll %DL_INSTALL_DIR%\bin\dl.dll

```

##### 2. Install `libffi`

This installation requires seudo Linux terminal to execute shell script. We used MSYS2. For installation of MSYS2, please see [this page](https://www.msys2.org/). After installing MSYS2, install gcc, git and autoconf.

```bash

$ pacman -Syu base-devel gcc vim cmake make texinfo libtool autoconf automake-wrapper

```

You will be asked to close msys2 once. Then close and execute following line.

```bash

$ pacman -Su

```

Now gcc was installed. In order to enable gcc, user might need to create PATH to proper directory. Add following ine to the bottm of `.bashrc` and restart MSYS2.

```bash

$ export PATH=/mingw64/bin:$PATH

```

Then clone the source of `libffi` and build it.

**Note:** If `aclocal is not found` is returned at the `autogen.sh`, libraries `libtool`, `autoconf`, `automake-wrapper` might need to be installed again.

```bash

$ git clone https://github.com/libffi/libffi.git
$ cd libffi
$ ./autogen.sh
libffi]$ mkdir build
libffi]$ mkdir install
libffi]$ cd build
build]$ ../configure --prefix=$(pwd)/../install --disable-docs --enable-static=yes --enable-shared=yes -build=x86_64-w64-mingw32
build]$ make install

```

Now `libffi` package was built. Go to the directory where `dlfcn-win32` was cloned.

1. Create a new directory `libffi` and copy the libraries into `install/` under the new `libffi` directory.
2. Set `FFI_INSTALL_DIR` to the copied install directory.

```bat

> mkdir libffi && cd libffi
libffi> mkdir install
libffi> xcopy /s /i C:\msys64\home\[username]\[some path]\libffi\install\ install
libffi> set FFI_INSTALL_DIR=%cd%\install

```

Create lib from dll fie.

```bat

libffi> cd install\bin
bin> echo EXPORTS > libffi.def
bin> for /f "usebackq tokens=4,* delims=_ " %i in (`dumpbin /exports "libffi-8.dll"`) do if %i==ffi echo %i_%j >> libffi.def
bin> lib /def:"libffi.def" /out:"libffi.lib" /machine:x64

```

Finally, you need to create a symlink in `QHOME\w64` to dll library.

```bat

bin> cd %QHOME%\w64
w64> MKLINK libffi.dll %FFI_INSTALL_DIR%\bin\libffi-8.dll

```

#### Linux

For linux `libffi` needs to be installed.

environment                    | installation
-------------------------------|----------------------------------------------------------
Ubuntu Linux with 64-bit kdb+  | `sudo apt-get install libffi-dev`
RHEL/CentOS  with 64-bit kdb+  | `sudo yum install libffi-devel`

In order to make `libffi` visible to kdb+, you need to add the path to `libffi.so` to `LD_LIBRARY_PATH`:. It depends on your environment but example is below:

```bash

$ export LD_LIBRARY_PATH=/usr/lib64:${LD_LIBRARY_PATH}

```

#### MacOS

For MacOS, if user machine is version<=10.13, `libffi` should be in `/usr/lib` and no action is required. You can proceed to the [next step](#Buildingffikdb).

If user machine is version>=10.14 `libffi` needs to be installed via `brew` command:

```bash

$ `brew install libffi`

```

Then set `FFI_INSTALL_DIR` to the installation directory where `include` and `lib` exist. To make `libffi` visible to kdb+, you need to add the path to `DYLD_LIBRARY_PATH`:

```bash

$ export DYLD_LIBRARY_PATH=${FFI_INSTALL_DIR}/lib:${DYLD_LIBRARY_PATH}

```

#### Building ffikdb

*Note: Compiling interface from source uses cmake. The default build type is `Release`. User should set `-DCMAKE_BUILD_TYPE=Debug` following `cmake` for debugging build, i.e.,*

```bash

$ cmake --config Debug ..

```

*Note: `cmake --build . --target install` installs shared object and q files under `q/` into `QHOME/[os]64` and `QHOME` directory respectively. If you have a preference of where to place shared object and q files, you can execute `cmake --build .` instead of `cmake --build . --target install`. Then buit package can be found under `build/ffikdb/`.*

#### Linux/MacOSX

```bash

ffi]$ mkdir build && cd build
build]$ cmake ..
build]$ cmake --build . --target install

```

#### Windows

```bat

ffi>mkdir build && cd build
build>cmake --config Release ..
build>cmake --build . --config Release --target install

```
