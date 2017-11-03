# Rmath library

Bindings to [Rmath](https://cran.r-project.org/doc/manuals/r-release/R-admin.html#The-standalone-Rmath-library) using FFI for kdb+.

`Rmath` provides the routines supporting the distribution and special(e.g. Bessel, beta and gamma functions) functions in `R`. 

## Requirements

Using this library requires standalone Rmath library to be installed as well as FFI for kdb+.

### Ubuntu 
```bash
sudo apt-get install r-mathlib
```

## Examples

Generate 100K numbers from normal distribution
```
q)do[100000;r,:.rm.rnorm[0f;1f]]   // generate 100K N(0,1) random numbers
q)(avg;dev)@\:r           // verify that avg and dev are 0 and 1
0.0009293088 1.002748
```

