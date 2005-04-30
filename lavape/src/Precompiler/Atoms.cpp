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


#include "Atoms.h"
#include <string.h>


AtomType Atom;
char StringBuffer[MaxAtomStore];
TStringBufferPos StringBufferPos;
char ATOMSTR[NoAtom+1][MaxLenAtom];
unsigned AtomLine, AtomCol, CommentLine, CommentCol;


/************************************************************************/

static bool __INITstarted=false;


static AtomType iAtom;


static const unsigned hashPrime = 499;


typedef struct entry *entryPtr;

struct entry {
  entryPtr nextEntry;
  AtomType index;
};

static struct {
  entryPtr firstEntry, lastEntry;
} hashTable[hashPrime];

static unsigned hashCode (const char * const s);


static void ATO (const char * const s);


/************************************************************************/

unsigned hashCode (const char * const s)

{
  unsigned hc, i;

  hc = 0;
  for (i = 0; ; i++) {
    if (s[i] == '\0') return (hc % hashPrime);
    hc += s[i];
  }
}


void ATO (const char * const s)

{
  entryPtr currentEntry, newEntry;
  unsigned hc;

  if (iAtom < NoAtom)
    iAtom = AtomType(iAtom+1);
  strcpy(ATOMSTR[iAtom],s);

  if (iAtom < EndOfReservedWords) {
    newEntry = new entry;
    newEntry->nextEntry = 0;
    newEntry->index = iAtom;

    hc = hashCode(s);
    currentEntry = hashTable[hc].lastEntry;
    if (currentEntry == 0)
      hashTable[hc].firstEntry = newEntry;
    else
      currentEntry->nextEntry = newEntry;
    hashTable[hc].lastEntry = newEntry;
  }
}


bool IsReservedWord ()

{
  entryPtr currentEntry;
  const char * const substrPtr=&StringBuffer[StringBufferPos];
  unsigned hc;
  int result;

  hc = hashCode(substrPtr);
  currentEntry = hashTable[hc].firstEntry;
  for (;;) {
    if (currentEntry == 0)
      break;
    result = Compare(substrPtr,ATOMSTR[currentEntry->index]);
    if (result <= 0)
      break;
    currentEntry = currentEntry->nextEntry;
  }
  if ((currentEntry == 0)
      || (result < 0))
    return false;
  else {
    Atom = currentEntry->index;
    return true;
  }
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void Atoms_INIT ()
{
  unsigned i;
  
  if (__INITstarted) return;
  __INITstarted = true;


  AtomLine = 1;
  AtomCol = 1;

  for (i = 0; i <= hashPrime-1; i++) {
    hashTable[i].firstEntry = 0;
    hashTable[i].lastEntry = 0;
  }

  iAtom = StartAtom;
  ATO("APPLICATION");
  ATO("ARBITRARY");
  ATO("AVAILABLE");
  ATO("BITMAP");
  ATO("BITSTRING");
  ATO("BUTTONMENU");
  ATO("BYTES");
  ATO("CALL");
  ATO("CALLSUSPEND");
  ATO("CASE");
  ATO("CDP");
  ATO("CENTERALIGNED");
  ATO("CENTERED");
  ATO("CHAIN");
  ATO("CHAIN0");
  ATO("CHAINANY");
  ATO("CHAINANY0");
  ATO("CHAINX");
  ATO("CHECK");
  ATO("CHECKPOINT");
  ATO("CONTEXT");
  ATO("COPY");
  ATO("CURSOR");
  ATO("DATE");
  ATO("DEFAULT");
  ATO("DONE");
  ATO("DROP");
  ATO("ELSE");
  ATO("END");
  ATO("ENTER");
  ATO("ERRORTEXT");
  ATO("EXPLICIT");
  ATO("false");
  ATO("FIO");
  ATO("FIXEDLENGTH");
  ATO("FOR");
  ATO("FORM");
  ATO("FORSYS");
  ATO("FROM");
  ATO("FULLBOX");
  ATO("GET");
  ATO("GETPRIVATE");
  ATO("GETSUSPEND");
  ATO("HIGHEMPHASIS");
  ATO("HORIZ");
  ATO("HORIZONTAL");
  ATO("HRULES");
  ATO("IMPLICIT");
  ATO("IMPORT");
  ATO("INCLUDE");
  ATO("INDENTATION");
  ATO("INOID");
  ATO("INSYSOID");
  ATO("INVAL");
  ATO("INVALENC");
  ATO("INVISIBLE");
  ATO("LEFTALIGNED");
  ATO("LEFTJUSTIFIED");
  ATO("LIKE");
  ATO("LOCAL");
  ATO("LOWEMPHASIS");
  ATO("LZCARDINAL");
  ATO("MENU");
  ATO("MENUFIRST");
  ATO("MONEYSTRING");
  ATO("NAME");
  ATO("NESTED");
  ATO("NESTED0");
  ATO("NESTEDANY");
  ATO("NESTEDANY0");
  ATO("NESTEDX");
  ATO("NEWFRAME");
  ATO("NEWLINE");
  ATO("NF");
  ATO("NF0");
  ATO("NF1");
  ATO("NF2");
  ATO("NF3");
  ATO("NL");
  ATO("NL0");
  ATO("NL1");
  ATO("NL2");
  ATO("NL3");
  ATO("NOCLASS");
  ATO("NOECHO");
  ATO("NOELLIPSIS");
  ATO("NORULES");
  ATO("OF");
  ATO("OID");
  ATO("OPTIONMENU");
  ATO("ORGUNIT");
  ATO("ORGVAL");
  ATO("ORGVAR");
  ATO("OUTOID");
  ATO("OUTSYSOID");
  ATO("OUTVAL");
  ATO("OVERLAY");
  ATO("OWNER");
  ATO("POPDOWN");
  ATO("POPUP");
  ATO("POPUPCASE");
  ATO("POPUPMENU");
  ATO("PRIVATE");
  ATO("PROC");
  ATO("PROCEDURE");
  ATO("PUT");
  ATO("PUTPRIVATE");
  ATO("REALIZE");
  ATO("RETURN");
  ATO("REVERSE");
  ATO("RIGHTALIGNED");
  ATO("RIGHTJUSTIFIED");
  ATO("SET");
  ATO("SHOW");
  ATO("SHOWENC");
  ATO("SIG");
  ATO("SIGNATURE");
  ATO("SPACE");
  ATO("START");
  ATO("STOP");
  ATO("STRING");
  ATO("STRING0");
  ATO("SYSOID");
  ATO("SYSPROC");
  ATO("TAB");
  ATO("TEXT");
  ATO("TIME");
  ATO("TOGGLE");
  ATO("true");
  ATO("TYPE");
  ATO("UNALIGNED");
  ATO("UNIVERSAL");
  ATO("VAL");
  ATO("VALENC");
  ATO("VARIANT");
  ATO("VERIFY");
  ATO("VERSION");
  ATO("VERT");
  ATO("VERTICAL");
  ATO("VRULES");
  ATO("YEAR");
  
// small initial letter:
  ATO("address");
  ATO("asm");
  ATO("auto");
  ATO("bool");
  ATO("break");
  ATO("byte");
  ATO("catch");
  ATO("char");
  ATO("class");
  ATO("const");
  ATO("continue");
  ATO("default");
  ATO("delete");
  ATO("do");
  ATO("double");
  ATO("else");
  ATO("enum");
  ATO("extern");
  ATO("float");
  ATO("for");
  ATO("friend");
  ATO("goto");
  ATO("if");
  ATO("inline");
  ATO("int");
  ATO("long");
//  ATO("new");
  ATO("operator");
  ATO("private");
  ATO("protected");
  ATO("public");
  ATO("register");
  ATO("short");
  ATO("signed");
  ATO("sizeof");
  ATO("static");
  ATO("struct");
  ATO("switch");
  ATO("template");
//  ATO("this");
  ATO("throw");
  ATO("try");
  ATO("typedef");
  ATO("union");
  ATO("unsigned");
  ATO("virtual");
  ATO("void");
  ATO("volatile");
  ATO("while");
  ATO("word");
  ATO("EORW"); /* dummy */
  
  ATO("identifier");
  ATO("charConst");
  ATO("stringConst");
  ATO("intConst");
  ATO("octalConst");
  ATO("hexConst");
  ATO("floatConst");
  
  ATO("!");
  ATO("%");
  ATO("^");
  ATO("&");
  ATO("*");
  ATO("(");
  ATO(")");
  ATO("-");
  ATO("+");
  ATO("=");
  ATO("{");
  ATO("}");
  ATO("|");
  ATO("~");
  ATO("[");
  ATO("]");
  ATO(";");
  ATO(":");
  ATO("<");
  ATO(">");
  ATO("?");
  ATO(",");
  ATO(".");
  ATO("/");
  
  ATO("->");
  ATO("++");
  ATO("--");
  ATO(".*");
  ATO("->*");
  ATO("<<");
  ATO(">>");
  ATO("<=");
  ATO(">=");
  ATO("==");
  ATO("!=");
  ATO("&&");
  ATO("||");
  ATO("*=");
  ATO("/=");
  ATO("%=");
  ATO("+=");
  ATO("-=");
  ATO("<<=");
  ATO(">>=");
  ATO("&=");
  ATO("^=");
  ATO("|=");
  ATO("::");
  ATO("::*");
 
  ATO("$");
  ATO("#");
  ATO("end-of-file");
  ATO("no-Atom");

  Atom = NoAtom;

}
