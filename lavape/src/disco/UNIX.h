#ifndef __UNIX
#define __UNIX


#ifdef SOLARIS2
// boolean is defined as "enum boolean {B_FALSE,B_TRUE}"
// in /usr/include/sys/types.h
#define boolean dummyBoolean
#include <sys/types.h>
#undef boolean
#endif

#include <errno.h>

#ifdef WIN32
#include <io.h>
#define W_OK 2
#define R_OK 4
#define X_OK 4
#include <time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef __GNUC__
#define SignalHandler __hide_signal_handler
#include <signal.h>
#undef SignalHandler
#else
#include <signal.h>
#endif
#include "DateTime.h"

#include "SYSTEM.h"


//CPUTimePtr cpuTime ();

//DateTimePtr dateTime ();

typedef int TSignal;

typedef void (*SignalHandler)(TSignal,...);



/* **********************************************************************/

/*
extern "C" SignalHandler signal (TSignal sig,
                                 SignalHandler func);
*/
/* Define handler procedure for specified signal.
   Previous (or initial) value of func for the particular
   signal returned. */


extern "C" DISCO_DLL void CatchSignal (TSignal sig,
                             SignalHandler func);
/* Define handler procedure for specified signal */


extern "C" DISCO_DLL void ResetSignal (TSignal sig);


extern "C" DISCO_DLL void IgnoreSignal (TSignal sig);


#endif
