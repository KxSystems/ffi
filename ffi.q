.ffi:(`:./ffi 2:`ffi,1)[]
\d .ffi
// prepare calling interface with arg_types and return type 
cif0:{[arg_types;ret_type] cif[args_types;ret_type]}
// bind function - similar to cif, but performs lookup in advance
bind0:{[func;arg_types;ret_type] bind[func;arg_types;ret_type]} 
// call prepared cif with values
call0:{[cif;func;vals]call[cif;func;vals]}
// simple call: f|(r;f),args */
cf0:{[f;args]cf[f;args]}
// dlsym for sym&nargs. similar to 2:
kfn0:{[sym;nargs]kfn[sym;nargs]}
// gets/sets errno
errno:{ern x}
// dereference pointer to another pointer
deref0:{deref x}
nil:8#0x0
\d .

