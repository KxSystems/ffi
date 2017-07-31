#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <errno.h>
#include <ffi.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "k.h"
#define MIN(a, b) ((a) < (b) ? (a) : (b))

Z ffi_type *chartotype(C c) {
  switch(tolower(c)) {
  case 'b':
  case 'c':
  case 'x':
    R &ffi_type_uint8;
  case 'h':
    R &ffi_type_sint16;
  case 'i':
    R &ffi_type_sint32;
  case 'j':
    R &ffi_type_sint64;
  case 'e':
    R &ffi_type_float;
  case 'f':
    R &ffi_type_double;
  case 'p':
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
    R &ffi_type_pointer;
  case -3:
    R &ffi_type_void;
  }
  R &ffi_type_pointer;
}
Z K retvalue(ffi_type *type, V *x) {
  switch(type->type) {
  case FFI_TYPE_VOID:
    R(K) 0;
  case FFI_TYPE_UINT8:
  case FFI_TYPE_SINT8:
    R kg(*(G *) x);
  case FFI_TYPE_UINT16:
  case FFI_TYPE_SINT16:
    R kh(*(H *) x);
  case FFI_TYPE_UINT32:
  case FFI_TYPE_SINT32:
  case FFI_TYPE_INT:
    R ki(*(I *) x);
  case FFI_TYPE_UINT64:
  case FFI_TYPE_SINT64:
    R kj(*(J *) x);
  case FFI_TYPE_FLOAT:
    R ke(*(E *) x);
  case FFI_TYPE_DOUBLE:
    R kf(*(F *) x);
  }
  R(K) 0;
}

Z I ktype(C t) {
  Z C ts[]= "kbg xhijefcs mdz uvt";
  C *p;
  if((p= strchr(ts, t))) {
    return ts - p;
  }
  if((p= strchr(ts, tolower(t))))
    return -(ts - p);
  return -128;
}

Z K kvalue(I t, void *ret) {
  K r;
  if(t == 0)
    R *(K *) ret;
  if(t == -3)
    R(K) 0;
  if(t == KC) {
    char *rs= *(char **) ret;
    r= ktn(KC, 1 + strlen(rs));
    memcpy(kG(r), rs, r->n);
    R r;
  }
  // if we filling vector dereference ret array
  if(t > 0) {
    ret= *(void **) ret;
    t= -t;
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

Z K cif(K x, K y) /* atypes, rtype */
{
  K r;
  ffi_type **atypes;
  ffi_cif *pcif;
  if(x->t != KC)
    R krr("cif1: argtypes");
  if(y->t != -KC)
    R krr("cif2: rtype");
  r= ktn(0, 3); /* cif,atypes,rtype */
  pcif= (ffi_cif *) kG(kK(r)[0]= ktn(KG, sizeof(ffi_cif)));
  atypes= (ffi_type **) kG((kK(r)[1]= ktn(KG, x->n * sizeof(V *))));
  kK(r)[2]= r1(y);
  DO(x->n, atypes[i]= chartotype(kC(x)[i]));
  switch(ffi_prep_cif(pcif, FFI_DEFAULT_ABI, x->n, chartotype(y->g), atypes)) {
  case FFI_OK:
    R r;
  case FFI_BAD_TYPEDEF:
    R r0(r), krr("FFI_BAD_TYPEDEF");
  case FFI_BAD_ABI:
    R r0(r), krr("FFI_BAD_ABI");
  }
  R krr("prep_cif");
}

Z V *getclosure(K x, V **p);

Z V *getvalue(K x, V **p) {
  if(x->t < 0)
    R(V *) & x->g;
  if(x->t == 0)
    *p= getclosure(x, p);
  else if(x->t <= KT)
    *p= xG;
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
Z K bind(K f, K a, K r) {
  K bound= cif(a, r), fp;
  if(!bound)
    return (K) 0;
  void *func= lookupFunc(f);
  if(!func)
    R r0(bound), (K) 0;
  fp= ks("");
  jk(&bound, fp);
  fp->s= func;
  R k(0, ".ffi.call", r1(bound), ks(""), (K) 0);
}

Z K call(K x, K y, K z) /*cif,func,values*/
{
  char ret[FFI_SIZEOF_ARG];
  ffi_cif *pcif;
  void *func;
  void **values, **pvalues;
  if(x->t != 0 || x->n < 2)
    R krr("ciftype");
  if(y->t != -KS)
    R krr("functype");
  if(z->t != 0)
    R krr("argtype");
  pcif= (ffi_cif *) kG(kK(x)[0]);
  if(pcif->abi != FFI_DEFAULT_ABI)
    R krr("abi");
  if(x->n > 3)
    func= kK(x)[3]->s;
  else
    func= lookupFunc(y);
  if(!func)
    R(K) 0;
  values= malloc(sizeof(V *) * z->n);
  pvalues= malloc(sizeof(V *) * z->n);
  DO(z->n, values[i]= getvalue(kK(z)[i], &pvalues[i]));
  ffi_call(pcif, func, &ret, values);
  free(pvalues);
  free(values);
  R kvalue(ktype(kK(x)[2]->g), ret); // retvalue(pcif->rtype, ret);
}

Z V closurefunc(ffi_cif *cif, void *resp, void **args, void *userdata) {
  I i, n= cif->nargs, sz;
  K x= ktn(0, n);
  K t= kK((K) userdata)[1];
  for(i= 0; i != n; ++i) {
    kK(x)[i]= kvalue(ktype(kC(t)[i]), args[i]);
  }
  K r= dot(kK((K) userdata)[0], x);
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
Z V *getclosure(K x, V **p) {
  ffi_status rc;
  ffi_type *rtype= &ffi_type_pointer;
  void *boundf;
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

Z V freeclosure(V *p) {
  // don't free closure as maybe used for callbacks/etc from c
  // pcl,cif,arg_types will be retained until process exits
  /*
ffi_closure *pcl= p;
free(pcl->cif->arg_types);
free(pcl->cif);
free(pcl);
*/
}

Z K cf(K x, K y) /* simple call: f|(r;f),args */
{
  K r;
  ffi_cif cif;
  ffi_type **types;
  ffi_status rc;
  char *f;
  H rt;   /* return type */
  I i, n; /* args count */
  void *handle, *func, **values, **pvalues;
  char ret[FFI_SIZEOF_ARG];
  if(y->t != 0)
    return krr("type: args should be generic list");
  n= y->n;
  if(x->t != 0 && x->t != -KS)
    return krr("type: func should be sym or generic list");
  if(x->t == 0) {
    char *p;
    if(x->n != 2 || kK(x)[0]->t != -KC)
      R krr("type");
    rt= ktype(kK(x)[0]->g);
    if(rt == -128)
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
    values[i]= getvalue(kK(y)[i], &pvalues[i]);
  }
  rc= ffi_prep_cif(&cif, FFI_DEFAULT_ABI, n, gettype(rt), types);
  if(FFI_OK == rc)
    ffi_call(&cif, func, &ret, values);
  for(i= 0; i != n; ++i) {
    if(pvalues[i] && values[i] != &pvalues[i])
      freeclosure(pvalues[i]);
  }
  free(pvalues);
  free(values);
  free(types);
  if(FFI_OK != rc)
    R krr("prep");
  return kvalue(rt, ret);
}

K kfn(K x, K y) {
  V *func;
  if(x->t != -KS || y->t != -KI && y->t != -KJ)
    R krr("type");
  func= lookupFunc(x);
  if(!func)
    R(K) 0;
  R dl(func, -KJ == y->t ? y->j : y->i);
}

K ern(K x) {
  I old= errno;
  if(x->t == -KI) {
    errno= x->i;
  }
  return ki(old);
}

K deref1(K x) {
  if(x->t != -KJ)
    return krr("type");
  if(!x->j)
    return krr("deref null");
  return kj((J) * (V **) x->j);
}

K deref(K x) {
  K r;
  if(x->t != KG)
    return krr("type");
  if(x->n < sizeof(K))
    return krr("length: too small");
  if(!*(G **) kG(x))
    return krr("deref null");
  r= ktn(KG, x->n);
  memcpy(kG(r), *(V **) kG(x), r->n);
  return r;
}

#define N 8
#define FFIQ_ENTRY(i, name, def)                                               \
  kS(x)[i]= ss(name);                                                          \
  kK(y)[i]= def
#define FFIQ_FUNC(i, name, nargs) FFIQ_ENTRY(i, #name, dl(name, nargs))
#define FFIQ_ENUM(i, name) FFIQ_ENTRY(i, #name, ki(name))
K ffi(K x) {
  K y= ktn(0, N);
  x= ktn(KS, N);
  FFIQ_ENTRY(0, "", k(0, "::", (K) 0));
  FFIQ_FUNC(1, cif, 2);
  FFIQ_FUNC(2, bind, 3);
  FFIQ_FUNC(3, call, 3);
  FFIQ_FUNC(4, cf, 2);
  FFIQ_FUNC(5, kfn, 2);
  FFIQ_FUNC(6, ern, 1);
  FFIQ_FUNC(7, deref, 1);
  R xD(x, y);
}
