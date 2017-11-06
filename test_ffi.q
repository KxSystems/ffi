\l ffi.q
if["w"=.ffi.os;show "Use test_win.q on Windows.";exit 1;];
.ffi.cf[`puts]("test\000";::)
.ffi.cf[`printf]("%s %d\n\000";`test;1)
x: 80#"\000"
n: .ffi.cf[("i";`sprintf)]0N!(x;"%s %f %hd\000";"test\000"; 2f; 0h)
x til n

6i=.ffi.cf[("i";`strlen)]0N!(`abcdef;::)
3i=.ffi.cf[("i";`strlen)]0N!("abc\000";::)

puts:.ffi.bind[`puts;"s";"i"]
puts 0N!("def\n\000";::)
puts 0N!(`abc;::)

.ffi.call[.ffi.cif["Gi";"i"];`printf]0N!("Hello %d\n\000";42)



//register func on handle
valueLater:()
.z.ts:{
  if[0=count valueLater;show "Nothing left on timer";exit 0;];
  show ("Timer exec";first valueLater);
  res:@[value;first valueLater;{show "Error executing timer - ",x}];
  valueLater _:0;
  }
r:{
  b:20#"\000";
  n:.ffi.cf[`read](x;b;20);
  if[n=-1;show(`pipe_read_error;.ffi.errno[])];
  read::0N!n#b;
  }
pipe:0 0i;
res:.ffi.cf["i",`pipe](pipe;(::))
if[res=-1;show (`pipe_write_error;.ffi.errno[])];
.ffi.cf[`sd1](pipe[0];(r;1#"i")) / start handler
.ffi.cf[`write](pipe[1];written;count written:"piping data")

/ stop handler. also closes pipe[0]
/ execute after all immediate tests finished
valueLater,: enlist ({.ffi.cf[`sd0](pipe[0];::)};0)          

// Callbacks
cmp:{0N!x,y;(x>y)-x<y}
x:3 1 2i
.ffi.cf[(" ";`qsort)]0N!(x;`int$count x;4i;(cmp;"II";"i"))
x:2 2 -1i
.ffi.cf[(" ";`qsort)]0N!(x;`int$count x;4i;(cmp;"II";"i"))
x:101b
.ffi.cf[(" ";`qsort)]0N!(x;`int$count x;1i;(cmp;"BB";"i"))

x:`c`a`b;
// symbols are pointers - size of pointer is .ffi.nil length
.ffi.cf[(" ";`qsort)]0N!(x;`int$count x;`int$.ffi.ptrsize;(cmp;"SS";"i")) 
.z.i=.ffi.cf[`getpid]()
// get parrent pid
.ffi.cf[`getppid]()

// sleep for 10ms
10=10 xbar system"t .ffi.cf[`usleep](10000i;::)"

// decode errno
.ffi.cf[`strerror_r](60i;errs;count errs:256#" ")

\t 100
