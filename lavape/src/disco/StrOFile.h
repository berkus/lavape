#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

         #include file for class StringOutFile

 **********************************************************************/

#ifndef __StringOutFile
#define __StringOutFile


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "OutFile.h"


/**********************************************************************/
/*                                                                    */
/*                      DEFINITION OF CLASS                           */
/*                                                                    */
/**********************************************************************/


class DISCO_DLL StringOutFile : public OutFile {

public:

  StringOutFile (char * const buffer,
           const unsigned bufferLength,
             const BufferProc bufferProc);

  /* Done == true: string file has been opened successfully. */


  void Flush ();


/**********************************************************************/

private:

  BufferProc bufferProc;
};

#endif
