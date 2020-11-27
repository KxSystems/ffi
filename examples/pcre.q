//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            File Description                          //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* @file
* pcre.q
* @overview
* This file shows how to use ffi interface to utilize functions in PCRE POSIX library.
\

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Libraries                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

\l ffi.q

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Functions                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// https://linux.die.net/man/3/pcreposix

//int regcomp(regex_t *preg, const char *pattern,int cflags);
.pcre.regcomp:.ffi.bind[`libpcreposix.so`regcomp; "Xsi"; "i"];

// int regexec(regex_t *preg, const char *string,size_t nmatch, regmatch_t pmatch[], int eflags);
.pcre.regexec:.ffi.bind[`libpcreposix.so`regexec; "XCjIi"; "i"];

// size_t regerror(int errcode, const regex_t *preg,char *errbuf, size_t errbuf_size);
.pcre.regerror:.ffi.bind[`libpcreposix.so`regerror; "iXCj"; "j"];

// void regfree(regex_t *preg);
.pcre.regfree:.ffi.bind[`libpcreposix.so`regfree; "X"; " "];

// size of 
regoff_t:$["m"=.ffi.os[]; 0; 0i];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Global Variables                          //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// regcomp cflags
REG_BASIC:   0000b;
REG_EXTENDED:0001b;
REG_ICASE:   0010b;
REG_NEWLINE: 0100b;
REG_NOSUB:   1000b;

// regexec eflags
REG_DEFAULT: 000b;
REG_NOTBOL:  001b;
REG_NOTEOL:  010b;
REG_STARTEND:100b;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                             Play Around                              //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// Single Matching //------------------------------/

// Holder of regex
regex:64#0x0;
// Compile regex
.pcre.regcomp (regex; `$"(.*)(hello)+"; REG_EXTENDED; ::);
// Test cases
hellos:("This should match... hello"; "This could match... hello!"; "More than one hello.. hello"; "No chance of a match...");
// Examine
match:null .pcre.regexec each {[rgx;txt] (rgx; txt; 0; 0#0i; REG_DEFAULT; ::)}[regex] each hellos;
show match;

// Use pcount and pmatch
pmatch:(2*pcount:4)#regoff_t;
match: null .pcre.regexec (regex; "123hello"; pcount; pmatch; REG_DEFAULT; ::);
show match;
show pcount;
show pmatch;

// Free regex
.pcre.regfree (regex; ::);

// Multiple Matching //----------------------------/

// Holder of regex
regex:64#0x0;
// Compile regex
.pcre.regcomp (regex; `$"From:([^@]+)@([^\r]+)"; REG_EXTENDED | REG_NEWLINE; ::);
// Test case
emailaddresses:"From:regular.expressions@example.com\r\nFrom:exddd@43434.com\r\nFrom:7853456@exgem.com\r\n";
// Examine
match:null .pcre.regexec (regex; emailaddresses; 0; 0#0i; REG_DEFAULT; ::);
show match;
// Free regex
.pcre.regfree (regex; ::);

// https://eli.thegreenplace.net/2012/11/14/some-notes-on-posix-regular-expressions

// Basic Regex //----------------------------------/

// Holder of regex
regex:64#0x0;
// Compile regex
.pcre.regcomp (regex; `$pattern:"[a-z][a-z]*[0-9]\\{2,3\\}"; REG_BASIC; ::);
// Examine
if[null .pcre.regexec (regex; s:"abc123"; 0; 0#0i; REG_DEFAULT; ::); show " " sv (pattern; "matches(basic)"; s)];
// Free regex
.pcre.regfree (regex; ::);

// Extended Regex //-------------------------------/

// Holder of regex
regex:64#0x0;
// Compile regex
.pcre.regcomp (regex; `$pattern:"[a-z]+[0-9]{2,3}"; REG_EXTENDED; ::);
// Examine
if[null .pcre.regexec (regex; s:"abc123"; 0; 0#0i; REG_DEFAULT; ::); show " " sv (pattern; "matches(extended)"; s)];
// Free regex
.pcre.regfree (regex; ::);

// Back Reference //-------------------------------/

// Holder of regex
regex:64#0x0;
// Compile regex
.pcre.regcomp (regex; `$pattern:"([a-z]+)_\\1"; REG_EXTENDED; ::);
// Use pmatch
pmatch:(2*pcount:2)#regoff_t;
// Examine
if[null .pcre.regexec (regex; s:"abc_abc"; pcount; pmatch; REG_DEFAULT; ::); show " " sv (pattern; "matches(extended)"; s; " with groups "), sublist[;s] @/: 2 cut pmatch];
// Free regex
.pcre.regfree (regex; ::);

// Complex Tracking //-----------------------------/

// Holder of regex
regex:64#0x0;
// Compile regex
.pcre.regcomp (regex; `$pattern:"^(A+)*B"; REG_EXTENDED; ::);
// Examine
\ts match:null .pcre.regexec (regex; s:(1000#"A"), "C"; 0; 0#0i; REG_DEFAULT; ::);
show match;
// Free
.pcre.regfree (regex; ::);

