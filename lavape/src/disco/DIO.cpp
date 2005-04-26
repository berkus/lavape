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

#include "disco_all.h"
/*
#include "DIO.h"
#include "StrIFile.h"
#include "StrOFile.h"
*/
#include "UNIX.h"


IOCLASS IO;
static bool __INITIALIZED=false;

/************************************************************************/


void IOCLASS::OpenInput (const char *filename)

{
  CurrentInFile = new InFile(filename);
  Done = CurrentInFile->Done;
}

extern bool openInAppendMode;

void IOCLASS::Append ()

{
  openInAppendMode = true;
}


void IOCLASS::OpenOutput (const char *filename)

{
  CurrentOutFile = new OutFile(filename);
  Done = CurrentOutFile->Done;
}


void IOCLASS::CloseInput ()

{
  if (CurrentInFile->fileref > 0)
    delete CurrentInFile;
  CurrentInFile = StdIn;
  Done = true;
}


void IOCLASS::CloseOutput ()

{
  if (CurrentOutFile->fileref > 2)
    delete CurrentOutFile;
  CurrentOutFile = StdOut;
  Done = true;
}


void IOCLASS::FOpenInput (InFile **f,const char *filename)

{
  CurrentInFile = new InFile(filename);
  *f = CurrentInFile;
  Done = CurrentInFile->Done;
}


void IOCLASS::FROpenInput (InFile **f,const unsigned fileref)

{
  CurrentInFile = new InFile(fileref);
  *f = CurrentInFile;
  Done = CurrentInFile->Done;
}


void IOCLASS::FOpenInputString (InFile **f,
        char * const buffer,
        const unsigned length,
        BufferProc bufProc)

{
  CurrentInFile = new StringInFile(buffer,length,bufProc);
  *f = CurrentInFile;
  Done = CurrentInFile->Done;
}


void IOCLASS::SwitchInput (InFile *f)
{
  if (f == 0) Done = false;
  else {
    CurrentInFile = f;
    Done = true;
  }
}


void IOCLASS::CurrentInput (InFile **f)
{
  *f = CurrentInFile;
}


void IOCLASS::FOpenOutput (OutFile **f,const char *filename)

{
  CurrentOutFile = new OutFile(filename);
  *f = CurrentOutFile;
  Done = CurrentOutFile->Done;
}


void IOCLASS::FROpenOutput (OutFile **f,const unsigned fileref)

{
  CurrentOutFile = new OutFile(fileref);
  *f = CurrentOutFile;
  Done = CurrentOutFile->Done;
}


void IOCLASS::FOpenOutputString (OutFile **f,
         char * const buffer,
         const unsigned length,
         const BufferProc bufProc)

{
  CurrentOutFile = new StringOutFile(buffer,length,bufProc);
  *f = CurrentOutFile;
  Done = CurrentOutFile->Done;
}


void IOCLASS::SwitchOutput (OutFile *f)
{
  if (f == 0) Done = false;
  else {
    CurrentOutFile = f;
    Done = true;
  }
}


void IOCLASS::CurrentOutput (OutFile **f)
{
  *f = CurrentOutFile;
}


void IOCLASS::Remove (const char *filename)

{
  int result;

  if (filename[0] == '\0') {
    Done = false;
    return;
  }

  do
    result = unlink(filename);
  while ((result == -1) && (errno == EINTR));

  if (result < 0) Done = false;
  else Done = true;
}


void IOCLASS::Read (char& ch)
{
  CurrentInFile->Read(ch);
  Done = CurrentInFile->Done;
  Length = CurrentInFile->Length;
}


void IOCLASS::Again ()
{
  CurrentInFile->Again ();
  Done = true;
}


void IOCLASS::ReadString (char * const s)
{
  CurrentInFile->ReadString(s);
  Done = CurrentInFile->Done;
  termCH = CurrentInFile->termCH;
  Length = CurrentInFile->Length;
}


void IOCLASS::ReadString (DString& s)
{
  CurrentInFile->ReadString(s);
  Done = CurrentInFile->Done;
  termCH = CurrentInFile->termCH;
  Length = CurrentInFile->Length;
}


void IOCLASS::ReadInt (int& x)

{
  CurrentInFile->ReadInt(x);
  termCH = CurrentInFile->termCH;
  Length = CurrentInFile->Length;
}


void IOCLASS::ReadCard (unsigned& x)

{
  CurrentInFile->ReadCard(x);
  Done = CurrentInFile->Done;
  termCH = CurrentInFile->termCH;
  Length = CurrentInFile->Length;
}


void IOCLASS::ReadHex (unsigned& x)

{
  CurrentInFile->ReadHex(x);
  Done = CurrentInFile->Done;
  Length = CurrentInFile->Length;
}


void IOCLASS::ReadLn (char * const s)
{
  CurrentInFile->ReadLn(s);
  Done = CurrentInFile->Done;
  termCH = CurrentInFile->termCH;
  Length = CurrentInFile->Length;
}


void IOCLASS::ReadLn (DString& s)
{
  CurrentInFile->ReadLn(s);
  Done = CurrentInFile->Done;
  termCH = CurrentInFile->termCH;
  Length = CurrentInFile->Length;
}


void IOCLASS::Write (const char ch)

{
  CurrentOutFile->Write(ch);
  Done = CurrentOutFile->Done;
  Length = CurrentOutFile->Length;
}

void IOCLASS::WriteString (const char * const s)

{
  CurrentOutFile->WriteString(s);
  Done = CurrentOutFile->Done;
  Length = CurrentOutFile->Length;
}


void IOCLASS::WriteInt (const long int x,const unsigned d)

{
  CurrentOutFile->WriteInt(x,d);
  Done = CurrentOutFile->Done;
  Length = CurrentOutFile->Length;
}


void IOCLASS::WriteCard (const long unsigned x,const unsigned d)

{
  CurrentOutFile->WriteCard(x,d);
  Done = CurrentOutFile->Done;
  Length = CurrentOutFile->Length;
}


void IOCLASS::WriteLn ()

{
  CurrentOutFile->WriteLn();
  Done = CurrentOutFile->Done;
}


void IOCLASS::WriteHex (const unsigned char * const x, const unsigned length)

{
  CurrentOutFile->WriteHex(x,length);
  Done = CurrentOutFile->Done;
  Length = CurrentOutFile->Length;
}


void IOCLASS::WriteHex (const DString& s)

{
  CurrentOutFile->WriteHex(s);
  Done = CurrentOutFile->Done;
  Length = CurrentOutFile->Length;
}


void IOCLASS::BigBuffer ()
{
  CurrentOutFile->BigBuffer();
}


void IOCLASS::Flush ()

{
  CurrentOutFile->Flush();
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void IOCLASS::INIT ()
{
  if (__INITIALIZED) return;
  __INITIALIZED = true;


  Done = true;


  StdIn = new InFile(0);
  CurrentInFile = StdIn;

  StdOut = new OutFile(1);
  CurrentOutFile = StdOut;

  StdErr = new OutFile(2);
}
