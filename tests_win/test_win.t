\l q/ffi.q

// Use Standard C Library //-----------------------------------/

-1 "<< Testing sprintf Function >>";

// Holder of message
x: 80#"\000";
.ffi.callFunction[("i"; `msvcrt`sprintf)] (x; "%s %f %hd\000"; "test\000"; 2f; 0h; ::);
"test 2.000000 0\000" ~ x til n

-1 "<< Testing strlen Function >>";

6i=.ffi.callFunction[("i"; `msvcrt`strlen)] (`abcdef;::);
3i=.ffi.callFunction[("i"; `msvcrt`strlen)] ("abc\000";::); 

-1 "<< Testing puts Function >>";

puts:.ffi.bind[`msvcrt`puts; "s"; "i"];
puts ("def\n\000"; ::);
puts (`abc; ::);

-1 "<< Testing qsort Function >>";

// Callbacks
cmp:{0N!x,y;(x>y)-x<y};

x:3 1 2i;
.ffi.callFunction[(" "; `msvcrt`qsort)] (x; `int$count x; 4i; (cmp;"II";"i"); ::);
x~1 2 3i

x:2 2 -1i;
.ffi.callFunction[(" "; `msvcrt`qsort)] (x; `int$count x; 4i; (cmp;"II";"i"); ::);
x~-1 2 2i

x:101b;
.ffi.callFunction[(" "; `msvcrt`qsort)] (x; `int$count x; 1i; (cmp;"BB";"i"); ::);
x~011b

x:`c`a`b;
// symbols are pointers - size of pointer is .ffi.ptrsize
.ffi.callFunction[(" "; `msvcrt`qsort)] (x; `int$count x; .ffi.ptrsize[]; (cmp;"SS";"i"); ::);
x~`a`b`c

-1 "<< Testing getpid Function >>";

.z.i=.ffi.callFunction[("i"; `msvcrt`_getpid)](::; ::)

-1 "<< Testing usleep Function >>";

// sleep for 10ms
10=10 xbar system"t .ffi.callFunction[(\" \";`Kernel32`Sleep)] (10i; ::)"

-1 "<< Testing strerror_s Function >>";

// decode errno
.ffi.callFunction[(" ";`msvcrt`strerror_s)] (errs; count errs:256#" "; 60i; ::);
// drop all after first \000
"Device not a stream" ~ first $["c";0] vs errs
