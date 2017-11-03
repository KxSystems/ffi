\l ffi.q

.ffi.loadlib .ffi.ext `libpcreposix
\d .pcre
lib:{x}
bind:{[f;a;r] '[.ffi.bind[lib f;a;r],[;::]::;enlist]}

// regcomp cflags
REG_BASIC:   0i
REG_EXTENDED:1i
REG_ICASE:   2i
REG_NEWLINE: 4i
REG_NOSUB:   8i
// regexec eflags
REG_NOTBOL:  1i
REG_NOTEOL:  2i
REG_STARTEND:4i


// https://linux.die.net/man/3/pcreposix
//int regcomp(regex_t *preg, const char *pattern,int cflags);
regcomp0:bind[`regcomp;"Xsi";"i"];
regcomp:{[pattern;cflags] reg:64#0x0;res:regcomp0[reg;`$pattern;cflags];$[res=0i;reg;'regerror[reg;res]]}
// int regexec(regex_t *preg, const char *string,size_t nmatch, regmatch_t pmatch[], int eflags);
regexec:bind[`regexec;"XCjIi";"i"];
// return bool: similar to .q.like
rlike:{[r;s]regexec[r;s;0;0#0i;0i]=0i}
// size_t regerror(int errcode, const regex_t *preg,char *errbuf, size_t errbuf_size);
regerror0:bind[`regerror;"iXCj";"j"];
regerror:{[reg;code]buf:100#"";l:regerror0[code;reg;buf;count buf];(l-1)#buf}
// void regfree(regex_t *preg);
regfree:bind[`regfree;"X";" "];
// size of 
regoff_t:$["m"=first string .z.o;0;0i]

\d .
