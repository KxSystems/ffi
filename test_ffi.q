\l ffi.q
\d .ffi
cf[`puts]("test\000";::)
cf[`printf]("%s %d\n\000";`test;1)
x: 80#"\000"
n: cf[`sprintf](x;"%s %f %hd\000";"test\000"; 2f; 0h)
x til n

puts_cif:cif[list"s";"i"]
call[puts_cif;`puts](`abc;::)
call[puts_cif;`puts]("def\n\000";::)
call[cif["si";"i"];`printf]("Hello %d\n\000";42)
\\
