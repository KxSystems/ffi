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

// @brief Creates a projection with the function resolved to call with arguments.
// @param funcname {dynamic}: Name of function.
// - symbol: Name of function to look for.
// - symbol list: list of shared object and a function name in the library.
// @param argtypes {string}: type characters of arguments.
// @param returntype {char}: type character of returned value
// @return
// - function
.ffi.bind:`ffikdb 2: (`bind; 3);

// @brief A simple function call, intended for one-off calls.
// @param returntype_and_func {dynaic}:
// - symbol: Name of function
// - tuple of (q type character of returned value; symbol function name or list of shared object name and function name)
// @param args {compound list}: List of arguments to be passed to the foreign function. This list must have (::) at the end.
// @return 
// - any
.ffi.callFunction:`ffikdb 2: (`call_function; 2);

// @brief Load all symbols in a given shared object to current process. Once eintire library
//  is loaded, shared library name does not need to be passed as an argument of `bind` and `callFunction`.
// @param shared_object {symbol}: Name of a shared library.
// @note
// This is an experimental feature.
.ffi.loadlib:`ffikdb 2: (`loadlib; 1);

//%% Utility %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

// @brief Search a variable with a given name and convert it to K object.
// @param rtype_and_var {dynamic}:
// - symbol: Variable name
// - tuple of (char; symbol): (type character of returned value; variable name or list of shared object name and variable name)
// @return 
// - any
.ffi.cvar:`ffikdb 2: (`cvar; 1);

// @brief Set a given number on `errno` and returns old `errno` value. If the argument is null or non-int
// value it returns current `errno` value. 
// @param n {int}: New `errno` value. This value must be int.
// @return 
// - int: Old `errno` value.
.ffi.setErrno:`ffikdb 2: (`set_errno; 1);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                           Other Utilities                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// @brief Return an extension of a shared library according to a kind of OS.
// @return
// - symbol: File extension.
.ffi.extension:{[] ("wlm"!`dll`so`dylib) first string .z.o};

// @brief Get a size of void pointer.
// @return
// - int: Size of a void pointer.
.ffi.ptrsize:{$[.z.o like "?32"; 4i; 8i]};

// @brief Get the first symbol of OS type.
// @return 
// - char: The first symbol of OS type.
.ffi.os:{[] first string .z.o};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                 Load for internal Reason (DO NOT CALL)               //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

.ffi.call:`ffikdb 2: (`call; 3);

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
