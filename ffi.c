/* -*- mode: c; c-basic-offset: 8 -*- */
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ffi.h>
#include <assert.h>
#include "k.h"

extern K dl(V*f,I n);

ZK null() {K x=ka(101);xi=0;R x;}

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
getclosure(K x, V**p);

ZV*
getvalue(K x, V**p)
{
	if (xt < 0)
		R &xg;
	if (xt == 0)
		*p = getclosure(x, p);
	else
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

Z ffi_type *
gettype(H t)
{
	switch (t) {
	case -KB:
	case -KG: R &ffi_type_uint8;
	case -KH: R &ffi_type_sint16;
	case -KM:
	case -KD:
	case -KU:
	case -KV:
	case -KT:
	case -KI: R &ffi_type_sint32;
	case -KJ: R &ffi_type_sint64;
	case -KE: R &ffi_type_float;
	case -KZ:
	case -KF: R &ffi_type_double;
	case -KS: R &ffi_type_pointer;
	case -2: R &ffi_type_void;
	}
	R &ffi_type_pointer;
}

ZV
closurefunc(ffi_cif *cif, 
	    void *resp, 
	    void **args, 
	    void *userdata) 
{
	I i, n = cif->nargs;
	K x = ktn(0,n);
	K t = kK((K)userdata)[1];
	for (i = 0; i != n; ++i) {
		switch (kC(t)[i]) {
		case 'g':
			xK[i] =  kg(**(G**)args[i]);
			break;
		case 'h':
			xK[i] =  kh(**(H**)args[i]);
			break;
		case 'i':
			xK[i] =  ki(**(I**)args[i]);
			break;
		case 's':
			xK[i] =  ks(**(S**)args[i]);
			break;
		}
	}

	K r = dot(kK((K)userdata)[0], x);
	r0(x);
	switch (r->t) {
	case -KB:
	case -KG:
		*(G*)resp = r->g;
		break;
	case -KH: 
		*(H*)resp = r->h;
		break;
	case -KM:
	case -KD:
	case -KU:
	case -KV:
	case -KT:
	case -KI:
		*(I*)resp = r->i;
		break;
	case -KJ:
		*(J*)resp = r->j;
		break;
	case -KE:
		*(E*)resp = r->e;
		break;
	case -KZ:
	case -KF:
		*(F*)resp = r->f;
		break;
	case -KS:
		*(S*)resp = r->s;
		break;
	}
	r0(r);
}

ZV*
getclosure(K x, V**p)
{
	ffi_status rc;
	ffi_closure *pcl = malloc(sizeof(ffi_closure));
	ffi_cif   *cif = malloc(sizeof(ffi_cif));
	I n = xK[1]->n;
	ffi_type  **types = malloc(sizeof(ffi_type*)*n);
	DO(n, types[i] = chartotype(kC(xK[1])[i]));
	rc = ffi_prep_cif(cif, FFI_DEFAULT_ABI, n, &ffi_type_pointer, types);
	assert(rc == FFI_OK);
	rc = ffi_prep_closure(pcl, cif, closurefunc, r1(x)); 
	assert(rc == FFI_OK);

	R pcl;
}

ZV
freeclosure(V* p)
{
	ffi_closure *pcl = p;
	free(pcl->cif->arg_types);
	free(pcl->cif);
	free(pcl);
}

Z K2(cf) /* simple call: f|(r;f),args */
{
	K r;
	Z char t[] = "kb  xhijefcs mdz uvtC";
	ffi_cif cif;
	ffi_type **types;
	ffi_status rc;
	char *f;
	H rt; /* return type */
	I i, n;  /* args count */
	void *handle, *func, **values, **pvalues;
	char ret[FFI_SIZEOF_ARG];
	if (xt == 0) {
		char *p;
		if (xn != 2 || xK[0]->t != -KC)
			R krr("type");
		if ((p=strchr(t, xK[0]->g)))
			rt = t - p;
		else
			R krr("type");
		if (xK[1]->t == -KS) {
			f = xK[1]->s;
			handle = 0;
		}
		else if (xK[1]->t == KS && xK[1]->n == 2) {
			handle = dlopen(kS(xK[1])[0], RTLD_LAZY);
			if (!handle)
				R krr(dlerror());
			f = kS(xK[1])[1];
		}
	}
	else if (xt == -KS) {
		rt = -KI;
		f = xs;
		handle = 0;
	}
	else
		R krr("type");
	if (y->t != 0)
		R krr("type");
	n = y->n;
	dlerror();    /* Clear any existing error */
	func = dlsym(handle, f);
	if (!func)
		R krr(dlerror());
	types = malloc(sizeof(ffi_type*)*n);
	values = malloc(sizeof(V*)*n);
	pvalues = calloc(n, sizeof(V*));
	for (i = 0; i != n; ++i) {
		types[i] = gettype(kK(y)[i]->t);
		values[i] = getvalue(kK(y)[i], pvalues+i);
	}
	rc = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, n,
			  gettype(rt), types);
	if (FFI_OK == rc)
		ffi_call(&cif, func, &ret, values);
	for (i = 0; i != n; ++i) {
		if (pvalues[i] && values[i] != pvalues+i)
			freeclosure(pvalues[i]);
	}
	free(pvalues);
	free(values);
	free(types);
	if (handle)
		dlclose(handle);
	if (FFI_OK != rc)
		R krr("prep");
	if (rt == 0)
		R *(K*)ret;
	if (rt == -2)
		R null();
	if (rt == -20) {
		char *rs = *(char**)ret;
		r = ktn(KC, 1+strlen(rs));
		memcpy(kG(r), rs, r->n);
		R r;
	}
	r = ka(rt);
	switch (rt) {
	case -KB:
	case -KG: R r->g = *(G*)ret, r;
	case -KH: R r->h = *(H*)ret, r;
	case -KM:
	case -KD:
	case -KU:
	case -KV:
	case -KT:
	case -KI: R r->i = *(I*)ret, r;
	case -KJ: R r->j = *(J*)ret, r;
	case -KE: R r->e = *(E*)ret, r;
	case -KZ:
	case -KF: R r->f = *(F*)ret, r;
	case -KS: R r->s = ss(*(S*)ret), r;
	}
	R krr("rtype");
}


#define N 4
#define FFIQ_ENTRY(i, name, def) \
	xS[i] = ss(name); kK(y)[i] = def
#define FFIQ_FUNC(i, name, nargs) FFIQ_ENTRY(i, #name, dl(name, nargs))
#define FFIQ_ENUM(i, name) FFIQ_ENTRY(i, #name, ki(name))
K1(ffi)
{
	K y = ktn(0,N); x = ktn(KS,N);
	FFIQ_ENTRY(0, "", null());
	FFIQ_FUNC(1, cif,   2);
	FFIQ_FUNC(2, call,  3);
	FFIQ_FUNC(3, cf,  2);
	R xD(x,y);
}


