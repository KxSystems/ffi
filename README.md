# FFI for kdb+

`ffikdb` is an extension to kdb+ for loading and calling dynamic libraries using pure `q`. 
It is part of the [_Fusion for kdb+_](https://code.kx.com/q/interfaces/fusion/) interface collection.

The main purpose of the library is to build stable interfaces on top of external libraries, or to interact with the operating system from `q`. No compiler toolchain or writing C/C++ code is required to use this library.

We are grateful to @abalkin for allowing us to adapt and expand on his original codebase. 

Please [report issues](https://github.com/KxSystems/ffi/issues) in this repository.


## Requirements

- Operating system: Linux, macOS 10.12+, Windows 7+
- kdb+ v3.5 or higher
- [dlfcn-win32](https://github.com/dlfcn-win32/dlfcn-win32) (for Windows)
- libffi 3.1+ as per instructions. RHEL/CentOS 6/7 provided libffi 3.0.5 can be used as well.

environment                    | installation
-------------------------------|----------------------------------------------------------
Ubuntu Linux with 64-bit kdb+  | `sudo apt-get install libffi-dev`
Ubuntu Linux with 32-bit kdb+  | `sudo apt-get install libffi-dev:i386`
RHEL/CentOS  with 64-bit kdb+  | `sudo yum install libffi-devel`
RHEL/CentOS  with 32-bit kdb+  | `sudo yum install glibc-devel.i686 libgcc.i686 libffi-devel.i686`
macOS                          | (no action required)
Windows                        | (no action required)


## Installation

### Pre-built Binary

Download the appropriate release archive from [releases](../../releases/latest) page. 

Then unpack and install by following command according to your platform type.

environment     | action
----------------|---------------------------------------------------------------------------------------
Linux           | `tar xzvf ffi_linux-v*.tar.gz -C $QHOME --strip 1`
macOS           | `tar xzvf ffi_osx-v*.tar.gz -C $QHOME --strip 1`
Windows         | Open the archive and copy content of the `ffi` folder (`ffi\*`) to `%QHOME%` or `c:\q`


### Build from Source

Clome this repository and exeute following commands.

```bash

$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ make install

```

## Documentation

See [code.kx.com/q/interfaces/ffi](https://code.kx.com/q/interfaces/ffi/) for documentation.

## Status

The FFI interface is provided here under an Apache 2.0 license.

If you find issues with the interface or have feature requests please consider raising an issue [here](https://github.com/KxSystems/ffi/issues).

If you wish to contribute to this project please follow the contributing guide [here](https://github.com/KxSystems/ffi/blob/master/CONTRIBUTING.md).

## Unsupported Functionality

Foreign functions taking a struct do not work properly (can cause crash). For example, a function taking `K` pointer does not work.