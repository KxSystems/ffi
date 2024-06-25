---
title: Using foreign functions with kdb+ – Interfaces – kdb+ and q documentation
description: FFI for kdb+ is an extension to kdb+ for loading and calling dynamic libraries using pure q.
hero: <i class="fab fa-superpowers"></i> Fusion for Kdb+
keywords: foreign, function, fusion, interface, kdb+, q
---

# ffikdb

FFI (foreign function interface) is a mechanism by which a program written in one programming language can call routines or make use of services written in another. Some programs may not know at the time of compilation what arguments are to be passed to a function. For instance, an interpreter may be told at run-time about the number and types of arguments used to call a given function. Libffi can be used in such programs to provide a bridge from the interpreter program to compiled code.

This interface is utilizing [libffi](https://sourceware.org/libffi/) which can be already installed in your OS except for some Linux OS (See pre-requisite in :fontawesome-brands-github: [KxSystems/ffikdb](https://github.com/KxSystems/ffi#requirements)). The documentation of libffi API can be found in its [repository](https://github.com/libffi/libffi/tree/master/doc).

## FFI/kdb+ Integration

ffikdb is an extension to kdb+ for loading and calling dynamic libraries using pure q. The main purpose of the library is to build stable interfaces on top of external libraries, or to interact with the operating system from `q`. No compiler toolchain or writing C/C++ code is required to use this library.

!!! warning "Know what you are doing"

    You don't need to write C code, but you do need to know what you are doing. You can easily crash the kdb+ process or corrupt in-memory data structures with no hope of finding out what happened. For example, when q callback function is passed to foreign function and the q function failed, user might be able to see error message in console but as the foreign function cannot handle q error, the execution crashes and leads to entire application crash. Or if user passed valid but wrong tye characters to q callback, interna conversion failure of q cannot be handed and application crashes without any error message.

    No support is offered for crashes caused by use of this library.

### Acknowledgement

We are grateful to Alexander Belopolsky for allowing us to adapt and expand on his original codebase. 
