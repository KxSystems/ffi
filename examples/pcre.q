//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            File Description                          //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* @file pcre.q
* @overview
* This file shows how to use ffi interface to utilize functions in PCRE library.
* @note
* Reference: https://linux.die.net/man/3/pcreposix
\

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Libraries                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

\l ffi.q

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Global Variable                           //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// regcomp cflags
.pcre.REG_BASIC:   0000b;
.pcre.REG_EXTENDED:0001b;
.pcre.REG_ICASE:   0010b;
.pcre.REG_NEWLINE: 0100b;
.pcre.REG_NOSUB:   1000b;
// regexec eflags
.pcre.REG_NOTBOL:  0001b;
.pcre.REG_NOTEOL:  0010b;
.pcre.REG_STARTEND:0100b;

// size of 
.pcre.regoff_t:$["m"=.ffi.os[]; 0; 0i];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Functions                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// int regcomp(regex_t *preg, const char *pattern,int cflags);
.pcre.regcomp_impl:.ffi.bind[`regcomp;"Xsi";"i"];

// int regexec(regex_t *preg, const char *string,size_t nmatch, regmatch_t pmatch[], int eflags);
.pcre.regexec_impl:.ffi.bind[`regexec;"XCjIi";"i"];

// size_t regerror(int errcode, const regex_t *preg,char *errbuf, size_t errbuf_size);
.pcre.regerror_impl:.ffi.bind[`regerror;"iXCj";"j"];

// void regfree(regex_t *preg);
.pcre.regfree_impl:.ffi.bind[`regfree;"X";" "];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Wrapper Functions                         //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// @brief Compile regex.
// @param pattern {string}: Regex pattern.
// @param cflags {int}: Bit field of compile flags.
// @return {list of byte}: Regex object.
.pcre.regcomp:{[pattern;cflags]
  regex:64#0x0;
  cflags:2 sv $[1h ~ type cflags; cflags; (|/) cflags];
  error:.pcre.regcomp_impl (regex; `$pattern; cflags; ::);
  $[null error; regex; '.pcre.regerror[regex; error]]
 };

// @brief Execute regex.
// @param regex {list of byte}: Regex object.
// @param text {string}: Text to match the regex pattern.
// @return
// - bool
.pcre.regexec:{[regex; text] null .pcre.regexec_impl (regex; text; 0; 0#0i; 0i; ::)};

// @brief Decode regex error code to readable error message.
// @param regex {list of byte}: Regex object.
// @error_code {int}: Error code.
// @return
// - string: Error message for the error code.
.pcre.regerror:{[regex;error_code]
  buffer:100#"";
  length:.pcre.regerror_impl (error_code; regex; buffer; count buffer; ::);
  (length-1)#buffer
 };

// @brief Free regex.
.pcre.regfree:{[regex]
  .pcre.regfree_impl (regex; ::)
 };

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                             Play Around                              //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

//%% Simple Pattern %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

// https://eli.thegreenplace.net/2012/11/14/some-notes-on-posix-regular-expressions

// basic //---------------------------/

// Compile regex
regex:.pcre.regcomp[pattern:"[a-z][a-z]*[0-9]\\{2,3\\}"; .pcre.REG_BASIC];
if[.pcre.regexec[regex; s:"abc123"]; show " " sv (pattern; "matches(basic)"; s)];

// Free regex
.pcre.regfree regex;

// extended //------------------------/

// Compile regex
regex:.pcre.regcomp[pattern:"[a-z]+[0-9]{2,3}"; .pcre.REG_EXTENDED];
if[.pcre.regexec[regex;s:"abc123"];show " " sv (pattern; "matches(extended)";s)];

// Free regex
.pcre.regfree regex;

// pmatch //--------------------------/

// compile regex
regex:.pcre.regcomp["(.*)(hello)+"; .pcre.REG_EXTENDED];

texts:("This should match... hello"; "This could match... hello!"; "More than one hello.. hello"; "No chance of a match...");
matches:.pcre.regexec[regex;] each texts;
show matches;

pmatch:(2*pcount:4)#.pcre.regoff_t;
.pcre.regexec_impl (regex; `$"123hello"; pcount; pmatch; 0i; ::)

// Free regex
.pcre.regfree regex;

//%% Email Address %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

// Compile regex
regex:.pcre.regcomp["From:([^@]+)@([^\r]+)"; (.pcre.REG_EXTENDED; .pcre.REG_NEWLINE)];
multiline:"From:regular.expressions@example.com\r\nFrom:exddd@43434.com\r\nFrom:7853456@exgem.com\r\n";
emailmatch:.pcre.regexec[regex;`$multiline]

// Free regex
.pcre.regfree regex;

//%% Back Reference %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

// Compile regex
regex:.pcre.regcomp[pattern:"([a-z]+)_\\1";.pcre.REG_EXTENDED];
pmatch:(2*pcount:2)#.pcre.regoff_t;
if[null .pcre.regexec_impl (regex; s:"abc_abc"; pcount; pmatch;0i; ::); show " " sv (pattern; "matches(extended)"; s; "with groups "), sublist[;s]@/:2 cut pmatch];

// Free regex
.pcre.regfree regex;

//%% Compile tracking %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

// Compile regex
regex:.pcre.regcomp[rx:"^(A+)*B";.pcre.REG_EXTENDED];
\ts .pcre.regexec[regex; s:(1000#"A"),"C"]

// Free regex
.pcre.regfree regex;

