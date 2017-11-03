import:{ 
  pwd:last -2 _ get .z.s;
  p:1 _ string ` sv first[` vs hsym[`$pwd]],x;
  system"l ",p}

import `pcre.q

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

