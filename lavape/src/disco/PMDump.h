#ifndef __PMDump
#define __PMDump

/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"


class DISCO_DLL PMDumpCLASS {

public:

  void PostMortemDump ();
  /* produces a post mortem dump and HALTs */

};

extern DISCO_DLL PMDumpCLASS PMDump;

#endif
