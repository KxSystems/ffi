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
regfree:bind[`regfree;1#"X";" "];
// size of 
regoff_t:$["m"=first string .z.o;0;0i]

\d .

reg:.[.pcre.regcomp;("";0i);::]
show ":" sv ("Failed to compile";reg);
reg:.pcre.regcomp["(.*)(hello)+";.pcre.REG_EXTENDED]  // compile regex
show "Regex compiled";
tests:("This should match... hello";"This could match... hello!";"More than one hello.. hello";"No chance of a match...");
matches:.pcre.rlike[reg;] each `$tests
pmatch:(2*pcount:4)#.pcre.regoff_t
.pcre.regexec[reg;`$"123hello";pcount;pmatch;0i]
.pcre.regfree[reg]

reg:.pcre.regcomp["From:([^@]+)@([^\r]+)";2 sv sum 2 vs .pcre[`REG_EXTENDED`REG_NEWLINE]]  // compile regex
show "Regex compiled";
multiline:"From:regular.expressions@example.com\r\nFrom:exddd@43434.com\r\nFrom:7853456@exgem.com\r\n";
emailmatch:.pcre.rlike[reg;`$multiline]
.pcre.regfree[reg]
// https://eli.thegreenplace.net/2012/11/14/some-notes-on-posix-regular-expressions
//basic
reg:.pcre.regcomp[rx:"[a-z][a-z]*[0-9]\\{2,3\\}";0i]
if[.pcre.rlike[reg;s:"abc123"];show " " sv (rx;"matches(basic)";s)];
.pcre.regfree reg
// extended
reg:.pcre.regcomp[rx:"[a-z]+[0-9]{2,3}";.pcre.REG_EXTENDED]
if[.pcre.rlike[reg;s:"abc123"];show " " sv (rx;"matches(extended)";s)];
.pcre.regfree reg

// back reference
reg:.pcre.regcomp[rx:"([a-z]+)_\\1";0i]
pmatch:(2*pcount:2)#.pcre.regoff_t;
if[0i~.pcre.regexec[reg;s:"abc_abc";pcount;pmatch;0i];show " " sv (rx;"matches(extended)";s;" with groups "),sublist[;s]@/:2 cut pmatch];
.pcre.regfree reg

