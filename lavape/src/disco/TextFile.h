/**********************************************************************

         #include file for class TextFile

 **********************************************************************/

#ifndef __TextFile
#define __TextFile


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "SYSTEM.h"

typedef void (*BufferProc)(char*&,unsigned&);


/**********************************************************************/
/*                                                                    */
/*                further #includes/types/constants                   */
/*              required by the private class members:                */
/*                                                                    */
/**********************************************************************/


static const unsigned BufferSize = 2048;


/**********************************************************************/
/*                                                                    */
/*                      DEFINITION OF CLASS                           */
/*                                                                    */
/**********************************************************************/

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
