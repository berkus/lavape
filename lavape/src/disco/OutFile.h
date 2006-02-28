#ifndef __OutFile
#define __OutFile
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"

#include "TextFile.h"
#include "DString.h"



extern DISCO_DLL bool openInAppendMode;

class DISCO_DLL OutFile : public TextFile {

  friend class IOCLASS;
  
public:

  OutFile () {}
  /* default constructor must exist for derived classes */

  OutFile (const char *filename);
  /* Done == true: file has been opened successfully. */

  OutFile (const unsigned fileref);
  /* Done == true: file has been opened successfully. */

  virtual ~OutFile ();
  /* closes output file */


  void WriteLn ();

  void Write (const char ch);

  void WriteString (const char * const s);

  void WriteString (const DString& s)
  { WriteString(s.c); }

  void WriteInt (const long int x,const unsigned d);
  /* write integer x with (at least) d characters on file "out".
     If n is greater than the number of digits needed,
     blanks are added preceding the number */

  void WriteCard (const long unsigned x,
        const unsigned d)
  { WriteInt(x,d); }

  void WriteHex (const unsigned char * const s,
     const unsigned length);

  void WriteHex (const DString& s);
  /* every byte is represented by 2 hexadecimal digits */

  virtual void BigBuffer ();
  /* WriteLn causes the current output buffer to be flushed
     immediately by default. After calling BigBuffer the
     output buffer of the current output file is flushed only 
     if it is full or if CloseOutput is called.

     Repeated invocation of BigBuffer switches back and
     forth beween these two modes. */

  virtual void Flush ();
  /* flush without inserting a newline */

/**********************************************************************/

protected:

  QFile file;
  unsigned Size;
  bool bigBuffer;
  
};

#endif
