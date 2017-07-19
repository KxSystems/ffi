\l ffi.q
\d .ffi
cf[`puts]("test\000";::)
cf[`printf]("%s %d\n\000";`test;1)
x: 80#"\000"
n: cf[`sprintf](x;"%s %f %hd\000";"test\000"; 2f; 0h)
x til n

puts_cif:cif[(),"s";"i"]
call[puts_cif;`puts](`abc;::)
call[puts_cif;`puts]("def\n\000";::)
call[cif["si";"i"];`printf]("Hello %d\n\000";42)

//register func on handle
r:{b:20#"\000";n:cf[`read](x;b;20);0N!n#b;0}
//cf[`sd1](h;(r;(),"i")) / start handler
//cf[`sd0](h;::)          / stop handler

// Callbacks
cmp:{0N!x,y;(x>y)-x<y} 
x:3 1 2i
cf[(" ";`qsort)]0N!(x;count x;4;(cmp;"II")) 

x:`c`a`b;
cf[(" ";`qsort)](x;count x;8;(cmp;"SS")) 

\\
