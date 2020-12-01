\l q/ffi.q

// Windows is not supported by this test
if["w"=.ffi.os[]; show "Use test_win.q on Windows."; exit 1];

// Use Standard C Library //-----------------------------------/

-1 "<< Testing printf Function >>";

.ffi.callFunction[`printf]("%s %d\n\000"; `test; 1; ::);

-1 "<< Testing sprintf Function >>";

// Holder of message
x: 80#"\000";
n:.ffi.callFunction[("i"; `sprintf)] (x; "%s %f %hd\000"; "test\000"; 2f; 0h; ::);
"test 2.000000 0\000" ~ x til n

-1 "<< Testing strlen Function >>";

6i=.ffi.callFunction[("i"; `strlen)] (`abcdef; ::)
3i=.ffi.callFunction[("i"; `strlen)] ("abc\000"; ::)

strlen:.ffi.bind[`strlen; "C"; "i"];
3=strlen ("123\000"; ::)
3=strlen (`abc; ::)

-1 "<< Testing puts Function >>";

.ffi.callFunction[`puts]("test\000"; ::);
puts:.ffi.bind[`puts; "s"; "i"];
puts ("def\n\000"; ::);
puts (`abc; ::);

-1 "<< Testing qsort Function >>";

// Callbacks
cmp:{0N!x,y;(x>y)-x<y};

x:3 1 2i;
.ffi.callFunction[(" "; `qsort)] (x; `int$count x; 4i; (cmp;"II";"i"); ::);
x~1 2 3i

x:2 2 -1i;
.ffi.callFunction[(" "; `qsort)] (x; `int$count x; 4i; (cmp;"II";"i"); ::);
x~-1 2 2i

x:101b;
.ffi.callFunction[(" "; `qsort)] (x; `int$count x; 1i; (cmp;"BB";"i"); ::);
x~011b

x:`c`a`b;
// symbols are pointers - size of pointer is .ffi.ptrsize
.ffi.callFunction[(" "; `qsort)] (x; `int$count x; .ffi.ptrsize[]; (cmp;"SS";"i"); ::);
x~`a`b`c

-1 "<< Testing getpid Function >>";

// get parrent pid
.z.i=.ffi.callFunction[`getpid] (::;::)

-1 "<< Testing usleep Function >>";

// sleep for 10ms
10=10 xbar system "t .ffi.callFunction[`usleep] (10000i; ::)"

-1 "<< Testing strerror_r Function >>";

// decode errno
//.ffi.callFunction[`strerror_r] (60i; errs; count errs:256#"\000"; ::);
//errs
strerror_r:.ffi.bind[`strerror_r; "iCj"; "s"];
errs:strerror_r (60i; errs; count errs:256#" "; ::);
errs = `$"Device not a stream"


// Use C API //------------------------------------------------/

//register func on handle
valueLater:();

.z.ts:{
  if[0=count valueLater;show "Nothing left on timer"; exit 0];
  show ("Timer exec"; first valueLater);
  res:@[value;first valueLater;{show "Error executing timer - ",x}];
  valueLater _:0;
 };

read_to_buffer:{
  b:20#"\000";
  n:.ffi.callFunction[`read] (x; b; 20 ; ::);
  if[n=-1; show (`pipe_read_error; .ffi.setErrno[])];
  0N!n#b;
 };

pipe:0 0i;
res:.ffi.callFunction[("i"; `pipe)] (pipe; ::);
if[res=-1; show (`pipe_write_error; .ffi.setErrno[])];
.ffi.callFunction[`sd1] (pipe[0]; (read_to_buffer; enlist "i"); ::); / start handler
.ffi.callFunction[`write] (pipe[1]; written; count written:"piping data"; ::);

// stop handler. also closes pipe[0]
// execute after all immediate tests finished
valueLater,: enlist ({.ffi.callFunction[`sd0] (pipe[0]; ::)}; 0);

// Start timer to execute `valueLater calls`
\t 100
 