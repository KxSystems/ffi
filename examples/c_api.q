//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            File Description                          //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* @file c_api.q
* @overview
* This file shows how to use ffi interface to utilize functions in C API for q.
*  This example alone does not make an example. You need to launch another q process connecting
*  to this process and communicate.
* @note
* Using `sd1` from q seems to break event loop and q gets unable to receive user input...
\
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Initial Setting                           //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// If port is not set, set 5000
if[not system "p"; system "p 5000"];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Libraries                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

\l ffi.q

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                            Load Functions                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// Converts a q date to a yyyymmdd integer
.capi.dj:.ffi.bind[`dj; "i"; "i"];

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                           Global Variable                            //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* Dictionary storing count of remote call regardless of asynchronous or synchronous.
* # Key
* int   | ID denoting a client handle
* # Value
* long  | Count of remote calls from each client
\
CALL_COUNT:()!();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                           Define Functions                           //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/
* @brief Increment call count from a client and send asynchronous message to the client.
* @param fd: File descriptor (handle)
\
callback:{[fd]
  CALL_COUNT[.z.w]+:1;
  neg[fd] "show `Ack";
  0
 };

/
* @brief Set call count of a client 0 when the client opens a handfle to this process.
* @param clien: Handle of the client.
\
.z.po:{[client]
  -1 "Oh no! Someone is trying to intrude this process!";
  CALL_COUNT[client]:0;
  // Hook `callback` to IPC event loop
  .ffi.callFunction[`sd1] (client; (callback; enlist "i"); ::);
 };

/
* @brief Display call count from the client whoc close a handle.
\
.z.pc:{[client]
  -1 "Thanks God! He is gone after ", string[CALL_COUNT client], " knocks!!";
  .ffi.callFunction[`sd0] (client; ::);
  exit 0;
 };

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                             Play Around                              //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

// Wait until another process connects to this process.
.z.ts:{
  $[not count key .z.W;
    -1 string[.capi.dj (.z.d; ::)], ": What a slow day...";
    -1 "... (silence)"
  ];
 };

// Set timer 1000 milliseconds
\t 1000
