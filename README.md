# Foreign Function Interface for kdb+

`ffiq` is an extension to kdb+ for loading and calling dynamic libraries using pure `q`. 

Main purpose of the library is to build stable interfaces on top of external libraries or interact with operating system from `q`. No compiler toolchain or writing C/C++ code should be required when using this library.

***DANGER*** Although you don't need to write C code, you do need to know what you are doing. You can easily crash kdb+ process or corrupt in-memory data structures without any hope to find out what has happened. You would not get any support for crashes caused by use of this library.

## Requirements
 - Linux, macOS 10.10+, Windows 7+
 - libffi 3.1+(see installation instructions below)
 - Latest build of kdb+ 3.4 or higher

### Installation
#### Dependencies

*Ubuntu Linux* 
```
sudo apt-get install libffi-dev
sudo apt-get install libffi-dev:i386  # to install 32bit version on 64bit OS
```

*macOS*
```
brew install libffi     # at the time of writing is libffi 3.2.1
```

*Windows*

No steps required.

#### Library

Download appropriate release archive from [releases](/releases/latest) page(`tar.gz` for Linux/macOS or `.zip` for Windows).
Unpack content of the archive to `QHOME`(`c:\q` on Windows or `~/q` on Linux/macOS as a default).

```bash
# on macOS/Linux to extract direct to $QHOME
tar xzvf ffi_osx-v0.9.2.tar.gz -C $QHOME --strip 1
```

On Windows, open archive and copy contents of `ffi` folder(`ffi\*`) from archive to `%QHOME%` or `c:\q`.


## API

ffiq exposes 2 main functions in `.ffi` namespace. See `test_ffi.q` for detailed examples of usage.

### Passing data and getting back results

Throughout the library characters are used to encode types of data provided and expected as a result. These are based on `c` column for [primitive data types](http://code.kx.com/q/ref/datatypes/#primitive-datatypes) and corresponding upper case for vectors of the same type. `sz` column is useful to workout what type can hold enough data to/from C.

Argument types are derived from data passed to function(in case of `cf`) or have to be explicitly specified(in case of `bind`). Number of character types provided must match number of arguments expected by C function.
Return type is specified as single character and can be `' '`(space) which means to discard result(`void`). If not provided defaults to `int`.

| char          | C types       |
| ------------- | ------------- |
| b, c, x       |  usigned int8 |
| h             | signed int16  |
| i             |signed int32   |
| j             | signed int64  |
| e             | float         |
| f             | double        |
| g, s          | uint8*        |
| ' '(space)    | void (only as return type) |
| r             | raw pointer   |
| k             | K object      |
| uppercase letter| pointer to the same type|

It is possible to pass a q function to C code as a callback(see qsort example below). Function must be presented as a mixed list `(func;argument_types;return_type)`, where func is a q function(type `100h`), argument_types is a char array with types function expects and char corresponding to return type of the function. Note, that as callbacks potentially have unbounded life in C code, they are not deleted after function completes.

### `cf` - call function

Simple function call. Intended for one-off calls. Takes 2 arguments:

1. function name(symbol) or list of return type char and function name.
2. mixed list of arguments. Types of arguments passed to function are inferred from q types and should match width of arguments C function expects. If arguments are not mixed list, append `(::)` at the end.


### `bind` - create projection with function resolved to call with arguments

Prepare q function and bind it to provided C function for future calls. Useful for multiple calls to C lib. Takes 3 arguments:
1. function name. Simple symbol or list of 2 symbols specifying library name.
2. char array of argument types
3. char with return type


Some additional utility functions are provided as well:

`cif` - prepare argument and return types to be used in `call`

`call` - call function using prepared types by `cif` with arguments provided

`errno` - return current `errno` global on \*nix OS

`kfn` - bind function which returns and accepts K objects in current process. Similar to `2:`

Arguments to call with should be passed as generic list to `cf`, `call` and function created by `bind`.

`cf` and `call` performs native function loading and resolution at the time of call which created significant overhead. Use `bind` to perform this steps in advance and reduce runtime overhead.


## Examples
```
q)\l ffi.q / populates .ffi namespace
q).ffi.cf[`strlen](`abc;::) / arguments should be passed as generic lists
3i
q).ffi.cf[`strlen]("abc\000";::) / null-terminate q-strings
3i
q)n:.ffi.cf[`printf]("%s %c %hd %d %ld %g\n\000";`test;"x";1h;2;3j;4f)
test x 1 2 3 4
q)b:n#"\000" / buffer
q)n:.ffi.cf[`sprintf](b;"%s %c %hd %d %ld %g\n\000";`test;"x";1h;2;3j;4f)
q)b
"test x 1 2 3 4\n"

q).ffi.cf[("h";`getppid)]() / specify return type, no args
13615h
// only Linux
q).ffi.cf[("e";`libm.so`powf)]2 2e,(::) / explicit library
4e
```
### BLAS 
All arguments should be vectors(i.e. pointers to appropriate type).
```
q)x:10#2f;
q).ffi.cf[("f";`libblas.so`ddot_)](1#count x; x;1#1;x;1#1)
40f
q).ffi.cf[(" ";`libblas.so`daxpy_)](1#count x;1#2f; x;1#1;x;1#1)
q)x / <- a*x+y, a=x=y=2
6 6 6 6 6 6 6 6 6 6f
```
### Callbacks
```
q)cmp:{0N!x,y;(x>y)-x<y} 
q)x:3 1 2i;.ffi.cf[(" ";`qsort)](x;3i;4i;(cmp;"II";"i")) 
1 2
3 1
3 2
q)x
1 2 3i
q)x:`c`a`b;.ffi.cf[(" ";`qsort)](x;3i;8i;(cmp;"SS";"i")) 
`a`b
`c`a
`c`b
q)x
`a`b`c
```
Register a callback on a handle
```
// h is handle to some other process
r:{b:20#"\000";n:.ffi.cf[`read](x;b;20);0N!n#b;0}
.ffi.cf[`sd1](h;(r;(),"i"))   / start handler
.ffi.cf[`sd0](h;::)           / stop handler
```


- - - - - - - - -
### Notes
This version is based on original [ffiq](https://github.com/enlnt/ffiq) created by @abalkin