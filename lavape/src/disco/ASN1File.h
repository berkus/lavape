#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

         #include file for class ASN1FileCLASS

 **********************************************************************/

#ifndef __ASN1File
#define __ASN1File

#include "DString.h"
#include "ASN1.h"



class DISCO_DLL ASN1InFile : public virtual ASN1 {

public:

  unsigned BufferSize;
  bool Done;
  /* error messages are printed if and as long as Silent is false
     (= default) */


  ASN1InFile (const DString& fileName, bool silent=false);

  ~ASN1InFile ();

  void putChar (const unsigned char&) {}

  void getChar (unsigned char&);

  void errorExitProc ();


/**********************************************************************/

private:

  unsigned fildes;
  unsigned bufferPos, bufferSize;
  int charsRead;
  char *bufferPtr;
  bool openForOutput;

  void error (DString msgText);
};


class DISCO_DLL ASN1OutFile : public virtual ASN1 {

public:

  unsigned BufferSize;
  bool Done;
  /* error messages are printed if and as long as Silent is false
     (= default) */


  ASN1OutFile (const DString& fileName, bool silent=false);

  ~ASN1OutFile ();

  void putChar (const unsigned char&);

  void getChar (unsigned char&) {}

  void errorExitProc ();


/**********************************************************************/

private:

  unsigned fildes;
  unsigned bufferPos, bufferSize;
  int charsRead;
  char *bufferPtr;
  bool openForOutput;

  void error (DString msgText);
};

#endif
