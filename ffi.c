#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <ffi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if((defined(_WIN32) || defined(WIN32)) && (defined(_MSC_VER)))
#define snprintf sprintf_s
#define EXP __declspec(dllexport)
#define RTLD_NODELETE 0
#else
#define EXP
#endif

#if defined(MACOSX) && defined(SYSFFI)
// as per man ffi_prep_closure on osx
// this is needed to utilise system version of libffi. Not required if using libffi installed via brew.
#include <sys/mman.h> // for mmap()
ffi_status ffi_prep_closure_loc(ffi_closure *closure, ffi_cif *cif,
                                void (*func)(ffi_cif *cif, void *ret,
                                             void **args, void *user_data),
                                void *data, void *loc) {
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
  if((closure= mmap(NULL, sizeof(ffi_closure), PROT_READ | PROT_WRITE,
                    MAP_ANON | MAP_PRIVATE, -1, 0)) == (void *) -1)
    return NULL;
  return closure;
}
#endif

#define KXVER 3
#include "k.h"
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define VD -3   // void
#define RP 127  // raw pointer
#define ER -128 // error

/*
' ' - void for return types
'r' - for raw pointer(stored in kG)
*/

Z ffi_type *chartotype(C c) {
  if(isupper(c))
    R &ffi_type_pointer;
  switch(tolower(c)) {
  case 'b':
  case 'c':
  case 'x':
    R &ffi_type_uint8;
  case 'h':
    R &ffi_type_sint16;
  case 'i':
  case 'm':
  case 'd':
  case 'v':
    R &ffi_type_sint32;
  case 'j':
  case 'p':
  case 'n':
    R &ffi_type_sint64;
  case 'e':
    R &ffi_type_float;
  case ' ':
    R &ffi_type_void; // valid only for return types
  case 'f':
  case 'z':
    R &ffi_type_double;
  case 'r':
  case 'k':
  case 'g':
  case 's':
    R &ffi_type_pointer;
  }
  R NULL;
}
Z ffi_type *gettype(H t) {
  switch(t) {
  case -KB:
  case -KC:
  case -KG:
    R &ffi_type_uint8;
  case -KH:
    R &ffi_type_sint16;
  case -KM:
  case -KD:
  case -KU:
  case -KV:
  case -KT:
  case -KI:
    R &ffi_type_sint32;
  case -KP:
  case -KN:
  case -KJ:
    R &ffi_type_sint64;
  case -KE:
    R &ffi_type_float;
  case -KZ:
  case -KF:
    R &ffi_type_double;
  case -KS:
  case -UU:
    R &ffi_type_pointer;
  case VD:
    R &ffi_type_void;
  }
  R &ffi_type_pointer;
}

Z K retvalue(I t, ffi_type *ft, V *ret) {
  switch(ft->type) {
  case FFI_TYPE_VOID:
    R(K) 0;
  case FFI_TYPE_UINT8:
  case FFI_TYPE_SINT8:
    R kg(*(G *) ret);
  case FFI_TYPE_UINT16:
  case FFI_TYPE_SINT16:
    R kh(*(H *) ret);
  case FFI_TYPE_UINT32:
  case FFI_TYPE_SINT32:
  case FFI_TYPE_INT:
    R ki(*(I *) ret);
  case FFI_TYPE_UINT64:
  case FFI_TYPE_SINT64:
    R kj(*(J *) ret);
  case FFI_TYPE_FLOAT:
    R ke(*(E *) ret);
  case FFI_TYPE_DOUBLE:
    R kf(*(F *) ret);
  }
  R(K) 0;
}

Z I ktype(C t) {
  Z C ts[]= "kbg xhijefcspmdznuvt";
  C *p;
  if((p= strchr(ts, t))) {
    return ts - p;
  }
  if((p= strchr(ts, tolower(t))))
    return -(ts - p);
  if(t == 'r')
    return RP;
  return ER;
}

Z K kvalue(I t, void *ret) {
  K r;
  if(t == 0)
    R *(K *) ret;
  if(t == VD)
    R(K) 0;
  if(t == KC) {
    char *rs= *(char **) ret;
    r= ktn(KC, 1 + strlen(rs));
    memmove(kG(r), rs, r->n);
    R r;
  }
  // if we filling vector dereference ret array
  if(t > 0) {
    ret= *(void **) ret;
    t= -t;
  }
  if(t == -RP) {
    t= ffi_type_pointer.size == 4 ? -KI : -KJ;
  }
  r= ka(t);
  switch(t) {
  case -KB:
  case -KC:
  case -KG:
    R r->g= *(G *) ret, r;
  case -KH:
    R r->h= *(H *) ret, r;
  case -KM:
  case -KD:
  case -KU:
  case -KV:
  case -KT:
  case -KI:
    R r->i= *(I *) ret, r;
  case -KJ:
  case -KP:
  case -KN:
    R r->j= *(J *) ret, r;
  case -KE:
    R r->e= *(E *) ret, r;
  case -KZ:
  case -KF:
    R r->f= *(F *) ret, r;
  case -KS:
    R r->s= ss(*(S *) ret), r;
  }
  R krr("rtype");
}

Z K cif(K x, K y) /* atypes, rtype -> (cif;ffi_atypes;atypes;rtype) */
{
  K r;
  ffi_type **atypes;
  ffi_cif *pcif;
  if(x->t != KC && x->t!= -KC)
    R krr("cif1: argtypes");
  if(y->t != -KC)
    R krr("cif2: rtype");
  r= ktn(0, 4); x=x->t==-KC?kpn((S)&x->g,1):r1(x);
  /* cif,ffi_atypes,atypes, rtype */
  pcif= (ffi_cif *) kG(kK(r)[0]= ktn(KG, sizeof(ffi_cif)));
  atypes= (ffi_type **) kG((kK(r)[1]= ktn(KG, x->n * sizeof(ffi_type *))));
  kK(r)[2]= x;
  kK(r)[3]= r1(y);
  DO(x->n, atypes[i]= chartotype(kC(x)[i]));
  switch(ffi_prep_cif(pcif, FFI_DEFAULT_ABI, x->n, chartotype(y->g), atypes)) {
  case FFI_OK:
    R r;
  case FFI_BAD_TYPEDEF:
    R r0(r), krr("prep_cif: FFI_BAD_TYPEDEF");
  case FFI_BAD_ABI:
    R r0(r), krr("prep_cif: FFI_BAD_ABI");
  }
  R r0(r), krr("prep_cif: unknown");
}

Z V *getclosure(K x, V **p);

Z V *getvalue(I t, K x, V **p) {
  t= x->t;
  if(t < 0)
    R(V *) & x->g;
  if(t == 0)
    *p= getclosure(x, p);
  else if(t <= KT)
    *p= kG(x);
  else
    *p= x;
  R(V *) p;
}

void *lookupFunc(K x) {
  S f;
  Z C err[256];
  void *handle, *func;
  dlerror(); /* Clear any existing error */
  if(x->t == -KS) {
    f= x->s;
    handle= RTLD_DEFAULT;
  } else if(x->t == KS && x->n == 2) {
    // bind now and don't unload until process exits
    handle= dlopen(kS(x)[0], RTLD_NOW | RTLD_NODELETE);
    if(!handle) {
      snprintf(err, sizeof(err), "dlopen: %s", dlerror());
      R krr(err);
    }
    f= kS(x)[1];
  } else {
    R krr("type: `func or `lib`func expected");
  }
  dlerror(); // clear any existing errors
  func= dlsym(handle, f);
  if(!func) {
    snprintf(err, sizeof(err), "dlsym: func not found - %s", dlerror());
    // dlclose(handle); // check for RTLD_DEFAULT?
    R krr(err);
  }
  return func;
}

// [func;atypes;rtype]
EXP K bindf(K f, K a, K r) {
  K bound= cif(a, r), fp;
  void *func;
  if(!bound)
    R(K) 0;
  func= lookupFunc(f);
  if(!func)
    R r0(bound), (K) 0;
  fp= ktn(KG, sizeof(V *));
  memcpy(kG(fp), &func, fp->n);
  R k(0, ".ffi.call", bound, fp, (K) 0);
}

EXP K call(K x, K y, K z) /*cif,func,values*/
{
  char ret[FFI_SIZEOF_ARG];
  ffi_cif *pcif;
  void *func, **values, **pvalues;
  I argc, rt;
  if(x->t != 0 || x->n < 2)
    R krr("ciftype");
  if(z->t != 0)
    R krr("argtype");
  pcif= (ffi_cif *) kG(kK(x)[0]);
  if(pcif->abi != FFI_DEFAULT_ABI)
    R krr("abi");
  if(y->t == KG)
    func= *(V **) kG(y);
  else
    func= lookupFunc(y);
  if(!func)
    R(K) 0;
  if(z->n < pcif->nargs)
    R krr("rank");
  // min of passed args and number of argtypes in cif
  argc= MIN(z->n, pcif->nargs);
  rt= ktype(kK(x)[3]->g);
  values= malloc(sizeof(V *) * argc);
  pvalues= malloc(sizeof(V *) * argc);
  DO(argc, values[i]= getvalue(ktype(kC(kK(x)[2])[i]), kK(z)[i], &pvalues[i]));
  ffi_call(pcif, func, &ret, values);
  free(pvalues);
  free(values);
  R kvalue(rt, ret);
}

Z V closurefunc(ffi_cif *cif, void *resp, void **args, void *userdata) {
  I i, n= cif->nargs, sz;
  K x= ktn(0, n), r, t= kK((K) userdata)[1];
  for(i= 0; i != n; ++i) {
    kK(x)[i]= kvalue(ktype(kC(t)[i]), args[i]);
  }
  r= dot(kK((K) userdata)[0], x);
  r0(x);
  if(cif->rtype != &ffi_type_void)
    memset(resp, 0, cif->rtype->size);
  if(!r)
    return; // error occured during callback from c
  if(r->t < 0) {
    sz= MIN(cif->rtype->size, gettype(r->t)->size);
    memcpy(resp, &r->k, sz);
  }
  r0(r);
}

// x is (func;atypes) or (func;atypes;rtype) - similar to cif
// Note: Closure's are not free'd as maybe used for callbacks/etc from c
// pcl,cif,arg_types will be retained until process exits
Z V *getclosure(K x, V **p) {
  ffi_status rc;
  ffi_type *rtype= &ffi_type_void;
  void *boundf= NULL;
  ffi_closure *pcl= ffi_closure_alloc(sizeof(ffi_closure), &boundf);
  ffi_cif *cif= malloc(sizeof(ffi_cif));
  I n= kK(x)[1]->n;
  ffi_type **types;
  types= malloc(sizeof(ffi_type *) * n);
  DO(n, types[i]= chartotype(kC(kK(x)[1])[i]));
  if(x->n > 2)
    rtype= chartotype(kK(x)[2]->g);
  rc= ffi_prep_cif(cif, FFI_DEFAULT_ABI, n, rtype, types);
  assert(rc == FFI_OK);
  rc= ffi_prep_closure_loc(pcl, cif, closurefunc, r1(x), boundf);
  assert(rc == FFI_OK);

  R pcl;
}

EXP K cf(K x, K y) /* simple call: f|(r;f),args */
{
  ffi_cif cif;
  ffi_type **types;
  ffi_status rc;
  H rt;   /* return type */
  I i, n; /* args count */
  void *func= NULL, **values, **pvalues;
  char ret[FFI_SIZEOF_ARG];
  if(y->t != 0)
    return krr("type: args should be generic list");
  n= y->n;
  if(x->t != 0 && x->t != -KS)
    return krr("type: func should be sym or generic list");
  if(x->t == 0) {
    if(x->n != 2 || kK(x)[0]->t != -KC)
      R krr("type");
    rt= ktype(kK(x)[0]->g);
    if(rt == ER)
      return krr("type");
    func= lookupFunc(kK(x)[1]);
  } else if(x->t == -KS) {
    rt= -KI;
    func= lookupFunc(x);
  }
  if(!func)
    R(K) 0;
  types= malloc(sizeof(ffi_type *) * n);
  values= malloc(sizeof(V *) * n);
  pvalues= calloc(n, sizeof(V *));
  for(i= 0; i != n; ++i) {
    types[i]= gettype(kK(y)[i]->t);
    values[i]= getvalue(kK(y)[i]->t, kK(y)[i], &pvalues[i]);
  }
  rc= ffi_prep_cif(&cif, FFI_DEFAULT_ABI, n, gettype(rt), types);
  if(FFI_OK == rc)
    ffi_call(&cif, func, &ret, values);
  free(pvalues);
  free(values);
  free(types);
  if(FFI_OK != rc)
    R krr(rc == FFI_BAD_TYPEDEF ? "prep: FFI_BAD_TYPEDEF" :
                                  "prep: FFI_BAD_ABI");
  return kvalue(rt, ret);
}

EXP K kfn(K x, K y) {
  V *func;
  if(x->t != -KS || (y->t != -KI && y->t != -KJ))
    R krr("type");
  func= lookupFunc(x);
  if(!func)
    R(K) 0;
  R dl(func, -KJ == y->t ? y->j : y->i);
}

EXP K loadlib(K x) {
  V *handle;
  Z C err[256];
  if(x->t != -KS)
    R krr("type");
  // load all symbols to current process
  handle= dlopen(x->s, RTLD_NOW | RTLD_NODELETE);
  if(!handle) {
    snprintf(err, sizeof(err), "loadlib: %s", dlerror());
    R krr(err);
  }
  R(K) 0;
}

EXP K ern(K x) {
  I old= errno;
  if(x->t == -KI) {
    errno= x->i;
  }
  return ki(old);
}

EXP K deref(K x) {
  K r;
  if(x->t != KG)
    return krr("type");
  if(x->n < sizeof(V *))
    return krr("length: too small");
  if(!*(V **) kG(x))
    return krr("deref null");
  r= ktn(KG, x->n);
  memcpy(kG(r), *(V **) kG(x), r->n);
  return r;
}

Z K cvar(K x) {
  V *v;
  H t;
  if(x->t == -KS) {
    t= -KI;
  } else {
    if(x->t != 0 || x->n != 2 || kK(x)[0]->t != -KC)
      R krr("type");
    t= ktype(kK(x)[0]->g);
    x= kK(x)[1];
  }
  v= lookupFunc(x);
  if(!v)
    R(K) 0;
  R kvalue(t, v);
}

#define N 10
#define FFIQ_ENTRY(i, name, def)                                               \
  kS(x)[i]= ss(name);                                                          \
  kK(y)[i]= def
#define FFIQ_FUNC(i, name, nargs) FFIQ_ENTRY(i, #name, dl(name, nargs))
#define FFIQ_ENUM(i, name) FFIQ_ENTRY(i, #name, ki(name))
EXP K ffi(K x) {
  K y= ktn(0, N);
  x= ktn(KS, N);
  FFIQ_ENTRY(0, "", k(0, "::", (K) 0));
  FFIQ_FUNC(1, cif, 2);
  FFIQ_FUNC(2, bindf, 3);
  FFIQ_FUNC(3, call, 3);
  FFIQ_FUNC(4, cf, 2);
  FFIQ_FUNC(5, kfn, 2);
  FFIQ_FUNC(6, ern, 1);
  FFIQ_FUNC(7, deref, 1);
  FFIQ_FUNC(8, loadlib, 1);
  FFIQ_FUNC(9, cvar, 1);

  R xD(x, y);
}
