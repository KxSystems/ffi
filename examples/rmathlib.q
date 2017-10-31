\l ffi.q
\d .rm
// installation:
// 
// Ubuntu 
// sudo apt-get install r-mathlib
// Also see C bindings on https://github.com/rwinston/kdb-rmathlib
lib:{.ffi.ext[`libRmath],x}
//TODO: check nargs > count a
//TODO: check a~.Q.t abs type each count[a]#args
bind:{[f;a;r] '[.ffi.bind[lib f;a;r],[;::]::;enlist]}



set_seed:bind[`set_seed;"ii";" "];
//get_seed:bind[`get_seed;"II";" "];
//Rlog1p:bind[`Rlog1p;1#"f";"f"];
R_pow:bind[`R_pow;"ff";"f"];
R_pow_di:bind[`R_pow_di;"fi";"f"];
norm_rand:bind[`norm_rand;"";"f"];
unif_rand:bind[`unif_rand;"";"f"];
exp_rand:bind[`exp_rand;"";"f"];
dnorm:bind[`dnorm4;"fffi";"f"];
pnorm:bind[`pnorm5;"fffii";"f"];
qnorm:bind[`qnorm5;"fffii";"f"];
rnorm:bind[`rnorm;"ff";"f"];
dunif:bind[`dunif;"fffi";"f"];
punif:bind[`punif;"fffii";"f"];
qunif:bind[`qunif;"fffii";"f"];
runif:bind[`runif;"ff";"f"];
dgamma:bind[`dgamma;"fffi";"f"];
pgamma:bind[`pgamma;"fffii";"f"];
qgamma:bind[`qgamma;"fffii";"f"];
rgamma:bind[`rgamma;"ff";"f"];
log1pmx:bind[`log1pmx;1#"f";"f"];
lgamma1p:bind[`lgamma1p;1#"f";"f"];
logspace_add:bind[`logspace_add;"ff";"f"];
logspace_sub:bind[`logspace_sub;"ff";"f"];
logspace_sum:bind[`logspace_sum;" i";"f"];
dbeta:bind[`dbeta;"fffi";"f"];
pbeta:bind[`pbeta;"fffii";"f"];
qbeta:bind[`qbeta;"fffii";"f"];
rbeta:bind[`rbeta;"ff";"f"];
dlnorm:bind[`dlnorm;"fffi";"f"];
plnorm:bind[`plnorm;"fffii";"f"];
qlnorm:bind[`qlnorm;"fffii";"f"];
rlnorm:bind[`rlnorm;"ff";"f"];
dchisq:bind[`dchisq;"ffi";"f"];
pchisq:bind[`pchisq;"ffii";"f"];
qchisq:bind[`qchisq;"ffii";"f"];
rchisq:bind[`rchisq;1#"f";"f"];
dnchisq:bind[`dnchisq;"fffi";"f"];
pnchisq:bind[`pnchisq;"fffii";"f"];
qnchisq:bind[`qnchisq;"fffii";"f"];
rnchisq:bind[`rnchisq;"ff";"f"];
df:bind[`df;"fffi";"f"];
pf:bind[`pf;"fffii";"f"];
qf:bind[`qf;"fffii";"f"];
rf:bind[`rf;"ff";"f"];
dt:bind[`dt;"ffi";"f"];
pt:bind[`pt;"ffii";"f"];
qt:bind[`qt;"ffii";"f"];
rt:bind[`rt;1#"f";"f"];
dbinom_raw:bind[`dbinom_raw;"     ";"f"];
dbinom:bind[`dbinom;"fffi";"f"];
pbinom:bind[`pbinom;"fffii";"f"];
qbinom:bind[`qbinom;"fffii";"f"];
rbinom:bind[`rbinom;"ff";"f"];
dcauchy:bind[`dcauchy;"fffi";"f"];
pcauchy:bind[`pcauchy;"fffii";"f"];
qcauchy:bind[`qcauchy;"fffii";"f"];
rcauchy:bind[`rcauchy;"ff";"f"];
dexp:bind[`dexp;"ffi";"f"];
pexp:bind[`pexp;"ffii";"f"];
qexp:bind[`qexp;"ffii";"f"];
rexp:bind[`rexp;1#"f";"f"];
dgeom:bind[`dgeom;"ffi";"f"];
pgeom:bind[`pgeom;"ffii";"f"];
qgeom:bind[`qgeom;"ffii";"f"];
rgeom:bind[`rgeom;1#"f";"f"];
dhyper:bind[`dhyper;"ffffi";"f"];
phyper:bind[`phyper;"ffffii";"f"];
qhyper:bind[`qhyper;"ffffii";"f"];
rhyper:bind[`rhyper;"fff";"f"];
dnbinom:bind[`dnbinom;"fffi";"f"];
pnbinom:bind[`pnbinom;"fffii";"f"];
qnbinom:bind[`qnbinom;"fffii";"f"];
rnbinom:bind[`rnbinom;"ff";"f"];
dnbinom_mu:bind[`dnbinom_mu;"fffi";"f"];
pnbinom_mu:bind[`pnbinom_mu;"fffii";"f"];
qnbinom_mu:bind[`qnbinom_mu;"fffii";"f"];
rnbinom_mu:bind[`rnbinom_mu;"ff";"f"];
dpois_raw :bind[`dpois_raw ;"ffi";"f"];
dpois:bind[`dpois;"ffi";"f"];
ppois:bind[`ppois;"ffii";"f"];
qpois:bind[`qpois;"ffii";"f"];
rpois:bind[`rpois;1#"f";"f"];
dweibull:bind[`dweibull;"fffi";"f"];
pweibull:bind[`pweibull;"fffii";"f"];
qweibull:bind[`qweibull;"fffii";"f"];
rweibull:bind[`rweibull;"ff";"f"];
dlogis:bind[`dlogis;"fffi";"f"];
plogis:bind[`plogis;"fffii";"f"];
qlogis:bind[`qlogis;"fffii";"f"];
rlogis:bind[`rlogis;"ff";"f"];
dnbeta:bind[`dnbeta;"ffffi";"f"];
pnbeta:bind[`pnbeta;"ffffii";"f"];
qnbeta:bind[`qnbeta;"ffffii";"f"];
//rnbeta:bind[`rnbeta;"fff";"f"];
dnf:bind[`dnf;"ffffi";"f"];
pnf:bind[`pnf;"ffffii";"f"];
qnf:bind[`qnf;"ffffii";"f"];
dnt:bind[`dnt;"fffi";"f"];
pnt:bind[`pnt;"fffii";"f"];
qnt:bind[`qnt;"fffii";"f"];
ptukey:bind[`ptukey;"ffffii";"f"];
qtukey:bind[`qtukey;"ffffii";"f"];
dwilcox:bind[`dwilcox;"fffi";"f"];
pwilcox:bind[`pwilcox;"fffii";"f"];
qwilcox:bind[`qwilcox;"fffii";"f"];
rwilcox:bind[`rwilcox;"ff";"f"];
dsignrank:bind[`dsignrank;"ffi";"f"];
psignrank:bind[`psignrank;"ffii";"f"];
qsignrank:bind[`qsignrank;"ffii";"f"];
rsignrank:bind[`rsignrank;1#"f";"f"];
gammafn:bind[`gammafn;1#"f";"f"];
lgammafn:bind[`lgammafn;1#"f";"f"];
lgammafn_sign:bind[`lgammafn_sign;"fI";"f"];
psigamma:bind[`psigamma;"ff";"f"];
digamma:bind[`digamma;1#"f";"f"];
trigamma:bind[`trigamma;1#"f";"f"];
tetragamma:bind[`tetragamma;1#"f";"f"];
pentagamma:bind[`pentagamma;1#"f";"f"];
beta:bind[`beta;"ff";"f"];
lbeta:bind[`lbeta;"ff";"f"];
choose:bind[`choose;"ff";"f"];
lchoose:bind[`lchoose;"ff";"f"];
bessel_i:bind[`bessel_i;"fff";"f"];
bessel_j:bind[`bessel_j;"ff";"f"];
bessel_k:bind[`bessel_k;"fff";"f"];
bessel_y:bind[`bessel_y;"ff";"f"];
bessel_i_ex:bind[`bessel_i_ex;"fffF";"f"];
bessel_j_ex:bind[`bessel_j_ex;"ffF";"f"];
bessel_k_ex:bind[`bessel_k_ex;"fffF";"f"];
bessel_y_ex:bind[`bessel_y_ex;"ffF";"f"];
hypot:bind[`hypot;"ff";"f"];
//pythag:bind[`pythag;"ff";"f"];
fmax2:bind[`fmax2;"ff";"f"];
fmin2:bind[`fmin2;"ff";"f"];
sign:bind[`sign;1#"f";"f"];
fprec:bind[`fprec;"ff";"f"];
fround:bind[`fround;"ff";"f"];
fsign:bind[`fsign;"ff";"f"];
ftrunc:bind[`ftrunc;1#"f";"f"];
cospi:bind[`cospi;1#"f";"f"];
sinpi:bind[`sinpi;1#"f";"f"];
tanpi:bind[`tanpi;1#"f";"f"];
logspace_add:bind[`logspace_add;"  ";"f"];
logspace_sub:bind[`logspace_sub;"  ";"f"];
// sketch of how to generate function binding above
// /Library/Frameworks/R.framework/Headers/Rmath.h
// funcs:except[;enlist ()]@{$[x like"double*);";x;()]}each read0 hsym `$"/Library/Frameworks/R.framework/Resources/include/Rmath.h"
// funcs:ssr[;"\t";" "] each funcs
// tm:("double";"int";"void";"double*";"double *";"int*")!"fi FFI";
// ftable:{rtype:tm (p:x?" ")#x;x:trim p _ x;fname:(p:x?"(")#x;x:-2 _ 1 _ p _ x;`rtype`func`rest!(rtype;fname;tm@/:", " vs x)}each funcs
// 1 "\n" sv {x[`func],":bind[`",x[`func],";\"",x[`rest],"\";\"",x[`rtype],"\"];"}each ftable

\d .

.rm.set_seed[123i;456i];
-0.29349741761200659=.rm.norm_rand[]
0.47839808109594339=.rm.qbeta[.10;1.5;.3;1i;0i]
.rm.set_seed[.z.t;77911i]        // set random seed
\ts do[100000;r,:.rm.rnorm[0f;1f]]   // generate 100K N(0,1) random numbers
0 1~ceiling (avg;dev)@\:r     // verify that avg and dev are 0 and 1
