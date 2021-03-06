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


#include "Tokens.h"
//#include "TokenNode.h"
#include "SYSTEM.h"
#include <string.h>

#pragma hdrstop

#include "Constructs.h"


const unsigned MaxLenToken = 20;
const unsigned MaxString = 1000;


/************************************************************************/

static bool __INITstarted=false;

QString TOKENSTR[NoToken+1];

static TToken iToken=TToken(0);


//THashTable hashTable[hashPrime];
QHash<QString,unsigned> hashTable;

unsigned priority[] = {
  1,2,2,2,2,3,
  10,10,10,10,10,10,
  30,30,31,31,31,31,31,31,31,31,31,32,
  40,40,41,
  90,90,31,31,2};



/************************************************************************/

//unsigned HashCode (const char * const s)
//
//{
//  unsigned hc, i;
//
//  hc = 0;
//  for (i = 0; ; i++) {
//    if (s[i] == '\0') return (hc % hashPrime);
//    hc += s[i];
//  }
//}


static void ATO (QString s)

{
  //TEntryPtr currentEntry, succEntry, newEntry;
  //unsigned hc;

//  strcpy(TOKENSTR[iToken],s);
  TOKENSTR[iToken] = s;
  hashTable[s] = iToken;
/*
  newEntry = new THTEntry;
  newEntry->index = iToken;
  newEntry->nextEntry = 0;

  hc = HashCode(s.toAscii());
  for ( currentEntry = hashTable[hc].firstEntry;
        currentEntry;
        currentEntry = currentEntry->nextEntry) {
    if (qstrcmp(TOKENSTR[currentEntry->index].toAscii(),s.toAscii())==-1)
      break;
  }
  if (currentEntry) {
    succEntry = currentEntry->nextEntry;
    currentEntry->nextEntry = newEntry;
    newEntry->nextEntry = succEntry;
    if (currentEntry == hashTable[hc].lastEntry)
      hashTable[hc].lastEntry = newEntry;
  }
  else {
    newEntry->nextEntry = hashTable[hc].firstEntry;
    hashTable[hc].firstEntry = newEntry;
    if (!hashTable[hc].lastEntry)
      hashTable[hc].lastEntry = newEntry;
  }
*/
  if (iToken < NoToken)
    iToken = TToken(iToken+1);

}



/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void Tokens_INIT ()
{
  //unsigned i;

  if (__INITstarted) return;
  __INITstarted = true;


  hashTable.clear();

  //for (i = 0; i <= hashPrime-1; i++) {
  //  hashTable[i].firstEntry = 0;
  //  hashTable[i].lastEntry = 0;
  //}

  //TOKENSTR = new QString[NoToken+1];

  iToken = (TToken)0;

  ATO(";");
  ATO("or");
  ATO("xor");
  ATO("and");
  ATO("and then");
  ATO("not");

  ATO("=");
  ATO("\xAC=");
  ATO("<");
  ATO(">");
  ATO("<=");
  ATO(">=");

  ATO("+");
  ATO("-");
  ATO("*");
  ATO("/");
  ATO("%");
  ATO("&");
  ATO("|");
  ATO("||");
  ATO("\xAC");
  ATO("<<");
  ATO(">>");
  ATO("[]");

#ifdef WIN32
  //ATO(QChar(0xDF)); // normal arrows (Wingdings)
  //ATO(QChar(0xE0));
  ATO(QChar(0xE7)); // fat arrows (Wingdings)
  ATO(QChar(0xE8));
#else //if Linux
  ATO(QChar(0x21D0));
  ATO(QChar(0x21D2));
//#else
//  ATO(" <== ");
//  ATO(" ==> ");
#endif

  ATO(".");
  ATO("(");
  ATO(")");

  ATO("@");
  ATO("#");
  ATO("?");
  ATO("?");
  ATO("?");

  ATO("EndOfOps");

  ATO(":");
  ATO(",");
  ATO("...");
  ATO("?");
  ATO("?");
  ATO("!");
  ATO("{");
  ATO("}");
  ATO("[");
  ATO("]");
  ATO("~");
  ATO("$");

  ATO("?");
  ATO("?");
  ATO("?");
  ATO("?");

// initial letter #
  ATO("#assert");
  ATO("#run");
  ATO("#connect");
  ATO("#clone");
  ATO("#declare");
  ATO("?");
  ATO("#exists");
  ATO("#foreach");
  ATO("#if");
  ATO("#ifx");
  ATO("#new");
  ATO("#select");
  ATO("#switch");
  ATO("#transaction");
  ATO("#try");
  ATO("#type");

  ATO("#ifdef");
  ATO("?");
  ATO("?");
  ATO("?");
  ATO("?");

  ATO("AVG");
  ATO("Boolean");
  ATO("Cardinal");
  ATO("CHAIN");
  ATO("Chain");
  ATO("Char");
  ATO("COUNT");
  ATO("Double");
  ATO("Float");
  ATO("Integer");
  ATO("MAX");
  ATO("MERGE");
  ATO("MIN");
  ATO("String");
  ATO("SUM");

  ATO("?");
  ATO("?");
  ATO("?");
  ATO("?");
  ATO("?");

// small initial letter:
  ATO("add");
  ATO("assert");
  ATO("set");  // object-to-variable assignment
  ATO("call");  // function call statement
  ATO("\xe8");  // function expression
  ATO("attach");
  ATO("as");
  ATO("base");
  ATO("but");
  ATO("by");
  ATO("run");
  ATO("connect");
  ATO("case");
  ATO("case");
  ATO("catch");
  ATO("clone");
  ATO("complete");
  ATO("concurrent");
  ATO("disconnect");
  ATO("const");
  ATO("invariant");
  ATO("copy");
  ATO("declare");
  ATO("initialize");
  ATO("default initializer");
  ATO("div");
  ATO("do");
  ATO("else");
  ATO("elsif");
  ATO("enum");
  ATO("exec");
  ATO("exists");
  ATO("extract");
  ATO("fail");
  ATO("false");
  ATO("for");
  ATO("foreach");
  ATO("from");
  ATO("function");
  ATO("get");
  ATO("handle");
  ATO("holds");
  ATO("if");
  ATO("ifx");
  ATO("implementation");
  ATO("in");
  ATO("initializer");
  ATO("initializing");
  ATO("main");
  ATO("inputs");
  ATO("to");
  ATO("interface");
  ATO("old");
  ATO("new");
  ATO("\xd8"); // null
  ATO("of");
  ATO("url");
  ATO("on");
  ATO("?");
  ATO("outputs");
  ATO("parameter");
  ATO("qua");
  ATO("quantifier");
  ATO("select");
  ATO("signal");
  ATO("succeed");
  ATO("switch");
  ATO("then");
  ATO("throw");
  ATO("to");
  ATO("transaction");
  ATO("true");
  ATO("try");
  ATO("type");
  ATO("unique");
  ATO("uuid");
  ATO("view");
  ATO("where");
  ATO("with");

  ATO("item");
  ATO("requires");
  ATO("ensures");
  ATO("implies");
  ATO("handler");

  ATO("charConst");
  ATO("floatConst");
  ATO("hexConst");
  ATO("identifier");
  ATO("intConst");
  ATO("octalConst");
  ATO("stringConst");

  ATO("doubleConst");
  ATO("bitConst");
  ATO("else");
  ATO("ifdef");
  ATO("?");

  ATO("<callee>");
  ATO("<comment>");
  ATO("<comp_obj>");
  ATO("ignore");
  ATO("<const>");
  ATO("<creatable>");
  ATO("<creatable>");
  ATO("<creatable>");
  ATO("<event>");
  ATO("<expr>");
  ATO("<expr>");
  ATO("<formparm>");
  ATO("<formparms>");
  ATO("<func>");
  ATO("<func>");
  ATO("<func>");
  ATO("\x0a");
  ATO("\xbb");
  ATO("<var>");
  ATO("<var>");
  ATO("<var>");
  ATO("<setAttrib>");
  ATO("<set>");
  ATO("<stm>");
  ATO("<var>");
  ATO("<type>");
  ATO("<type>");
  ATO("<varName>");
  ATO("<varName>");

  ATO("[type]");
  ATO("[expr]");
  ATO("[var]");
  ATO("?");
  ATO("?");

  ATO("&lt;==");
  ATO("==>");
  ATO("&lt;");
  ATO("&lt;=");
  ATO("&lt;&lt;");

  ATO("end-of-file");
  ATO("no-Token");
}


TokenNode::TokenNode () {
  synObject = 0;
  token = Stm_T;
}
