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

/*
#ifdef __GNUC__
#pragma implementation
#endif
*/

#include "UNIX.h"
#include "OutFile.h"
#include "MachDep.h"
//#include "OSDep.h"
#include "Convert.h"
//#include <SYS/Stat.h>


bool openInAppendMode=false;


/************************************************************************/


OutFile::OutFile (const char *filename)
{
	file.setFileName(filename);
  QIODevice::OpenMode openMode;

  Buffer = 0;
  if (filename[0] == '\0') {
    Done = false;
    return;
  }
  
  if (openInAppendMode) {
    openMode = QIODevice::Append | QIODevice::ReadWrite;
    openInAppendMode = false;
  }
  else
		openMode =QIODevice::WriteOnly;

  if (!file.open(openMode))
    Done = false;
  else {
    Buffer = new char [BufferSize];
    BufPos = 0;
    Size = BufferSize;
    bigBuffer = false;
    Done = true;
  }
}


OutFile::OutFile (const unsigned fref)
{
  if (!file.open(fref,QIODevice::WriteOnly))
		Done = false;
		else 
			Done = true;
  Buffer = new char[BufferSize];
  BufPos = 0;
  Size = BufferSize;
  bigBuffer = false;
}


OutFile::~OutFile ()

{
  int result;

  Done = true;
  Flush();
  if (!Buffer) return;
//  if (fileref <= 2) return;
  
  file.close();

  delete [] Buffer;
//  if (result == -1) Done = false;
}


void OutFile::WriteLn ()

{
  Write(EOL);
  if (!Done) return;
  if (!bigBuffer) Flush();
}


void OutFile::Write (const char ch)

{
  Done = true;
  if (BufPos >= Size) {
    Flush();
    if (!Done) {
      Length = 0;
      return;
    }
  }
  Buffer[BufPos++] = ch;
  Length = 1;
}


void OutFile::WriteString (const char * const s)

{
  Done = true;
  if (!s) return;
  
  for (Length = 0; s[Length] != '\0';) {
    if (BufPos >= Size) {
      Flush();
      if (!Done) return;
    }
    Buffer[BufPos++] = s[Length++];
  }
}


void OutFile::WriteInt (const long int x,
                        const unsigned d)

{
  int blanks;
  DString str(50);

  Convert.CardToString((x < 0 ? -x : x),str);
  
  str.l += (x<0);
  blanks = d-str.l;
  for (int i = 1; i <= blanks; i++) Write(' ');

  if (x < 0) Write('-');
    
  WriteString(str);
  
  Length = (d > str.l ? d : str.l);
}


void OutFile::WriteHex (const unsigned char * const s,
      const unsigned length)

{
  unsigned char ch;
  
  for (unsigned i = 0; i < length; i++) {
    ch = s[i]>>4;
    ch = (ch < 10 ? '0'+ch : 'A'+(ch-10));
    Write((char)ch);
    
    ch = s[i] & '\x0f';
    ch = (ch < 10 ? '0'+ch : 'A'+(ch-10));
    Write((char)ch);
  }
}


void OutFile::WriteHex (const DString& s)

{
  unsigned char ch;
  
  for (unsigned i = 0; i < s.Length(); i++) {
    ch = ((unsigned char)s[i])>>4;
    ch = (ch < 10 ? '0'+ch : 'A'+(ch-10));
    Write((char)ch);
    
    ch = ((unsigned)s[i]) & '\x0f';
    ch = (ch < 10 ? '0'+ch : 'A'+(ch-10));
    Write((char)ch);
  }
}


void OutFile::BigBuffer ()

{
  bigBuffer = !bigBuffer;
}


void OutFile::Flush ()

{
  int result, BytesToWrite;
  
  if (BufPos > 0) {
    BytesToWrite = BufPos;
    result = file.write(Buffer,BytesToWrite);
    if (result < 0) {
      printf("++++ error in OutFile::Flush, errno=%d\n",errno);
      Done = false;
      return;
    }
      
    if (result == BytesToWrite) BufPos = 0;
    else BufPos = result;
    
    Done = true;
  }
}
