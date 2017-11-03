import:{ pwd:last -2 _ get .z.s;p:1 _ string ` sv first[` vs hsym[`$pwd]],x;system"l ",p}

import `rmath.q


.rm.set_seed[123i;456i];
.rm.norm_rand[]=-0.29349741761200659
.rm.unif_rand[]=0.36904753613496366
.rm.exp_rand[]=0.18392036926102798
0.47839808109594339=.rm.qbeta[.10;1.5;.3;1i;0i]
.rm.set_seed[456i;77911i]        // set random seed
\ts do[100000;r,:.rm.rnorm[0f;1f]]   // generate 100K N(0,1) random numbers
(avg;dev)@\:r           // verify that avg and dev are 0 and 1
.rm.dbeta[0.5;0.1;5.0;0]=0.014267678091051986
.rm.rbeta[0.1;5.0] within 0 1