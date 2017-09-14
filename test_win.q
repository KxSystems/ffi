\l ffi.q
//.ffi.cf[`puts]("test\000";::)
//.ffi.cf[`printf]("%s %d\n\000";`test;1)
x: 80#"\000"
n: .ffi.cf[("i";`msvcrt`sprintf)](x;"%s %f %hd\000";"test\000"; 2f; 0h)
x til n

6i=.ffi.cf[("i";`msvcrt`strlen)]0N!(`abcdef;::)
3i=.ffi.cf[("i";`msvcrt`strlen)]0N!("abc\000";::) 

puts:.ffi.bind[`msvcrt`puts;"s",();"i"]
puts 0N!("def\n\000";::)
puts 0N!(`abc;::)

//.ffi.call[.ffi.cif["Gi";"i"];`printf]0N!("Hello %d\n\000";42)



//register func on handle
valueLater:()
.z.ts:{
  if[0=count valueLater;show "Nothing left on timer";exit 0;];
  show ("Timer exec";first valueLater);
  res:@[value;first valueLater;{show "Error executing timer - ",x}];
  valueLater _:0;
  }

// Callbacks
cmp:{0N!x,y;(x>y)-x<y} 
x:3 1 2i
.ffi.cf[(" ";`msvcrt`qsort)]0N!(x;`int$count x;4i;(cmp;"II";"i"))
x:2 2 -1i
.ffi.cf[(" ";`msvcrt`qsort)]0N!(x;`int$count x;4i;(cmp;"II";"i"))
x:101b
.ffi.cf[(" ";`msvcrt`qsort)]0N!(x;`int$count x;1i;(cmp;"BB";"i"))

x:`c`a`b;
// symbols are pointers - size of pointer is .ffi.nil length
.ffi.cf[(" ";`msvcrt`qsort)]0N!(x;`int$count x;`int$count .ffi.nil;(cmp;"SS";"i")) 
.z.i=.ffi.cf[("i";`msvcrt`_getpid)]()



// sleep for 10ms
10=10 xbar system"t .ffi.cf[(\" \";`Kernel32`Sleep)](10i;::)"

// decode errno
.ffi.cf[(" ";`msvcrt`strerror_s)](errs;count errs:256#" ";60i)
show first $["c";0] vs errs;  // drop all after first \0

\t 100
