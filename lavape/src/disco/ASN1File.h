#ifdef __GNUC__
#pragma interface
#endif

/**********************************************************************

         #include file for class ASN1FileCLASS

 **********************************************************************/

#ifndef __ASN1File
#define __ASN1File

#include "prelude.h"
#include "sflsock.h"
#include "qstring.h"
#include "ASN1.h"



class DISCO_DLL ASN1InFile : public virtual ASN1 {

public:

  unsigned BufferSize;
  bool Done;
  /* error messages are printed if and as long as Silent is false
     (= default) */


  ASN1InFile (const QString& fileName, bool silent=false);

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

  void error (QString msgText);
};


class DISCO_DLL ASN1OutFile : public virtual ASN1 {

public:

  unsigned BufferSize;
  bool Done;
  /* error messages are printed if and as long as Silent is false
     (= default) */


  ASN1OutFile (const QString& fileName, bool silent=false);

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

  void error (QString msgText);
};


class DISCO_DLL ASN1InSock : public virtual ASN1 {

public:

  unsigned BufferSize;
  bool Done;
  /* error messages are printed if and as long as Silent is false
     (= default) */


  ASN1InSock (sock_t socket, bool silent=true);

  ~ASN1InSock ();

  void putChar (const unsigned char&) {}

  void getChar (unsigned char&);

  void errorExitProc ();


/**********************************************************************/

private:

  sock_t fildes;
  unsigned bufferPos, bufferSize;
  int charsRead;
  char *bufferPtr;
  bool openForOutput;

  void error (QString msgText);
};


class DISCO_DLL ASN1OutSock : public virtual ASN1 {

public:

  unsigned BufferSize;
  bool Done;
  /* error messages are printed if and as long as Silent is false
     (= default) */


  ASN1OutSock (sock_t socket, bool silent=true);

  ~ASN1OutSock ();

  void flush ();

  void putChar (const unsigned char&);

  void getChar (unsigned char&) {}

  void errorExitProc ();


/**********************************************************************/

private:

  sock_t fildes;
  unsigned bufferPos, bufferSize;
  int charsRead;
  char *bufferPtr;
  bool openForOutput;

  void error (QString msgText);
};

#endif
