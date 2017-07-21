.ffi:(`:./ffi 2:`ffi,1)[]
\d .ffi
// prepare calling interface with arg_types and return type 
cif0:{[arg_types;ret_type] cif[args_types;ret_type]}  
// call prepared cif with values
call0:{[cif;func;vals]call[cif;func;vals]}
// simple call: f|(r;f),args */
cf0:{[f;args]cf[f;args]}
// dlsym for sym&nargs. similar to 2:
kfn0:{[sym;nargs]kfn[sym;nargs]}
// gets/sets errno
errno:{ern x}
\d .

