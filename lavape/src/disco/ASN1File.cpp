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


#ifdef __GNUC__
#pragma implementation
#endif

#include "ASN1File.h"

#include "SYSTEM.h"
#include "DIO.h"
#include "UNIX.h"
#include "Halt.h"
//#include "disco.h"


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


void ASN1InFile::error (DString msgText)

{
  if (!Silent) {
    IO.WriteLn();
    IO.WriteString("++++ ASN1InFile, error in ");
    IO.WriteString(msgText.c);
    IO.WriteLn();
  }
  Done = false;
} // END OF error


void ASN1OutFile::error (DString msgText)

{
  if (!Silent) {
    IO.WriteLn();
    IO.WriteString("++++ ASN1OutFile, error in ");
    IO.WriteString(msgText.c);
    IO.WriteLn();
  }
  Done = false;
} // END OF error

#ifndef WIN32
#define O_BINARY 0
#endif

ASN1InFile::ASN1InFile (const DString& fileName, bool silent)
{
  int rc;
  
  IO.INIT();
  
  BufferSize = 20000;
  Silent = silent;
  bufferPtr = 0;

  rc = open(fileName.c,O_RDONLY|O_BINARY,0644);
  if (rc < 0) {
    error("UNIX.open, file \""+fileName+"\"");
    Done = false;
    return;
  }
  bufferPtr = new char [BufferSize];
  fildes = rc;
  bufferPos = 0;
  bufferSize = BufferSize;
  charsRead = 0;
  openForOutput = false;
  Done = true;
} // END OF ASN1InFile


ASN1OutFile::ASN1OutFile (const DString& fileName, bool silent)
{
  int rc;
  
  IO.INIT();
  
  BufferSize = 20000;
  Silent = silent;
  bufferPtr = 0;
  
  rc = open(fileName.c,O_TRUNC | O_CREAT | O_WRONLY | O_BINARY,0644);
  if (rc < 0) {
    error("UNIX.open, file \""+fileName+"\"");
    Done = false;
    return;
  }
  bufferPtr = new char [BufferSize];
  fildes = rc;
  bufferPos = 0;
  bufferSize = BufferSize;
  charsRead = 0;
  openForOutput = true;
  Done = true;
} // END OF ASN1OutFile


ASN1InFile::~ASN1InFile ()

{
  if (bufferPtr) {
    close(fildes);
    delete [] bufferPtr;
  }
} // END OF ~ASN1InFile


ASN1OutFile::~ASN1OutFile ()

{
  int rc;
  
  if (openForOutput && (bufferPos > 0)) {
    rc = write(fildes,bufferPtr,bufferPos);
    if ((rc < 0) || (rc != (int)bufferPos))
      error("~ASN1File: UNIX.write");
  }
  if (bufferPtr) {
    close(fildes);
    delete [] bufferPtr;
  }
} // END OF ~ASN1OutFile


void ASN1OutFile::putChar (const unsigned char& c)

{
  int rc;
  
  if (bufferPos == bufferSize) {
    rc = write(fildes,bufferPtr,bufferSize);
    if ((rc < 0) || (rc != (int)bufferSize)) {
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
    charsRead = read(fildes,bufferPtr,bufferSize);
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
