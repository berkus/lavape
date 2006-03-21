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


#include <stdlib.h>
#include <sys/stat.h>
#include "UNIX.h"
#include "MACROS.h"
#include "InFile.h"
#include "MachDep.h"
#pragma hdrstop



InFile::InFile (const DString& filename)
{
	file.setFileName(filename.c);
  Buffer = 0;
  if (filename.Length() == 0) {
    Done = false;
    return;
  }

  if (!file.open(QIODevice::ReadOnly))
    Done = false;
  else {
    Buffer = new char[BufferSize];
    BufPos = 0;
    CharsRead = 0;
    Done = true;
  }
}


InFile::InFile (const unsigned fref)
{
  if (!file.open(fref,QIODevice::ReadOnly))
    Done = false;
	else
  	Done = true;
  Buffer = new char[BufferSize];
  BufPos = 0;
  CharsRead = 0;
}


InFile::~InFile ()
{
  Done = true;
  if (!Buffer) {
    CharsRead = 0;
    BufPos = 0;
    return;
  }

	file.close();

  delete [] Buffer;
}


void InFile::Read (char& ch)
{
  if (BufPos >= CharsRead) {
    CharsRead = file.read(Buffer,BufferSize);
    if (CharsRead == 0) {
      Done = false;
      ch = EOF;
      Length = 0;
      return;
    }
    BufPos = 0;
  }
  ch = Buffer[BufPos++];
  Done = true;
  Length = 1;
}


void InFile::ReadString (char *s)
{
  char ch;
  unsigned pos;

  Length = 0;
  
  do
    Read(ch);
  while (Done && ((ch == ' ') || (ch == HT)));
             
  for (pos = 0; Done && ((unsigned char)ch > ' '); pos++) {
    s[pos] = ch;
    Read(ch);
  }
  s[pos] = '\0';
  
  if (Done) {
    termCH = ch;
    BufPos--;
  }
  else if (pos > 0) {
    termCH = EOF;
    Done = true;
  }
  Length = pos;
}

void InFile::ReadString (DString& s)
{
  char ch;
  unsigned pos;

  Length = 0;
  
  do
    Read(ch);
  while (Done && ((ch == ' ') || (ch == HT)));
             
  for (pos = 0; Done && ((unsigned char)ch > ' '); pos++) {
    if (pos >= s.m) {
      s.l = pos;
      s.Expand((s.m ? s.m : 80));
    }
    s[pos] = ch;
    Read(ch);
  }
  if (s.c) s[pos] = '\0';
  
  if (Done) {
    termCH = ch;
    BufPos--;
  }
  else if (pos > 0) {
    termCH = EOF;
    Done = true;
  }
  Length = pos;
  s.l = pos;
}


void InFile::ReadLn (char *s)
{
  unsigned pos=0;

  for (;;) {
    Read(termCH);
    if (!Done
        || (termCH == EOL)
        || (termCH == EOF))
      break;
    else {
      s[pos] = termCH;
      pos++;
    }
  }
  s[pos] = '\0';
  Length = pos;
}

void InFile::ReadLn (DString& s)
{
  unsigned pos=0;

  for (;;) {
    Read(termCH);
    if (!Done
        || (termCH == EOL)
        || (termCH == EOF))
      break;
    else {
      if (pos >= s.m) {
  s.l = pos;
  s.Expand((s.m ? s.m : 80));
      }
      s[pos] = termCH;
      pos++;
    }
  }
  Length = pos;
  s.l = pos;
  if (s.c) s[pos] = '\0';
}


void InFile::ReadInt (int& x)

{
  unsigned cardinal, l=0;
  int sign=1;
  char ch;

  Length = 0;

  for (;;) {
    Read(ch);
    if (!Done) return;
    if (ch > ' ') break;
  }
  
  if (ch == '+') l++;
  else if (ch == '-') {
    sign = -1;
    l++;
  }
  else BufPos--;
  
  ReadCard(cardinal);
  if (!Done) return;
  
  Length += l;
  x = (sign > 0 ? cardinal : -(int)cardinal);
}


void InFile::ReadCard (unsigned& x)

{
  char ch;
  unsigned l;

  Length = 0;

  for (;;) {
    Read(ch);
    if (!Done) return;
    if (ch > ' ') break;
  }
  
  if ((ch < '0') || (ch > '9')) {
    Again();
    Done = false;
    return;
  }
  
  x = 0;
  for (l=0; Done && (ch >= '0') && (ch <= '9'); l++) {
    x = x*10+(ch-'0');
    Read(ch);
  }
  
  if (Done) BufPos--;
  else Done = true;
  Length = l;
}


inline bool hexDigit (const char& ch)
{
  if (((ch >= '0') && (ch <= '9'))
       || ((ch >= 'A') && (ch <= 'F'))
       || ((ch >= 'a') && (ch <= 'f'))) return true;
  else return false;
}



void InFile::ReadHex (unsigned& x)

{
  char ch;
  unsigned l, digVal;

  Length = 0;

  for (;;) {
    Read(ch);
    if (!Done) return;
    if (ch > ' ') break;
  }
  
  if (!hexDigit(ch)) {
    Again();
    Done = false;
    return;
  }
  
  x = 0;
  for (l=0; Done && hexDigit(ch); l++) {
    if ((ch >= '0') && (ch <= '9'))
      digVal = ch-'0';
    else if ((ch >= 'A') && (ch <= 'F'))
      digVal = ch-'A'+10;
    else
      digVal = ch-'a'+10;
    x = x*16+digVal;
    Read(ch);
  }
  
  if (Done) BufPos--;
  else Done = true;
  Length = l;
}

/*
static void ReadBytes (const unsigned fileref,
           char *buf,
           const unsigned BytesToRead,
           unsigned& BytesRead)
{
  int result;
  
    result = _read(fileref,buf,BytesToRead);
  
  if (result == -1) BytesRead = 0;
  else BytesRead = result;
}

void InFile::BufferEmpty ()
{
  ReadBytes(fileref,Buffer,BufferSize,CharsRead);
}
*/

