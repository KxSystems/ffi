# Foreign Function Interface for Q 

`ffiq` is an extension to kdb+ for loading and calling dynamic libraries using pure `q`. It can be used to create bindings to other libraries or utilise OS functionality without writing any C code.

***DANGER*** Although you don't need to write C code, you do need to know what you are doing. You can easily crash kdb+ process or corrupt in-memory data structures without any hope to find out what has happened. You would not get any support for crashes caused by use of this library.

## Requirements
 - Linux, macOS
 - libffi (see installation instructions below)
 - Latest kdb+

### Installation
On Ubuntu 
```
sudo apt-get install libffi-dev
sudo apt install libffi-dev:i386 # to install 32bit version on 64bit OS
```
On masOS
```
brew install libffi     // at the time of writing is libffi 3.2.1
```
Optional: To install MKL go ahead to https://software.intel.com/en-us/mkl
```
/opt/intel/mkl/bin/mklvars.sh intel64
```

On Windows
Use NuGet(https://www.nuget.org/packages/libffi/)
```
nuget restore
wget https://github.com/KxSystems/kdb/raw/master/w64/q.lib -O w64/q.lib
wget https://github.com/KxSystems/kdb/raw/master/w32/q.lib -O w32/q.lib
```

## API

ffiq exposes several functions in `.ffi` namespace

`cf` - call function

`cif` - prepare argument and return types to be used in `call`

`call` - call function using prepared types by `cif` with arguments provided

`bind` - create projection with function resolved to call with arguments

`errno` - return current `errno` global on *nix OS

`kfn` - bind function which returns and accepts K objects in current process. Similar to `2:`

Arguments to to call with should be passed as generic list to `cf`, `call` and function created by `bind`.

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
### BLAS - all arguments should be lists
```
q)x:10#2f;
q).ffi.cf[("f";`libblas.so`ddot_)]((),count x; x;(),1;x;(),1)
40f
q).ffi.cf[(" ";`libblas.so`daxpy_)]((),count x;(),2f; x;(),1;x;(),1)
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
cf[`sd1](h;(r;(),"i")) / start handler
cf[`sd0](h;::)          / stop handler
```


- - - - - - - - -
### Notes
This version is based on original [ffiq](https://github.com/enlnt/ffiq) created by @abalkin