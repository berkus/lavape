#ifndef __StringInFile
#define __StringInFile
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "InFile.h"



class DISCO_DLL StringInFile : public InFile {

public:

  StringInFile (char * const buffer,
          const unsigned bufferLength,
            const BufferProc bufferProc);

  /* Done == true: string file has been opened successfully. */


/**********************************************************************/

private:

  BufferProc bufferProc;

//  void BufferEmpty ();
};

#endif
