//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            File Description                          //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* @file
* stdlib.q
* @overview
* This file shows how to use ffi interface to utilize functions in C standard library
\

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Libraries                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

\l ffi.q

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Functions                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// void qsort(void *base, size_t num_members, size_t size, int (*compare_function) (const void *, const void *));
// Size of void* varies by machine. "l" comes to help here.
// q function passed as a callback function is "k" (function pointer)
.cstdlib.qsort:.ffi.bind[`qsort; "liik"; " "];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                             Play Around                              //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//%% sprintf %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

// Buffer
buffer: 80#"\000";
// Write to buffer and return the number of bytes which was written
n:.ffi.callFunction[("i"; `sprintf)] (buffer; "%s %.4f %hd\000"; "example\000"; 3.16f; 144000h; ::);
// Show the written charcters
show buffer til n;

//%% Quick Sort %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

// Callback function
cmp:{0N!"compare: ",.Q.s x,y; (x>y)-x<y};

seq:3 1 2i
.cstdlib.qsort (seq; `int$count seq; 4i; (cmp; "II"; "i"); ::);
show seq;

seq:101b
.cstdlib.qsort (seq; `int$count seq; 1i; (cmp; "BB"; "i"); ::);
show seq;

seq:`c`a`b;
.cstdlib.qsort (seq; `int$count seq; 8i; (cmp; "SS"; "i"); ::); 
show seq;