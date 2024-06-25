# FFI for kdb+

FFI (foreign function interface) is a mechanism by which a program written in one programming language can call routines or make use of services written in another. Some programs may not know at the time of compilation what arguments are to be passed to a function. For instance, an interpreter may be told at run-time about the number and types of arguments used to call a given function. Libffi can be used in such programs to provide a bridge from the interpreter program to compiled code.

`ffikdb` is an extension to kdb+ for loading and calling dynamic libraries using pure `q`. 

The main purpose of the library is to build stable interfaces on top of external libraries, or to interact with the operating system from `q`. No compiler toolchain or writing C/C++ code is required to use this library.

You don't need to write C code, but you do need to know what you are doing. You can easily crash the kdb+ process or corrupt in-memory data structures with no hope of finding out what happened. For example, when q callback function is passed to foreign function and the q function failed, user might be able to see error message in console but as the foreign function cannot handle q error, the execution crashes and leads to entire application crash. Or if user passed valid but wrong tye characters to q callback, interna conversion failure of q cannot be handed and application crashes without any error message.

No support is offered for crashes caused by use of this library.

We are grateful to @abalkin for allowing us to adapt and expand on his original codebase. 

## Installation Documentation

:point_right: [`Install guide`](docs/install.md)

## API Documentation

:point_right: [`API reference`](docs/reference.md)

## Examples

:point_right: [`Example guide`](docs/examples.md)

## Status

The FFI interface is provided here under an Apache 2.0 license.

If you find issues with the interface or have feature requests please consider raising an issue [here](https://github.com/KxSystems/ffi/issues).

If you wish to contribute to this project please follow the contributing guide [here](https://github.com/KxSystems/ffi/blob/master/CONTRIBUTING.md).

## Unsupported Functionality

Foreign functions taking a struct do not work properly (can cause crash). For example, a function taking `K` pointer does not work.

