//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            File Description                          //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* @file rmath.q
* @overview
* This file shows how to use ffi interface to utilize functions in Rmath library.
\

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Libraries                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

\l ffi.q

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Functions                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* Use [Rmath](https://cran.r-project.org/doc/manuals/r-release/R-admin.html#The-standalone-Rmath-library).
* For linux you can install libRmath.so by:
* - Ubuntu:         $ sudo apt-get install r-mathlib
* - CentOS/RedHat:  $ sudo yum install libRmath
\
.rm.set_seed:.ffi.bind[`libRmath.so`set_seed; "ii"; " "];
.rm.norm_rand:.ffi.bind[`libRmath.so`norm_rand; ""; "f"];
.rm.unif_rand:.ffi.bind[`libRmath.so`unif_rand; ""; "f"];
.rm.exp_rand:.ffi.bind[`libRmath.so`exp_rand; ""; "f"];
.rm.rbeta:.ffi.bind[`libRmath.so`rbeta; "ff"; "f"];
.rm.dbeta:.ffi.bind[`libRmath.so`dbeta; "fffi"; "f"];
.rm.qbeta:.ffi.bind[`libRmath.so`qbeta; "fffii"; "f"];
.rm.rnorm:.ffi.bind[`libRmath.so`rnorm; "jff"; "f"];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                             Play Around                              //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// Set random seed
.rm.set_seed (123i; 456i; ::);

// Generate random number from normal distribution
normrand:.rm.norm_rand enlist (::);
normrand=-0.29349741761200659

// Generate random number from uniform distribution
unifrand:.rm.unif_rand enlist (::);
unifrand=0.36904753613496366

// Generate random number from exponential distribution
exprand:.rm.exp_rand enlist (::);
exprand=0.18392036926102798

// Get quantile of 0.1 under (alpha, beta) = (1.5, 0.3), non-centrality=1, lower.tail=false
beta_quantile:.rm.qbeta (0.1; 1.5; 0.3; 1i; 0i; ::);
beta_quantile=0.47839808109594339

// Get density of 0.5 under (alpha, beta) = (0.1, 5.0), non-centrality=0
beta_density:.rm.dbeta (0.5; 0.1; 5.0; 0; ::);
beta_density=0.014267678091051986

// Generate random number from beta distribution with (alpha, beta) = (0.1, 5.0)
betarand:.rm.rbeta (0.1; 5.0; ::);
betarand within 0 1

// Set random seed
.rm.set_seed (456i; 789i; ::);

// generate 100K N(0,1) random numbers
norms:`float$();
do[100000; gen:.rm.rnorm (1; 0f; 1f; ::); if[-9h ~ type gen; norms,: gen]];

// verify that avg and dev are 0 and 1
show (avg; dev) @\: norms;
