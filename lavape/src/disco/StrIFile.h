#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

         #include file for class StringInFile

 **********************************************************************/

#ifndef __StringInFile
#define __StringInFile


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "InFile.h"


/**********************************************************************/
/*                                                                    */
/*                      DEFINITION OF CLASS                           */
/*                                                                    */
/**********************************************************************/


class DISCO_DLL StringInFile : public InFile {

public:

  StringInFile (char * const buffer,
          const unsigned bufferLength,
            const BufferProc bufferProc);

  /* Done == true: string file has been opened successfully. */


/**********************************************************************/

private:

  BufferProc bufferProc;

  void BufferEmpty ();
};

#endif
