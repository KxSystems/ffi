/* -*- mode: c; c-basic-offset: 8 -*- */
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <ffi.h>
#include "k.h"

extern K dl(V*f,I n);
int myputs(char*s){R puts(s);}
ZK null() {K x=ka(101);xi=0;R x;}
K1(test){
      ffi_cif cif;
      ffi_type *args[1];
      void *values[1];
      char *s;
      int rc;
      /*
      int (*puts)(const char *) = dlsym(0, "puts");
      if (!puts) R orr("dlsym");

      puts("xyz\n"); R null();
      */
      /* Initialize the argument info vectors */
      args[0] = &ffi_type_pointer;
      values[0] = &s;

      /* Initialize the cif */
      if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1,
                       &ffi_type_uint, args) == FFI_OK)
        {
          s = "Hello World!";
          ffi_call(&cif, (void*)myputs, &rc, values);
          /* rc now holds the result of the call to puts */

          /* values holds a pointer to the function's arg, so to
             call puts() again all we need to do is change the
             value of s */
          s = "This is cool!";
          ffi_call(&cif, (void*)myputs, &rc, values);
        }

      R ks(ss("test"));
}
Z ffi_type* 
chartotype(G c)
{
	switch (c) {
	case 'g': R &ffi_type_uint8;
	case 'h': R &ffi_type_sint16;
	case 'i': R &ffi_type_sint32;
	case 'j': R &ffi_type_sint64;
	case 'e': R &ffi_type_float;
	case 'f': R &ffi_type_double;
	case 'p':
	case 's': R &ffi_type_pointer;
	}
	R NULL;
} 
Z K2(cif) /* atypes, rtype */
{
	K r;
	ffi_type **atypes;
	ffi_cif   *pcif;
	if (xt != KC)
		R krr("cif1");
	if (y->t != -KC)
		R krr("cif2");
	r = ktn(0, 2); /* cif,atypes */
	pcif = (ffi_cif*)kG(kK(r)[0] = ktn(KG, sizeof(ffi_cif)));
	atypes = (ffi_type**)kG((kK(r)[1] = ktn(KG, xn*sizeof(V*))));
	DO(xn,atypes[i]=chartotype(xC[i]));
	switch (ffi_prep_cif(pcif, FFI_DEFAULT_ABI, xn,
			     chartotype(y->g), atypes))
		{
		case FFI_OK:
			R r;
		case FFI_BAD_TYPEDEF:
			R r0(r),krr("FFI_BAD_TYPEDEF");
		case FFI_BAD_ABI:
			R r0(r),krr("FFI_BAD_ABI");
		}
	R krr("prep_cif");
}

ZV*
getvalue(K x, V**p)
{
	if (xt < 0)
		R &xg;
	*p = xG;
	R (V*)p;
}

ZK
retvalue(ffi_type *type, V *x)
{
	switch (type->type) {
	case FFI_TYPE_VOID:    R null();
	case FFI_TYPE_UINT16:
	case FFI_TYPE_SINT16:  R kh(*(H*)x);
	case FFI_TYPE_UINT32:
	case FFI_TYPE_SINT32:
	case FFI_TYPE_INT:     R ki(*(I*)x);
	case FFI_TYPE_FLOAT:   R ke(*(E*)x);
	case FFI_TYPE_DOUBLE:  R kf(*(F*)x);
	}
	R null();
}

ZK
call(K x, K y, K z) /*cif,func,values*/
{
	char ret[FFI_SIZEOF_ARG];
	ffi_cif *pcif;
	void *func;
	void **values, **pvalues;
	if (xt != 0 || xn != 2)
		R krr("ciftype");
	if (y->t != -KS)
		R krr("functype");
	if (z->t != 0)
		R krr("argtype");
	pcif = (ffi_cif*)kG(xK[0]);
	if(pcif->abi != FFI_DEFAULT_ABI)
		R krr("abi");
	func = dlsym(0,y->s);
	if (!func) R orr("dlsym");
	values = malloc(sizeof(V*)*z->n);
	pvalues = malloc(sizeof(V*)*z->n);
	DO(z->n,values[i] = getvalue(kK(z)[i], pvalues+i));
	ffi_call(pcif, func, &ret, values);
	free(pvalues);
	free(values);
	R retvalue(pcif->rtype, ret);
}


#define N 6
#define FFIQ_ENTRY(i, name, def) \
	xS[i] = ss(name); kK(y)[i] = def
#define FFIQ_FUNC(i, name, nargs) FFIQ_ENTRY(i, #name, dl(name, nargs))
#define FFIQ_ENUM(i, name) FFIQ_ENTRY(i, #name, ki(name))
K1(ffi)
{
	K y = ktn(0,N); x = ktn(KS,N);
	FFIQ_ENTRY(0, "", null());
	FFIQ_FUNC(1, test,  1); 
	FFIQ_FUNC(2, cif,   2);
	FFIQ_FUNC(3, call,  3);
	/* ffi_abi enum */ 
	FFIQ_ENUM(4, FFI_SYSV);
	FFIQ_ENUM(5, FFI_DEFAULT_ABI);
	R xD(x,y);
}
