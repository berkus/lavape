#ifndef __TextFile
#define __TextFile
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"


typedef void (*BufferProc)(char*&,unsigned&);

static const unsigned BufferSize = 2048;


class TextFile {

public:

  char *Buffer;
  unsigned BufPos;

  bool Done;

  unsigned Length;
  /* for every Read/Write... procedure,
     "Length" counts the number of transferred bytes in write direction;
     the length of the actual data (disregarding leading blanks) in
     read direction */
};
#endif
