#ifndef __ASN1File
#define __ASN1File
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include <QtNetwork/QTcpSocket>

#include "qstring.h"
#include "ASN1.h"
#include "qfile.h"



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

//  int fildes;
	QFile file;
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

//  unsigned fildes;
	QFile file;

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


  ASN1InSock (QTcpSocket *socket, bool silent=true);

  ~ASN1InSock ();

  void putChar (const unsigned char&) {}

  void getChar (unsigned char&);

  void errorExitProc ();


/**********************************************************************/

private:

  QTcpSocket *fildes;
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


  ASN1OutSock (QTcpSocket *socket, bool silent=true);

  ~ASN1OutSock ();

  void flush ();

  void putChar (const unsigned char&);

  void getChar (unsigned char&) {}

  void errorExitProc ();


/**********************************************************************/

private:

  QTcpSocket *fildes;
  unsigned bufferPos, bufferSize;
  int charsRead;
  char *bufferPtr;
  bool openForOutput;

  void error (QString msgText);
};

#endif
