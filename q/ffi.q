//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            File Description                          //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* @file
* ffi.q
* @overview
* Define ffi interface functions.
\

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Libraries                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//%% Main Interface %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

.ffi.bind:`libffikdb 2: (`bind; 3);
.ffi.callFunction:`libffikdb 2: (`call_function; 2);

//%% Utility %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

.ffi.cvar:`libffikdb 2: (`cvar; 1);
.ffi.setErrno:`libffikdb 2: (`set_errno; 1);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                           Other Utilities                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

.ffi.extension:{[] ("wlm"!`dll`so`dylib) first string .z.o};
.ffi.ptrsize:{$[.z.o like "?32"; 4i; 8i]};
.ffi.os:{first string .z.o};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                 Load for internal Reason (DO NOT CALL)               //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

.ffi.call:`libffikdb 2: (`call; 3);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                             Deprecated                               //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

.ffi.cf:{[returnedtype_and_func;args]
  -1 "Using deprecated function .ffi.cf!!";
  -1 "This function will vanish from the ver 0.13. Use .ffi.callFunction instead.";
  .ffi.callFunction[returnedtype_and_func;args]
 };

.ffi.kfn:{[funcname;n]
  -1 "Using deprecated function .ffi.kfn!!";
  -1 "This function will vanish from the ver 0.13. Use .ffi.asQFunc instead.";
  .ffi.asQFunc[funcname; n]
 };

.ffi.ext:{
  -1 "Using deprecated function .ffi.ext!!";
  -1 "This function will vanish from the ver 0.13. Use .ffi.extension instead.";
  ` sv x,("wlm"!`dll`so`dylib)first string .z.o
 };

.ffi.errno:{[n]
  -1 "Using deprecated function .ffi.errno!!";
  -1 "This function will vanish from the ver 0.13. Use .ffi.setErrno instead.";
  .ffi.setErrno[n]
 };

// This function is useless because null is now null, not 0i or 0j.
// .ffi.nil:{$[.z.o like "?32";0i;0j]};