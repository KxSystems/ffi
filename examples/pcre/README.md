# PCRE library

Bindings to [pcre(posix variant)](https://www.pcre.org/original/doc/html/pcreposix.html) using FFI for kdb+.

`pcreposix` is a set of functions provides a POSIX-style API for the PCRE regular expression 8-bit library.  

## Requirements
FFI for kdb+ is required for this library. `pcre` is normally available on modern Linux distributions and macOS

As any standard pcre POSIX has some [quirks](https://eli.thegreenplace.net/2012/11/14/some-notes-on-posix-regular-expressions) and differences between platforms([Linux](https://linux.die.net/man/3/pcreposix)) which this library is trying to resolve.

## Examples

Match multiline email
```
reg:.pcre.regcomp["From:([^@]+)@([^\r]+)";2 sv sum 2 vs .pcre[`REG_EXTENDED`REG_NEWLINE]]  // compile regex
show "Regex compiled";
multiline:"From:regular.expressions@example.com\r\nFrom:exddd@43434.com\r\nFrom:7853456@exgem.com\r\n";
emailmatch:.pcre.rlike[reg;`$multiline]
.pcre.regfree[reg]
```





