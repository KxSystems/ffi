---
title: Function reference | FFI | Interfaces | Documentation for kdb+ and q
keywords: ffi, api, fusion, interface, q
hero: <i class="fab fa-superpowers"></i> Fusion for Kdb+
---

# ffikdb function reference

The following functions are exposed in `ffi` namespace.

<pre markdown="1" class="language-text">
.ffi **FFI interface**

Call Function
    [bind](#ffibind)                    Create a projection with the function resolved to call with arguments.
    [callFunction](#fficallfunction)    A simple one-off function call.
Utility
    [cvar](#fficvar)                    Read global variable from the library.
    [extension](#ffiextension)          Return file extension of a shared object for a platform.
    [os](#ffios)                        Return a letter denoting OS.
    [ptrsize](#ptrsize)                 Return a size of `void*`.
    [setErrno](#ffiseterrno)            Set `errno`.
</pre>

## Call Function

### `.ffi.bind`

_Creates a projection with the function resolved to call with arguments._

Syntax: `bind[funcname;argtypes;returntype]`

Where

- `funcname`:
  *  symbol: Name of function to look for.
  *  symbol list: list of shared object and a function name in the library
- `argtypes`: type characters of arguments.
- `returntype`: type character of returned value

Returns a q function, bound to the specifed C function for future calls. Useful for multiple calls to the C library.


### `.ffi.callFunction`

_A simple function call, intended for one-off calls._

Syntax: `callFunction[returntype_and_func;args]`

Where

- `returntype_and_func`:
 * symbol: Name of function
 * tuple of (q type character of returned value; symbol function name or list of shared object name and function name)
- `args`: List of arguments to be passed to the foreign function. This list must have (::) at the end.

calls a function which has the given name with the arguments. 


!!! warning "Function lookup"

    `callFunction` performs function lookup on each call and has significant overhead. For hot-path functions use `bind`.

## Utility

### `.ffi.cvar`

_Read global variable from the library._

Syntax: `.ffi.cvar[rtype_and_var]`

Where

- rtype_and_var:
  * symbol: Variable name
  * tuple of (char; symbol): (type character of returned value; variable name or list of shared object name and variable name)

In the second example it is assumed that `libsampleparser.so` is exposing a global variable `CHUNK_SIZE_THRESHOLD`, i.e. the result of `nm` command is like below:

```bash

$ nm -D l64/libsampleparser.so | grep CHUNK
0000000000054a10 D CHUNK_SIZE_THRESHOLD

```

Then examples are:

```q

q).ffi.cvar`timezone
-32400i
q).ffi.cvar ("j"; `libsampleparser.so`CHUNK_SIZE_THRESHOLD)
25000

```

### `.ffi.extension`

_Return file extension of a shared object for a platform._

```q

q).ffi.extension[]
`so
q)` sv `libRmath, .ffi.extension[]
`libRmath.so

```

### `.ffi.os`

_Return a letter denoting OS._

```q

q).ffi.os[]
"l"

```

### `.ffi.ptrsize`

_Return a size of `void*`._

```q

q).ffi.ptrsize[]
8i

```

### `.ffi.setErrno`

_Get or set `errno`._

Syntax:

- `.ffi.setErrno[n]`: set `n` as `errno`.
- `.ffi.setErrno[]`: get current `errno`.

Where

- `n` is a new `errno` value. This value must be int.

```q

q)// No such file or directory
q).ffi.setErrno[]
2i
q)// No such process
q).ffi.setErrno[3i]
2i
q).ffi.setErrno[]
3i

```

## Passing data and getting results

Throughout the library, characters are used to encode the types of data provided and expected as a result. These are based on the `c` column of [primitive data types](../basics/datatypes.md#primitive-datatypes) and the corresponding upper case for vectors of the same type. The `sz` column is useful to work out what type can hold enough data passing to/from C.

The argument types are derived from data passed to the function (in case of `callFunction`) or explicitly specified (in case of `bind`). The number of character types provided must match the number of arguments expected by the C function.

The return type is specified as a single character and can be `" "` (space), which means to discard the result (i.e. `void`). If not provided, defaults to `int`.

char             | C type                                         | FFI type
-----------------| -----------------------------------------------|------------------------------------
b, c, x          | unsigned int8                                  | ffi_type_uint8
h                | signed int16                                   | ffi_type_sint16
i, m, d, u, v, t | signed int32                                   | ffi_type_sint32
j, p, n          | signed int64                                   | ffi_type_sint64
e                | float                                          | ffi_type_float
f, z             | double                                         | ffi_type_double
g, s             | uint8*                                         | ffi_type_pointer
`" "` (space)    | void (only as return type)                     | ffi_type_void
l                | size of pointer (size_t)                       | ffi_type_sint32 or ffi_type_sint64
k                | callback function/closure (only argument type) | ffi_type_pointer
uppercase letter | pointer to the same type                       | ffi_type_pointer

It is possible to pass a q function to C code as a callback (see `qsort` example below). In that case argument type should be specified as `"k"`. The function must be presented as a mixed list `(func; argument_types; return_type)`, where `func` is a q function (type `100h`), `argument_types` is a char array with the types the function expects, and `return_type` is a char corresponding to the return type of the function. Note that, as callbacks potentially have unbounded life in C code, they are not deleted after the function completes.
