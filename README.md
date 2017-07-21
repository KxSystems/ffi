# Foreign Function Interface for Q 

Examples
```
q)\l ffi.q / populates .ffi namespace
q).ffi.cf[`strlen](`abc;::) / arguments should be passed as generic lists
3
q).ffi.cf[`strlen]("abc\000";::) / null-terminate q-strings
3
q)n:.ffi.cf[`printf]("%s %c %hd %d %ld %g\n\000";`test;"x";1h;2;3j;4f)
test x 1 2 3 4
q)b:n#"\000" / buffer
q)n:.ffi.cf[`sprintf](b;"%s %c %hd %d %ld %g\n\000";`test;"x";1h;2;3j;4f)
q)b
"test x 1 2 3 4\n"

q).ffi.cf[("h";`getppid)]() / specify return type, no args
13615h
q).ffi.cf[("e";`libm.so`powf)]2 2e,(::) / explicit library
4e
```
BLAS - all arguments should be lists
```
q)x:10#2f;.ffi.cf[("f";`libblas.so`ddot_)](list count x; x;list 1;x;list 1)
40f
q).ffi.cf[(" ";`libblas.so`daxpy_)](list count x;list 2f; x;list 1;x;list 1)
q)x / <- a*x+y, a=x=y=2
6 6 6 6 6 6 6 6 6 6f
```
Callbacks
```
q)cmp:{0N!x,y;(x>y)-x<y} 
q)x:3 1 2;.ffi.cf[(" ";`qsort)](x;3;4;(cmp;"II")) 
1 2
3 1
3 2
q)x
1 2 3
q)x:`c`a`b;.ffi.cf[(" ";`qsort)](x;3;8;(cmp;"SS")) 
`a`b
`c`a
`c`b
q)x
`a`b`c
```
Register a callback on a handle
```
r:{b:20#"\000";n:.ffi.cf[`read](x;b;20);0N!n#b;0}
cf[`sd1](h;(r;(),"i")) / start handler
cf[`sd0](h;::)          / stop handler
```
