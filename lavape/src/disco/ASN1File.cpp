/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
         (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#include "ASN1File.h"
#include "SYSTEM.h"
#include "DIO.h"
#include "UNIX.h"
#include "Halt.h"
#include "qapplication.h"
#include <QAbstractSocket>

#include <sys/stat.h>
#pragma hdrstop



void ASN1InFile::errorExitProc ()

{
  error("ASN1 level procedure");
  Done = false;
} // END OF errorExitProc


void ASN1OutFile::errorExitProc ()

{
  error("ASN1 level procedure");
  Halt.HaltPMD();
} // END OF errorExitProc


void ASN1InFile::error (QString msgText)

{
  if (!Silent) {
    IO.WriteLn();
    IO.WriteString("++++ ASN1InFile, error in ");
    IO.WriteString(qPrintable(msgText));
    IO.WriteLn();
  }
  Done = false;
} // END OF error


void ASN1OutFile::error (QString msgText)

{
  if (!Silent) {
    IO.WriteLn();
    IO.WriteString("++++ ASN1OutFile, error in ");
    IO.WriteString(qPrintable(msgText));
    IO.WriteLn();
  }
  Done = false;
} // END OF error

ASN1InFile::ASN1InFile (const QString& fileName, bool silent)
{
	file.setFileName(fileName);

  IO.INIT();

  BufferSize = 20000;
  Silent = silent;
  bufferPtr = 0;

	if (!file.open(QIODevice::ReadOnly)) {
    error("UNIX.open, file \""+fileName+"\"");
    Done = false;
    return;
  }
  bufferPtr = new char [BufferSize];
  bufferPos = 0;
  bufferSize = BufferSize;
  charsRead = 0;
  openForOutput = false;
  Done = true;
} // END OF ASN1InFile


ASN1OutFile::ASN1OutFile (const QString& fileName, bool silent)
{
	file.setFileName(fileName);

  IO.INIT();

  BufferSize = 20000;
  Silent = silent;
  bufferPtr = 0;

	if (!file.open(QIODevice::WriteOnly)) {
    error("UNIX.open, file \""+fileName+"\"");
    Done = false;
    return;
  }
  bufferPtr = new char [BufferSize];
  bufferPos = 0;
  bufferSize = BufferSize;
  charsRead = 0;
  openForOutput = true;
  Done = true;
} // END OF ASN1OutFile


ASN1InFile::~ASN1InFile ()

{
  if (bufferPtr) {
    file.close();
    delete [] bufferPtr;
  }
} // END OF ~ASN1InFile


ASN1OutFile::~ASN1OutFile ()

{
  qint64 rc;

  if (openForOutput && (bufferPos > 0)) {
    rc = file.write(bufferPtr,bufferPos);
    if ((rc < 0) || (rc != (qint64)bufferPos))
      error("~ASN1File: UNIX.write");
  }
  if (bufferPtr) {
    file.close();
    delete [] bufferPtr;
  }
} // END OF ~ASN1OutFile


void ASN1OutFile::putChar (const unsigned char& c)

{
  qint64 rc;

  if (bufferPos == bufferSize) {
    rc = file.write(bufferPtr,bufferSize);
    if ((rc < 0) || (rc != (qint64)bufferSize)) {
      error("putChar: UNIX.write");
      return;
    }
    bufferPos = 0;
  }
  bufferPtr[bufferPos++] = c;
} // END OF putChar


void ASN1InFile::getChar (unsigned char& c)

{
  if (bufferPos == (unsigned)charsRead) {
    charsRead = file.read(bufferPtr,bufferSize);
    if (charsRead < 0) {
      error("getChar: UNIX.read");
      return;
    }
    else if (charsRead == 0) {
      error("getChar: EOF reached");
      return;
    }
    bufferPos = 0;
  }
  c = bufferPtr[bufferPos++];
} // END OF GetChar


////////////////////////////////////////////////////////////////////////////////
// Now the same for sockets:


void ASN1InSock::errorExitProc ()

{
  error("ASN1 level procedure");
  Done = false;
} // END OF errorExitProc


void ASN1OutSock::errorExitProc ()

{
  error("ASN1 level procedure");
  Halt.HaltPMD();
} // END OF errorExitProc


void ASN1InSock::error (QString msgText)

{
  if (!Silent) {
    IO.WriteLn();
    IO.WriteString("++++ ASN1InSock, error in ");
    IO.WriteString(qPrintable(msgText));
    IO.WriteLn();
  }
  Done = false;
  skip = true;
} // END OF error


void ASN1OutSock::error (QString msgText)

{
  if (!Silent) {
    IO.WriteLn();
    IO.WriteString("++++ ASN1OutSock, error in ");
    IO.WriteString(qPrintable(msgText));
    IO.WriteLn();
  }
  Done = false;
  skip = true;
} // END OF error

ASN1InSock::ASN1InSock (QTcpSocket *socket, bool silent) : ASN1()
{
  BufferSize = 20000;
  Silent = silent;
  bufferPtr = 0;

  bufferPtr = new char [BufferSize];
  fildes = socket;
  bufferPos = 0;
  bufferSize = BufferSize;
  charsRead = 0;
  openForOutput = false;
  Done = true;
} // END OF ASN1InFile


ASN1OutSock::ASN1OutSock (QTcpSocket *socket, bool silent) : ASN1()
{
  IO.INIT();

  BufferSize = 20000;
  Silent = silent;
  bufferPtr = 0;

  bufferPtr = new char [BufferSize];
  fildes = socket;
  bufferPos = 0;
  bufferSize = BufferSize;
  charsRead = 0;
  openForOutput = true;
  Done = true;
} // END OF ASN1OutFile


ASN1InSock::~ASN1InSock ()

{
  if (bufferPtr) {
    fildes->abort();
    delete [] bufferPtr;
  }
} // END OF ~ASN1InFile


void ASN1OutSock::flush ()
{
  int rc;

  if (openForOutput && (bufferPos > 0)) {
    rc = fildes->write(bufferPtr,bufferPos);
    if ((rc < 0) || (rc != (int)bufferPos))
      error("~ASN1OutSock: write_TCP");
    bufferPos = 0;
  }
} // END OF ~ASN1OutFile

void ASN1OutSock::waitForBytesWritten () {
  flush();
  fildes->waitForBytesWritten(-1);
}

ASN1OutSock::~ASN1OutSock ()

{
  flush();

  if (bufferPtr) {
    fildes->abort();
    delete [] bufferPtr;
  }
} // END OF ~ASN1OutFile


void ASN1OutSock::putChar (const unsigned char& c)

{
  int rc;

  if (bufferPos == bufferSize) {
    rc = fildes->write(bufferPtr,bufferSize);
    if ((rc < 0) || (rc != (int)bufferSize)) {
      error("putChar: write_TCP");
      return;
    }
    bufferPos = 0;
  }
  bufferPtr[bufferPos++] = c;
} // END OF putChar


void ASN1InSock::getChar (unsigned char& c)

{
  bool ok;

  if (bufferPos == (unsigned)charsRead) {
    fildes->waitForReadyRead(-1);
    ok = charsRead = fildes->read(bufferPtr,bufferSize);
    if (!ok || charsRead <= 0) {
      error("getChar: UNIX.read_TCP");
      return;
    }
    bufferPos = 0;
  }
  c = bufferPtr[bufferPos++];
} // END OF GetChar
