\l ffi.q
\d .ffi
puts_cif:cif[list"s";"i"]
call[puts_cif;`puts](`abc;::)
call[puts_cif;`puts]("def\n\000";::)
call[cif["si";"i"];`printf]("Hello %d\n\000";42)
