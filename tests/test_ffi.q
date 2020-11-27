\l ffi.q

// Windows is not supported by this test
if["w"=.ffi.os[]; show "Use test_win.q on Windows."; exit 1];

-1 "<< Testing printf Function >>";

.ffi.callFunction[`printf]("%s %d\n\000"; `test; 1; ::);

-1 "<< Testing sprintf Function >>";

x: 80#"\000";
n: .ffi.callFunction[("i"; `sprintf)] (x; "%s %f %hd\000"; "test\000"; 2f; 0h; ::);
show x til n;

-1 "<< Testing strlen Function >>";

6i=.ffi.callFunction[("i"; `strlen)] (`abcdef;::);
3i=.ffi.callFunction[("i"; `strlen)] ("abc\000";::);

strlen:.ffi.bind[`strlen; "C"; "i"];
show strlen ("123\000"; ::);
show strlen (`abc; ::);

-1 "<< Testing puts Function >>";

.ffi.callFunction[`puts]("test\000"; ::);
puts:.ffi.bind[`puts; "s"; "i"];
puts ("def\n\000"; ::);
puts (`abc; ::);

//show .ffi.call[.ffi.cif["Gi";"i"];`printf] ("Hello %d\n\000";42);

//register func on handle
valueLater:();

.z.ts:{
  if[0=count valueLater;show "Nothing left on timer";exit 0;];
  show ("Timer exec"; first valueLater);
  res:@[value;first valueLater;{show "Error executing timer - ",x}];
  valueLater _:0;
 };

r:{
  b:20#"\000";
  n:.ffi.callFunction[`read] (x; b; 20 ; ::);
  if[n=-1;show(`pipe_read_error; .ffi.errno[])];
  read::0N!n#b;
 };

pipe:0 0i;
res:.ffi.callFunction["i",`pipe] (pipe; ::);
if[res=-1; show (`pipe_write_error; .ffi.errno[])];
.ffi.callFunction[`sd1] (pipe[0]; (r; 1#"i"); ::); / start handler
.ffi.callFunction[`write] (pipe[1]; written; count written:"piping data"; ::);

/ stop handler. also closes pipe[0]
/ execute after all immediate tests finished
valueLater,: enlist ({.ffi.callFunction[`sd0] (pipe[0]; ::)}; 0);

// Callbacks
cmp:{0N!x,y;(x>y)-x<y}
x:3 1 2i
.ffi.callFunction[(" ";`qsort)] (x; `int$count x; 4i; (cmp;"II";"i"); ::);
x:2 2 -1i
.ffi.callFunction[(" ";`qsort)] (x; `int$count x; 4i; (cmp;"II";"i"); ::);
x:101b
.ffi.callFunction[(" ";`qsort)] (x; `int$count x; 1i; (cmp;"BB";"i"); ::);

x:`c`a`b;
// symbols are pointers - size of pointer is .ffi.ptrsize
.ffi.callFunction[(" ";`qsort)] (x; `int$count x; `int$.ffi.ptrsize[]; (cmp;"SS";"i"); ::);


.z.i=.ffi.callFunction[`getpid] (::;::);
// get parrent pid
show .ffi.callFunction[`getppid] (::;::);

// sleep for 10ms
10=10 xbar system "t .ffi.callFunction[`usleep] (10000i; ::)"

// decode errno
show .ffi.callFunction[`strerror_r] (60i; errs; count errs:256#" "; ::);

\t 100
 