\l ffi.q
.ffi.cf[`puts]("test\000";::)
.ffi.cf[`printf]("%s %d\n\000";`test;1)
x: 80#"\000"
n: .ffi.cf[`sprintf](x;"%s %f %hd\000";"test\000"; 2f; 0h)
x til n

6i=.ffi.cf[`strlen](`abcdef;::)
3i=.ffi.cf[`strlen]("abc\000";::) 

puts_cif:.ffi.cif[(),"s";"i"]
.ffi.call[puts_cif;`puts](`abc;::)
.ffi.call[puts_cif;`puts]("def\n\000";::)
.ffi.call[.ffi.cif["si";"i"];`printf]("Hello %d\n\000";42)

//register func on handle
r:{b:20#"\000";n:cf[`read](x;b;20);if[n=-1;show(`pipe_read_error;.ffi.errno[])];read::0N!n#b;0}
pipe:0 0i;
res:.ffi.cf[`pipe](pipe;(::))
if[res=-1;show (`pipe_write_error;.ffi.errno[])];
.ffi.cf[`sd1](pipe[0];(r;(),"i")) / start handler
.ffi.cf[`write](pipe[1];written;count written:"piping data")
//.ffi.cf[`sd0](pipe[0];::)          / stop handler. also closes pipe[0]

// Callbacks
cmp:{0N!x,y;(x>y)-x<y} 
x:3 1 2i
.ffi.cf[(" ";`qsort)]0N!(x;count x;4;(cmp;"II")) 

x:`c`a`b;
.ffi.cf[(" ";`qsort)](x;count x;8;(cmp;"SS")) 

.z.i=.ffi.cf[`getpid]()
// get parrent pid
.ffi.cf[`getppid]()

// sleep for 10ms
10=10 xbar system"t .ffi.cf[`usleep](10000i;::)"

// decode errno
.ffi.cf[`strerror_r](60i;errs;count errs:256#" ")


