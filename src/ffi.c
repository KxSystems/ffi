//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                        Preamble                       //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//%% kdb+ Version %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

#define KXVER 3

//%% Platform Setting %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

#if((defined(_WIN32) || defined(WIN32)) && (defined(_MSC_VER)))
#define snprintf sprintf_s
#define EXP __declspec(dllexport)
#define RTLD_NODELETE 0
#elif defined(linux)
#define _GNU_SOURCE
#define EXP
#else
#define EXP
#endif

//%% Quench Warning from Compiler %%//vvvvvvvvvvvvvvvvvvvv/

#ifdef __GNUC__
#define UNUSED(x) x __attribute__((__unused__))
#else
#define UNUSED(x) x
#endif


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                     Load Libraries                    //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//

#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <ffi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "k.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                    Global Variables                   //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/// Indicate q error type
#define ER -128
/// Indicator of void type
#define VD -3
/// Indicator of q forreign type
#define FO 112
/// Indicator of raw pointer type
#define RP 127  // raw pointer

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                         Macros                        //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//%% Utility %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

/**
 * @brief Return minimum of two arguments.
 * @param a: argument1
 * @param b: argument2
 * @note
 * Arguments must implement `<` operator.
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define FFI_ALIGN(v, a) (((((size_t)(v)) - 1) | ((a)-1)) + 1)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                    Private Functions                  //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//%% OS Specific Functions %%//vvvvvvvvvvvvvvvvvvvvvvvvvvv/

#if defined(MACOSX) && defined(SYSFFI)
/**
 * @brief As per manual ffi_prep_closure on osx.
 * @note
 * This is needed to utilise system version of libffi. Not required if using libffi installed via brew.
 */
#include <sys/mman.h> // for mmap()
ffi_status ffi_prep_closure_loc(ffi_closure *closure,
                                ffi_cif *cif,
                                void (*func)(ffi_cif *cif, void *ret, void **args, void *userdata),
                                void *data,
                                void *loc)
{
  ffi_status status= ffi_prep_closure(closure, cif, func, data);
  if(status == FFI_OK) {
    if(mprotect(closure, sizeof(closure), PROT_READ | PROT_EXEC) == -1)
      status= FFI_BAD_ABI;
  }
  return status;
}

void *ffi_closure_alloc(size_t size, void **code) {
  ffi_closure *closure;
  // Allocate a page to hold the closure with read and write permissions.
  if((closure= mmap(NULL, sizeof(ffi_closure), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0)) == (void *)-1)
    return NULL;
  return closure;
}
#endif

//%% Common Functions %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

/**
 * @brief Convert character denoting q type to FFI type object.
 * @param c: Character denoting q type including following custom characters:
 * - ' ': void for return types
 * - 'r': for raw pointer(stored in i/j)
 * @return ffi_type
 */
static ffi_type *char_to_type(C c) {
  if(isupper(c)){
    // list
    return &ffi_type_pointer;
  }

  // atom
  switch(c){
    case 'b':
    case 'c':
    case 'x':
      // bool, char or byte
      return &ffi_type_uint8;
    case 'h':
      // short
      return &ffi_type_sint16;
    case 'i':
    case 'm':
    case 'd':
    case 'u':
    case 'v':
    case 't':
      // int, month, date, minute, second or time
      return &ffi_type_sint32;
    case 'j':
    case 'p':
    case 'n':
      // long, timestamp or timespan
      return &ffi_type_sint64;
    case 'e':
      // real
      return &ffi_type_float;
    case ' ':
      // void
      // valid only for return types
      return &ffi_type_void;
    case 'f':
    case 'z':
      // float or datetime
      return &ffi_type_double;
    case 'r':
    case 'k':
    case 'g':
    case 's':
      // raw pointer, K object, byte or symbol
      return &ffi_type_pointer;
    case 'l':
      return sizeof(size_t) == 4 ? &ffi_type_sint32 : &ffi_type_sint64;
    default:
      return NULL;
  } 
}

/**
 * @brief Convert integer q type indicator to FFI type object.
 * @param t: Integer denoting q type.
 * @return ffi_type
 */
static ffi_type *gettype(H t) {
  switch(t) {
    case -KB:
    case -KC:
    case -KG:
      // bool, char or byte
      return &ffi_type_uint8;
    case -KH:
      // short
      return &ffi_type_sint16;
    case -KM:
    case -KD:
    case -KU:
    case -KV:
    case -KT:
    case -KI:
      // month, date, minute, second, time or int
      return &ffi_type_sint32;
    case -KP:
    case -KN:
    case -KJ:
      // timestamp, timespan or long
      return &ffi_type_sint64;
    case -KE:
      // real
      return &ffi_type_float;
    case -KZ:
    case -KF:
      // datetime or float
      return &ffi_type_double;
    case -KS:
    case -UU:
      // symbol or GUID
      return &ffi_type_pointer;
    case VD:
      // void
      return &ffi_type_void;
    default:
      // list
      return &ffi_type_pointer;
  }
}

/**
 * @brief Return integer type indicator of the given character.
 * @param t: Character denoting q type.
 * @return int: q type indicator.
 */ 
static I ktype(C t) {
  Z C typechars[]= "kbg xhijefcspmdznuvt";
  C *p;
  if((p= strchr(typechars, t))) {
    // lowercase (atom)
    // Negate by subtracting from 0
    return typechars - p;
  }
  else if((p= strchr(typechars, tolower(t)))){
    // uppercase (list)
    return p - typechars;
  }
  else if(t == 'r'){
    // raw pointer
    return RP;
  }
  else{
    // Invalid indicator
    return ER;
  }
}

/**
 * @brief Wrap a given value as K object.
 * @param targettype: Integer denoting target q type wrapping `ret`.
 * @param ret: value to be returned wrapped as K object.
 * @return K: value of `ret` wrapped as K object.
 */
static K kvalue(I targettype, void *ret) {
  K r;
  if(targettype == 0){
    // Mixed list
    return *(K *)ret;
  }
  else if(targettype == VD){
    // Null
    return (K) 0;
  }
  else if(targettype == KC) {
    // string
    S rs=(S*) ret;
    printf("GOT STRING!!%s\n", *rs);
    r= ktn(KC, 1 + strlen(rs));
    memmove(kG(r), rs, r->n);
    return r;
  }
  else if(targettype == -RP) {
    // Raw pointer
    // Cast to int or long according to machine bitness
    return sizeof(void *) == 4? ki((I)ret): kj((J)ret);
  }
  else if(targettype > 0) {
    // list other than string
    // filling vector dereference ret array
    ret= *(void **)ret;
    // Negate and continue processing
    targettype = -targettype;
  }
  else{
    /* nothing to do */
  }

  // Create K object of target q type
  r= ka(targettype);
  switch(targettype) {
    case -KB:
    case -KC:
    case -KG:
      // bool, char or byte
      r->g= *(G *)ret;
      return r;
    case -KH:
      // short
      r->h= *(H *)ret;
      return r;
    case -KM:
    case -KD:
    case -KU:
    case -KV:
    case -KT:
    case -KI:
      // month, date, minute, second, time or int
      r->i= *(I *)ret;
      return r;
    case -KJ:
    case -KP:
    case -KN:
      // long, timestamp or timespan
      r->j= *(J *)ret;
      return r;
    case -KE:
      // real
      r->e= *(E *)ret;
      return r;
    case -KZ:
    case -KF:
      // datetime or float
      r->f= *(F *)ret;
      return r;
    case -KS:
      // symbol
      r->s= ss(*(S *)ret);
      return r;
    default:
      // Unsupported type
      return krr("unsupported return type");
  }
}

/**
 * @brief Build ffi_cif structure for use with ffi_call.
 * @param argtypes: type characters of arguments.
 * @param returntype: type character of returned value
 * @return
 * - list of (cif; ffi_atypes; atypes; rtype) at success in `ffi_prep_cif`
 * - Error status at failure in `ffi_prep_cif`
 */
static K cif(K argtypes, K returntype){

  if(argtypes->t != KC && argtypes->t != -KC){
    return krr("cif1: argtypes must be char or string");
  }   
  if(returntype->t != -KC){
    return krr("cif2: returntype must be char");
  }
  
  // Holder of (cif;ffi_atypes;atypes;rtype)
  K res= ktn(0, 4);

  // convert char to string for iteration
  argtypes = (argtypes -> t == -KC)? kpn((S)&argtypes->g, 1): r1(argtypes);

  // cif (holder)
  ffi_cif *pcif= (ffi_cif *) kG(kK(res)[0] = ktn(KG, sizeof(ffi_cif)));
  // ffi_atypes (holder)
  ffi_type **atypes = (ffi_type **) kG((kK(res)[1] = ktn(KG, argtypes->n * sizeof(ffi_type *))));
  // atypes
  kK(res)[2]= argtypes;
  // rtypes
  kK(res)[3]= r1(returntype);

  // Map argument types (char denoting q type) to ffi types
  for(J i=0; i!=argtypes -> n; ++i){
    atypes[i] = char_to_type(kC(argtypes)[i]);
    if(atypes[i] == NULL){
      // argtype hit unsupported type
      r0(res);
      return krr("unsupported argument type");
    }
  }

  // Prepares a ffi_cif structure for use with ffi_call
  switch(ffi_prep_cif(pcif, FFI_DEFAULT_ABI, argtypes->n, char_to_type(returntype->g), atypes)) {
    case FFI_OK:
      return res;
    case FFI_BAD_TYPEDEF:
      r0(res);
      return krr("prep_cif: FFI_BAD_TYPEDEF");
    case FFI_BAD_ABI:
      r0(res);
      return krr("prep_cif: FFI_BAD_ABI");
    default:
      r0(res);
      return krr("prep_cif: unknown");
  }
}

/**
 * @brief Search and return a function with a given name.
 * @param funcname
 * - symbol: Name of function to look for.
 * - symbol list: list of shared object and a function name in the library
 * @return
 * - function pointer: Pointer to the matched function
 * - Error: if a function with the given name was not found.
 */
void *lookupFunc(K funcname) {
  S name_to_search;
  Z C error_message[256];
  void *handle;
  void *func;
  
  // Clear any existing error
  dlerror();
  
  if(funcname->t == -KS){
    // Call internal function
    name_to_search = funcname->s;
    handle= RTLD_DEFAULT;
  }
  else if(funcname->t == KS && funcname->n == 2){
    // Open handle to the given shared object
    // bind now and don't unload until process exits
    handle= dlopen(kS(funcname)[0], RTLD_NOW | RTLD_NODELETE);
    if(!handle) {
      // Fail to open handle to the shared object
      snprintf(error_message, sizeof(error_message), "dlopen: %s", dlerror());
      return krr(error_message);
    }
    name_to_search = kS(funcname)[1];
  }
  else {
    // Unexpected input
    return krr("type: `func or `lib`func expected");
  }

  // clear any existing errors
  dlerror();

  // Find the address of the given function with the handle to the given shared object.
  func = dlsym(handle, name_to_search);
  if(!func) {
    // Cannot find the specified function
    snprintf(error_message, sizeof(error_message), "dlsym: func not found - %s", dlerror());
    // dlclose(handle); // check for RTLD_DEFAULT?
    return krr(error_message);
  }

  return func;
}

/**
 * @brief Apply function passeed by `userdata` to `args` after converting them to K objects.
 *  Result is stored into `resp` according to byte information in `cif`. 
 * @param cif: Structure storing type information of returned value from foreign function.
 * @param resp: Holder of returned value from the execution of the foreign function.
 * @param args: Arguments to be passed to the foreign function.
 * @param userdata: tuple of (function; characters denoting q types of arguments)
 */
static void closurefunc(ffi_cif *cif, void *resp, void **args, void *userdata) {
  
  // The number of arguments
  I n = cif->nargs;
  // Holder of arguments wrapped as K objects
  K args_as_k = ktn(0, n);
  // characters denoting q types of arguments
  K qtypes = kK((K)userdata)[1];
  for(I i= 0; i != n; ++i) {
    // Wrap arguments as K objects acording to given char q type indicators
    kK(args_as_k)[i]= kvalue(ktype(kC(qtypes)[i]), args[i]);
  }

  // Apply function to the list of arguments
  // Enlist arguments in case the number of arguments is more than 8 (breach limitation)
  K res= dot(kK((K)userdata)[0], n>8? (args_as_k = knk(1, args_as_k)): args_as_k);

  // Delete K arguments
  r0(args_as_k);

  if(cif->rtype != &ffi_type_void){
    // Non void return value
    // Fetch proper bytes from the response
    memset(resp, 0, cif->rtype->size);
  }

  if(!res){
    // Null result from `dot` (error occured during callback)
    return;
  }
    
  if(res->t < 0) {
    // atom
    // Select smaller bytes between type information about returned type registered in cif and returned information from execution
    I sz= MIN(cif->rtype->size, gettype(res->t)->size);
    // Copy proper bytes from the execution result to response
    memmove(resp, &res->k, sz);
  }
  r0(res);
}

/**
 * @brief
 * @param func_and_types: Tuple of (function; types of arguments) or (function; type characters of arguments; type character of returned value)
 * @return void**: Pointer to a closure whose type is `ffi_closure*`.
 * @note
 * - Closures are not free'd as maybe used for callbacks/etc from C.
 * - `closure_ptr`, `cif`, `atypes` will be retained until process exits
 */
static void *getclosure(K func_and_types, void **UNUSED(unused)) {
  ffi_status rc;
  void *boundf= NULL;

  // Allocate closure and rerturns a pointer to the writable address, and sets `boundf` to the
  //  corresponding executable address.
  ffi_closure *closure_ptr= ffi_closure_alloc(sizeof(ffi_closure), &boundf);

  // cif describes the function parameters. 
  // This object, and the types to which it refers, must be kept alive until the closure itself is freed.
  ffi_cif *cif= malloc(sizeof(ffi_cif));

  I n= kK(func_and_types)[1]->n;
  // Allocate same length of array as type characters of arguments
  ffi_type **atypes= malloc(sizeof(ffi_type *) * n);
  // Convert type characters into ffi type pointers and store in `types`
  DO(n, atypes[i]= char_to_type(kC(kK(func_and_types)[1])[i]));

  ffi_type *rtype= &ffi_type_void;
  if(func_and_types->n > 2){
    // Third element (type of returned value) was provided.
    // Convert q type character to ffi type pointer
    rtype= char_to_type(kK(func_and_types)[2]->g);
  }
    
  // Prepares a ffi_cif structure for use with ffi_call
  rc= ffi_prep_cif(cif, FFI_DEFAULT_ABI, n, rtype, atypes);
  // Cannot but assert since returned type is void...
  assert(rc == FFI_OK);

  rc= ffi_prep_closure_loc(closure_ptr, cif, closurefunc, r1(func_and_types), boundf);
  // Cannot but assert since returned type is void...
  assert(rc == FFI_OK);

  return closure_ptr;
}

/**
 * @brief Contain value of argument into void pointer and return the pointer.
 * @param targettype: Integer indicating target C type to convert to from q object in order to pass it to the foreign function.
 * @param arg: Argument
 * @param p: Container of the value behind `arg`.
 */
static void *getvalue(I targettype, K arg, void **p) {
  I argtype= arg->t;
  if(targettype == RP){
    // Raw pointer
    // Cast int or long value to void pointer according to bitness of machine.
    // Return a pointer to the value.
    return (sizeof(void *) == 4)? (void **) &(arg->i): (void **) &(arg->j);
  }
  else if(argtype == FO && arg->n > 1) {
    // Foreign function
    *p = kK(arg)[1];
    return (void *) p;
  }
  else if(argtype < 0){
    // Atom
    return (void *) &(arg->g);
  }
  else if(argtype == 0){
    // Tuple of (function; types of arguments) or (function; type characters of arguments; type character of returned value)
    // Get a pointer to a closure and conitune to processing
    *p = getclosure(arg, p);
  }
  else if(argtype <= KT){
    // Simple list
    // Get a begin pointer of the q list
    *p = kG(arg);
  }    
  else{
    // Otherwise store as it is
    *p = arg;
  }
    
  return (void *) p;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                       Interface                       //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//%% Private Interface %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

/**
 * @param cif: ffi_cif structure, which is a list of (cif; ffi_atypes; atypes; rtype) created in `bindf`.
 * @param func: Runtime address of the foreign function.
 * @param args: List of arguments passed to the bound foreign function. This list must have to have `(::)` in the end.
 * @note
 * This is an internal function exposed to q process. User should not call this function directly.
 *  This function is returned by `bind` with `cif` and `func` are bound, leaving `args` unfilled, i.e.
 *  the returned value looks monadic function to user. Then user passes arguments to the monadic function
 *  in the form of compound list.
 */
EXP K call(K cif, K func, K args){

  ffi_cif *pcif= (ffi_cif *) kG(kK(cif)[0]);
  if(pcif->abi != FFI_DEFAULT_ABI){
    // Inproper ABI
    return krr("wrong abi");
  }
    
  // Enlist `args` if atom was passed as argument
  // Increment reference count to copy
  args = (args->t < 0)? knk(1, r1(args)) : r1(args);
  if((args->t != 0) || (kK(args)[(args->n)-1]->t != 101)){
    // type of arguments is not compoound list
    r0(args);
    return krr("type of arguments must be a compound list with (::) at the end");
  }
    
  if((args->n)-1 != pcif->nargs){
    // The number of arguments passed to the function does not match the number of parameters of the foreign function
    // Decrement reference count
    r0(args);
    return krr("rank");
  }

  // func is runtime address passed by `bind`. No check is necessary
  /*
  if(func->t == KG)
    func= *(V **)kG(func);
  else
    func= lookupFunc(func);
  if(!func)
    R r0(args), (K)0;
  */

  // Trim trailing (::)
  I argc = (args->n) - 1;
  void **values= calloc(argc, sizeof(void *));
  void **pvalues= calloc(argc, sizeof(void *));
  // Copy values in args into pointers in pvalues with converting args into proper type specified in `atypes` in cif,
  //  turning back to the `argtypes` passed to `bind` function.
  DO(argc, values[i]= getvalue(ktype(kC(kK(cif)[2])[i]), kK(args)[i], &pvalues[i]));

  // Convert func to void* to pass to ffi_call
  func = *(void **) kG(func);
  // The third argument to ffi_call must point to storage that is `sizeof(long)` or larger
  char ret[FFI_SIZEOF_ARG];
  // Call the function and store returned value to `ret`
  ffi_call(pcif, func, &ret, values);
  free(pvalues);
  free(values);

  // Decrement reference count as args is not necessary any more
  r0(args);

  // krr returns nullptr
  if(!*ret){
    return (K) ret;
  }
  else{
    // Get integer type indicator from `returntype` use passed to `bind`
    I returntype= ktype(kK(cif)[3]->g);
    // Wrap the returned value according to the `returntype` and return it
    return kvalue(returntype, ret);
  }
}

//%% Public Interface %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

/**
 * @brief Build a cif based on `argtypes` and `returntypes` and returns a function bound to the cif.
 *  The function is fetched with the given name.
 * @param funcname:
 * - symbol: Name of function to look for.
 * - symbol list: list of shared object and a function name in the library
 * @param  argtypes: type characters of arguments.
 * @param returntype: type character of returned value
 * @return K: Result of the execution.
 */
EXP K bind(K funcname, K argtypes, K returntypes) {

  K bound = cif(argtypes, returntypes);
  // krr returns nullptr
  if(!(K) bound){
    // Error happened in `cif`.
    // Return K error.
    return bound;
  }

  void *func = lookupFunc(funcname);
  // krr returns nullptr
  if(!(K) func){
    // Error happened inside lookupFunc
    // Return the propagated error.
    r0(bound);
    return (K) func;
  }

  // Holder of function pointer (runtime address)
  K func_ptr= ktn(KG, sizeof(void *));
  // Move the address from `func` to `func_ptr`
  memmove(kG(func_ptr), &func, func_ptr->n);
  // Bind .ffi.call to the cif and the function. Return a monadic function.
  return k(0, ".ffi.call", bound, func_ptr, (K)0);
}

/**
 * @brief Call foreign function which has the given name with the given arguments after searching the foreign function.
 * @param returntype_and_func:
 * - symbol: Name of function
 * - tuple of (q type character of returned value)
 * @param args: List of arguments to be passed to the foreign function. This list must have (::) at the end.
 * @return K: Result of execution of the given function.
 */
EXP K call_function(K returntype_and_func, K args){
  
  H returntype;
  void *func= NULL;

  if((args->t != 0) || (kK(args)[(args->n)-1]->t != 101)){
    // Argument is not a compound list
    return krr("type of arguments must be a compound list with (::) at the end");
  }
  else if(returntype_and_func->t != 0 && returntype_and_func->t != -KS){
    // Neither of function name nor (return type character; function name) 
    return krr("expected a function name or a tuple of (type character of returned value; function name)");
  }   
  else if(returntype_and_func->t == 0) {
    // tuple of (type character of returned value; function name)
    if(returntype_and_func->n != 2 || kK(returntype_and_func)[0]->t != -KC){
      return krr("expected a function name or a tuple of (type character of returned value; function name)");
    }

    // Retrieve an integer type indicator from the given q type character of returned type 
    returntype = ktype(kK(returntype_and_func)[0]->g);
    if(returntype == ER){
      return krr("unsupported returned type");
    }

    // Get a pointer to the function with the given name and continue to processing  
    func= lookupFunc(kK(returntype_and_func)[1]);
  }
  else if(returntype_and_func->t == -KS) {
    // Function name was specified. Default return type is set int
    returntype = -KI;
    // Get a pointer to the function with the given name and continue to processing 
    func= lookupFunc(returntype_and_func);
  }

  // krr returns nullptr
  if(!(K) func){
    // Error happened inside `lookupFunc`
    // Returns the K error
    return (K) func;
  }
  
  // Trim trailing (::)
  I n = (args->n) - 1;
  ffi_type **types= malloc(sizeof(ffi_type *) * n);
  // Why different from pvalues?
  // values= malloc(sizeof(void *) * n);
  void **values= calloc(n, sizeof(void *));
  void **pvalues= calloc(n, sizeof(void *));
  for(int i= 0; i != n; ++i) {
    // Convert q types of arguments to ffi type
    types[i]= gettype(kK(args)[i]->t);
    // Contain value behind `args` to void pointer according to the underlying type in `args` itself and copy to `values`
    values[i]= getvalue(kK(args)[i]->t, kK(args)[i], &pvalues[i]);
  }

  // Holder of cif
  ffi_cif cif;
  // Prepares a ffi_cif structure for use with ffi_call
  ffi_status rc= ffi_prep_cif(&cif, FFI_DEFAULT_ABI, n, gettype(returntype), types);
  // Holder of returned value from execution
  char ret[FFI_SIZEOF_ARG];
  if(FFI_OK == rc){
    // Execute the foreign function
    ffi_call(&cif, func, &ret, values);
    free(pvalues);
    free(values);
    free(types);
  }
  else{
    free(pvalues);
    free(values);
    free(types);
    return (rc == FFI_BAD_TYPEDEF)? krr("prep: FFI_BAD_TYPEDEF"): krr("prep: FFI_BAD_ABI");
  }
    
  // Wrap the returned value as K object according to specified return type
  return kvalue(returntype, ret);
}

/**
 * @brief Search a variable with a given name and convert it to K object.
 * @param rtype_and_var:
 * - symbol: Variable name
 * - tuple of (char; symbol): (type character of returned value; variable name or list of shared object name and variable name)
 */
EXP K cvar(K rtype_and_var) {
  void *v;
  H target_type;

  if(rtype_and_var -> t == -KS) {
    // Return type was not provided. Default is set int
    target_type = -KI;
  }
  else {
    if(rtype_and_var -> t != 0 || rtype_and_var -> n != 2 || kK(rtype_and_var)[0]->t != -KC){
      // Must be a tuple of (type character of returned value; function name)
      return krr("expected a tuple of (type character of returned value; variable name)");
    }
    // Retrieve proper integer q type indicator based on the q type character
    target_type = ktype(kK(rtype_and_var)[0]->g);
    // Consume type character and leave function name only
    rtype_and_var = kK(rtype_and_var)[1];
  }

  // Get a variable with the given name
  v = lookupFunc(rtype_and_var);
  // krr returns nullptr
  if(!(K) v){
    // Error happened inside lookupFunc
    // Return the propagated error.
    return (K) v;
  }
    
  // Wrap the variable as K object
  return kvalue(target_type, v);
}

/**
 * @brief Bind foreign function which takes K and returns K object in current process which takes `n` arguments and returns q function.
 * @param funcname: Name of function
 * @param n: The number of arguments.
 * @return K: Function bound.
 * @note
 * Cannot find example anywhere. Seems not working. Probably garbage.
 */ 
EXP K bind_K_function(K funcname, K n) {
  
  if(funcname->t != -KS || (n->t != -KI && n->t != -KJ)){
    // Function name is not symbol or `n` is not a number.
    return krr("function name is not symbol or n is neither of int nor long");
  }
  
  // Get a pointer to the function with the given name
  void *func= lookupFunc(funcname);
  // krr returns nullptr
  if(!(K) func){
    // Error happened inside `lookupFunc`.
    // Return the K error
    return (K) func;
  }
    
  return dl(func, (-KJ == n->t)? n->j: n->i);
}

//%% Probably Garbage %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

/**
 * @brief Load all symbols in a given shared object to current process.
 * @param library: Name of shared object.
 * @note
 * Not documented anywhere. Probably garbage.
 */
EXP K loadlib(K library) {
  void *handle;
  static C error_message[256];
  if(library->t != -KS){
    // library name is not symbol
    return krr("library name must be symbol");
  }
    
  // load all symbols to current process
  handle= dlopen(library->s, RTLD_NOW | RTLD_NODELETE);
  if(!handle) {
    // Error in opening a handle to the shared object
    snprintf(error_message, sizeof(error_message), "loadlib: %s", dlerror());
    return krr(error_message);
  }

  return (K) 0;
}

/**
 * @brief Set a given number on `errno` and returns old `errno` value. If the argument is null or non-int value it returns current `errno` value. 
 * @param n: New `errno` value. This value must be int.
 * @return K: Old `errno` value.
 * @note
 * Not sure how to use this. Probably garbage.
 */
EXP K set_errno(K n) {
  I old= errno;
  if(n->t == -KI) {
    errno= n->i;
  }
  return ki(old);
}

/**
 * @brief Derefer ffi structs as K objects. Start index is given by `kidx` and the number of objects to be
 *  dereferred is given by a length of returntypes which denote return q types of the dereferred results.
 * @param x: UNKNOWN
 * @param returntypes: Characters denoting q types of returned values.
 * @param kidx: Index to start to read struct.
 * @note
 * This is not documented anywhere. Probably garbage.
 */ 
EXP K deref(K x, K returntypes, K kidx) {
  
  G *p;

  if((!((x->t == KJ && sizeof(void *) == 8) || (x->t == KI && sizeof(void *) == 4))) || returntypes->t != KC || kidx->t != -KJ) {
    // type and bitness does not match or return type is not string or index is not long
    return krr("type: [r;C;j] expected"); 
  }
  else if(sizeof(void *) == 4 && x->t == -KI) {
    // value is int and bitness is 4 bytes
    // Cast the value to a pointer
    p= (G *) x->i;
  }
  else if(sizeof(void *) == 8 && x->t == -KJ) {
    // value is long and bitness is 8 bytes
    // Cast the value to a pointer
    p= (G *)x->j;
  }
  else {
    // Unreachable branch
    return krr("unreachable!!");
  }

  ffi_type test_struct_type;
  test_struct_type.size= 0;
  test_struct_type.alignment= 0;
  test_struct_type.type= FFI_TYPE_STRUCT;

  ffi_type **elems= (ffi_type **) calloc(returntypes->n + 1, sizeof(void *));
  for(J i= 0; i < returntypes->n; ++i) {
    // Convert q type characters to ffi types
    elems[i]= char_to_type(kC(returntypes)[i]);
  }
  test_struct_type.elements= elems;

  // Holder of cif
  ffi_cif cif;
  if(ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, &test_struct_type, NULL) != FFI_OK){
    // Failed to prepares a ffi_cif structure for use with ffi_call
    return krr("cannot align resulting data");
  }

  J idx= kidx->j;
  // Proceed pointer
  p+= test_struct_type.size * idx;

  K derefed= ktn(0, returntypes->n);
  J offset= 0;
  for(J i= 0; i < derefed->n; ++i) {
    offset= FFI_ALIGN(offset, elems[i]->alignment);
    // Wrap test_struct_type as K object according to the specified return types
    kK(derefed)[i]= kvalue(ktype(kC(returntypes)[i]), p + offset);
    offset+= elems[i]->size;
  }

  return derefed;
}
