#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

         #include file for class InFile

 **********************************************************************/

#ifndef __InFile
#define __InFile


/**********************************************************************/
/*                                                                    */
/*                    #includes/types/constants                       */
/*              required by the public class members:                 */
/*                                                                    */
/**********************************************************************/


#include "TextFile.h"
#include "DString.h"


/**********************************************************************/
/*                                                                    */
/*                      DEFINITION OF CLASS                           */
/*                                                                    */
/**********************************************************************/

class DISCO_DLL InFile : public TextFile {

  friend class IOCLASS;

public:

  InFile () {}
  /* for initialization from derived classes */

  InFile (const DString& filename);
  /* Done == true: file has been opened successfully. */

  InFile (const unsigned fileref);
  /* Done == true: file has been opened successfully. */

  virtual ~InFile ();
  /* closes input file */

  char termCH;


  void Read (char& ch);
  /* Done := NOT eof */

  void Again ()
  /* the next Read... operation reads the last character once more */
  {
    if (BufPos > 0) BufPos--;
    Done = true;
  }

  void ReadString (char *s);

  void ReadString (DString& s);
  /* read string, i.e. sequence of characters not containing
     blanks nor control characters; leading blanks are ignored.
     Input is terminated by any character <= " ";
     this character is assigned to termCH. */

  void ReadLn (char *s);

  void ReadLn (DString& s);

  /* Reads the remainder of the current input line into string s.
     Leading blanks are  n o t  ignored.
     The end-of-line character is skipped. */

  void ReadInt (int& x);
  /* read string and convert to integer. Syntax:
     integer = ["+"|"-"]digit{digit}.
     Leading blanks are ignored.
     Done := "integer was read" */

  void ReadCard (unsigned& x);
  /* read string and convert to cardinal. Syntax:
     cardinal = digit{digit}.
     Leading blanks are ignored.
     Done := "cardinal was read" */

  void ReadHex (unsigned& x);
  /* read string and convert to cardinal. Syntax:
     hexCardinal = hexDigit{hexDigit}.
     Leading blanks are ignored.
     Done := "cardinal was read" */

/**********************************************************************/

protected:

  unsigned fileref;
  unsigned CharsRead;
  
  virtual void BufferEmpty ();
};

#endif
