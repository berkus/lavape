#ifndef __StringOutFile
#define __StringOutFile
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "OutFile.h"



class DISCO_DLL StringOutFile : public OutFile {

public:

  StringOutFile (char * const buffer,
           const unsigned bufferLength,
             const BufferProc bufferProc);

  /* Done == true: string file has been opened successfully. */


  void Flush ();

private:

  BufferProc bufferProc;
};

#endif
