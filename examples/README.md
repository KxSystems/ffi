# ffikdb Example

Two examples are provided here:
- Rmath library (`libRmath`)
- PCRE POSIX library (`libpcreposix`)

## Rmath library

Bindings to [Rmath](https://cran.r-project.org/doc/manuals/r-release/R-admin.html#The-standalone-Rmath-library) using FFI for kdb+.

`Rmath` provides the routines supporting the distribution and special(e.g. Bessel, beta and gamma functions) functions in `R`. 

### Requirements

Using this library requires standalone Rmath library to be installed as well as FFI for kdb+.

#### Ubuntu

```bash

$ sudo apt-get install r-mathlib

```

#### CentOS/RedHat

```bash

$ yum install libRmath

```

## Example

In this example, 100K numbers from normal distribution are generated.

```q

q).rm.rnorm:.ffi.bind[`libRmath.so`rnorm; "jff"; "f"]
q)// Different way of calling
q)// .ffi.callFunction[("f"; `libRmath.so`rnorm)] (1; 0f; 1f; ::)
q).rm.rnorm (1; 0f; 1f; ::)
-1.269737
q)norms:()
q)// generate 100K N(0,1) random numbers
q)do[100000; norms,:.rm.rnorm (1; 0f; 1f; ::)]
q)// verify that avg and dev are 0 and 1
q)(avg; dev) @\: norms
-0.005513158 0.9977446

```

## PCRE library

Bindings to [pcre(posix variant)](https://www.pcre.org/original/doc/html/pcreposix.html) using FFI for kdb+.

`pcreposix` is a set of functions provides a POSIX-style API for the PCRE regular expression 8-bit library.  

**WARNING** Be aware of complex and potentially catastrophic regular expressions as they might exhibit [exponential run time](https://www.regular-expressions.info/catastrophic.html). This can lead to real [outages](http://stackstatus.net/post/147710624694/outage-postmortem-july-20-2016).

## Requirements

FFI for kdb+ is required for this library. `pcre` is normally available on modern Linux distributions and macOS

As any standard pcre POSIX has some [quirks](https://eli.thegreenplace.net/2012/11/14/some-notes-on-posix-regular-expressions) and differences between platforms([Linux](https://linux.die.net/man/3/pcreposix)) which this library is trying to resolve.

## Examples

Match multiline email addresses

```q

q).pcre.regcomp:.ffi.bind[`libpcreposix.so`regcomp; "Xsi"; "i"]
q).pcre.regexec:.ffi.bind[`libpcreposix.so`regexec;"XCjIi";"i"]
q).pcre.regfree:.ffi.bind[`libpcreposix.so`regfree;"X";" "]
q)REG_EXTENDED:001b
q)REG_NEWLINE: 100b
q)regex:64#0x0
q).pcre.regcomp (regex; `$"From:([^@]+)@([^\r]+)"; REG_EXTENDED | REG_NEWLINE; ::)
q)emailaddresses:"From:regular.expressions@example.com\r\nFrom:exddd@43434.com\r\nFrom:7853456@exgem.com\r\n"
q)match:null .pcre.regexec (regex; `$emailaddresses; 0; 0#0i; 0i; ::)
q)match
1b
q).pcre.regfree (regex; ::)

```
