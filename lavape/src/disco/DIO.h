#ifndef __IOCLASS
#define __IOCLASS
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "InFile.h"
#include "OutFile.h"
#include "DString.h"



class DISCO_DLL IOCLASS {

public:
  void INIT ();
  void EXIT ();
  
  bool Done;
  
  unsigned Length;
  /* for every IO.Read.../Write... procedure,
     "Length" counts the number of transferred bytes in write direction;
     the length of the actual data (disregarding leading blanks) in
     read direction */
     
  char termCH;

  void OpenInput (const char *filename);
  /* Done == true: file has been opened successfully. */

  void OpenInput (const DString& filename)
  { OpenInput(filename.c); }

  void Append ();
  /* the next [F]OpenOutput is performed in append mode */

  void OpenOutput (const char *filename);
  /* Done == true: file has been opened successfully. */

  void OpenOutput (const DString& filename)
  { OpenOutput(filename.c); }

  void CloseInput ();
  /* closes input file; returns input to terminal */

  void CloseOutput ();
  /* closes output file; returns output to terminal */

/**********************************************************************/
  /* additionally: */

  void FOpenInput (InFile **f, const char *filename);

  void FOpenInput (InFile **f, const DString& filename)
  { FOpenInput (f,filename.c); }

  void FROpenInput (InFile **f,const unsigned fileref);
  void FROpenInput (InFile **f, const DString& filename)
  { FOpenInput (f,filename.c); }

/*  void FOpenInputString (InFile **f,
       char * const buffer,
                         const unsigned length,
                         BufferProc bufferProc);
*/
  void SwitchInput (InFile *f);

  void CurrentInput (InFile **f);

  void FOpenOutput (OutFile **f,const char *filename);

  void FOpenOutput (OutFile **f,const DString& filename)
  { FOpenOutput(f,filename.c); }

  void FROpenOutput (OutFile ** f,const unsigned fileref);
/*
  void FOpenOutputString (OutFile **f,
        char * const buffer,
                          const unsigned length,
                          const BufferProc bufferProc);
*/
  void SwitchOutput (OutFile *f);

  void CurrentOutput (OutFile **f);

  /* These additional procedures allow the user to open several files
     and the standard input/output device "terminal" concurrently and
     to switch to any one of these at any time without closing the others.
     The meaning of "CloseInput" and "CloseOutput" isn't changed, however:
     They close the current input or output file and switch input or output
     to stdin or stdout (= terminal), respectively.

     "FOpenInputString" opens a pseudo file whose contents is read from
     "buffer". "bufferProc" is called if buffer is exhausted.

     "FOpenOutputString" opens a pseudo file whose contents is written to
     "buffer". "bufferProc" is called if buffer is full.

     "CloseOutput" appends a "0C" to the end of the string output if at
     least one character position is left over in "buffer".
  */

  void Remove (const char *filename);
  void Remove (const DString& filename)
  { Remove(filename.c); }
  /* removes the link/file from the file system */

/**********************************************************************/


  void Read (char& ch);
  /* Done := NOT in.eof */

  void Again ();
  /* the next Read... operation reads the last character once more */

  void ReadString (char * const s);

  void ReadString (DString& s);
  /* read string, i.e. sequence of characters not containing
     blanks nor control characters; leading blanks are ignored.
     Input is terminated by any character <= " ";
     this character is assigned to termCH.
     DEL is used for backspacing when input from terminal */

  void ReadInt (int& x);
  /* read string and convert to integer. Syntax:
     integer = "+"|"-"|digit{digit}.
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

  void ReadLn (char * const s);

  void ReadLn (DString& s);

  /* Reads the remainder of the current input line into string s.
     Leading blanks are  n o t  ignored.
     The end-of-line character is skipped.
     termCH = EOL if the entire line fitted into VAR s,
     else termCH = 0C.*/

  void Write (const char ch);

  void WriteString (const char * const s);

  void WriteString (const DString s) { WriteString(s.c); }

  void WriteInt (const long int x,
                 const unsigned d);
  /* write integer x with (at least) d characters.
     If d is greater than the number of digits needed,
     blanks are added preceding the number */

  void WriteCard (const long unsigned x,
                  const unsigned d);
  /* write cardinal x with (at least) d characters.
     If d is greater than the number of digits needed,
     blanks are added preceding the number */

  void WriteLn ();

  void WriteHex (const DString& x);

  void WriteHex (const unsigned char * const x, const unsigned length);
  /* every byte is represented by 2 hexadecimal digits */


  void BigBuffer ();
  /* WriteLn causes the current output buffer to be flushed
     immediately by default. After calling BigBuffer the
     output buffer of the current output file is flushed only 
     if it is full or if CloseOutput is called.

     Repeated invocation of BigBuffer switches back and
     forth beween these two modes. */

  void Flush ();
  /* flush without inserting a newline */

/**********************************************************************/

//private:
  
  InFile *CurrentInFile, *StdIn;
  OutFile *CurrentOutFile, *StdOut, *StdErr;
};

extern DISCO_DLL IOCLASS IO;

#endif
