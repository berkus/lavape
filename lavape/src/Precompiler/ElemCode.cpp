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


/**********************************************************************

               implementation of class ElemCodeCLASS

 **********************************************************************/

#include "SYSTEM.h"

#include "ElemCode.h"

#include "PCerrors.h"

#include "DIO.h"

#include "DString.h"

#include "MachDep.h"


DString designator(200);
unsigned sourceLine, sourceCol;

typedef struct TADstackElem *TADstackPtr;

struct TADstackElem {
  TADstackPtr father;
  /* points to the preceding stack element */
  TADstackPtr successor;
  /* points to the next stack element */
  int dir;
  unsigned steps;
};

static unsigned indent, codePos;

static const unsigned indentationIncrement = 2;


/* **********************************************************************/
/* auxiliary procedures for indent and code generation: */


static TADstackPtr topOfTADStack;
/* stack pointer: points to the top element of the TADstack */
static TADstackPtr bottomOfTADStack;
/* points to the first element of the TADstack */


void newTopOfTADStack ();


void flushAddressStack ();


static bool __INITstarted=false;


static const char blanks2[]="  ";
static const char blanks4[]="    ";
static const char blanks6[]="      ";
static const char blanks8[]="        ";
static const char blanks10[]="          ";


/************************************************************************/

void incIndent ()
/* increment indent */

{
  indent += indentationIncrement;
}


void decIndent ()
/* decrement indent */

{
  if (topOfTADStack)
    flushAddressStack();
  if (indent >= indentationIncrement)
    indent -= indentationIncrement;
}


void indentHere (unsigned& savedInd)
/* set new indentation column before the current code position */

{
  savedInd = indent;
  indent = codePos;
}


void restoreIndent (unsigned savedInd)

{
  indent = savedInd;
}


/************************************************************************/

void writeIndent ()

{
  unsigned i;

  if ((codePos == 0)
      && (indent > 0)) {
    switch (indent) {

    case 2:
      IO.WriteString(blanks2);
      break;

    case 4:
      IO.WriteString(blanks4);
      break;

    case 6:
      IO.WriteString(blanks6);
      break;

    case 8:
      IO.WriteString(blanks8);
      break;

    case 10:
      IO.WriteString(blanks10);
      break;

    default:
      for (i = 1; i <= indent; i++)
        IO.Write(' ');
    }
    codePos = indent;
  }
}


void writeLn ()

{
  IO.WriteLn();
  codePos = 0;
}


void code (const char *text)
/* put constant text code */

{
  writeIndent();
  IO.WriteString(text);
  codePos += IO.Length;
}


void codeNl (const char *text)
/* put constant text code and then new line */

{
  writeIndent();
  IO.WriteString(text);
  codePos += IO.Length;
  writeLn();
}


void nlCode (const char *text)
/* put new line and then constant text code */

{
  writeLn();
  writeIndent();
  IO.WriteString(text);
  codePos += IO.Length;
}


void codeCard (unsigned c)
/* put CARDINAL */

{
  writeIndent();
  IO.WriteCard(c,1);
  codePos += IO.Length;
}


void codeVariable (TStringBufferPos sbp)
/* put string data from StringBuffer */

{
  AtomPtr varPtr;

  writeIndent();
  varPtr = &StringBuffer[sbp];
  IO.WriteString(varPtr);
  codePos += IO.Length;
}


void codeAtom (TreeNodePtr node)

{
  writeIndent();
  
  switch (node->Atom) {
  case IdentifierA:
  case CharConst:
  case IntConst:
  case OctalConst:
  case HexConst:
  case FloatConst:
  case StringConst:
    codeVariable(node->StringBufferPos);
    break;

  default:
    IO.WriteString(ATOMSTR[node->Atom]);
  }
  
  codePos += IO.Length;
}


/************************************************************************/

void newTopOfTADStack ()

{
  TADstackPtr oldSP;

  if (bottomOfTADStack == 0) {
    topOfTADStack = new TADstackElem;
    bottomOfTADStack = topOfTADStack;
    topOfTADStack->father = 0;
    bottomOfTADStack->successor = 0;
  }
  else if (topOfTADStack == 0)
    topOfTADStack = bottomOfTADStack;
  else {
    oldSP = topOfTADStack;
    topOfTADStack = topOfTADStack->successor;
    if (topOfTADStack == 0) {
      topOfTADStack = new TADstackElem;
      oldSP->successor = topOfTADStack;
      topOfTADStack->father = oldSP;
      topOfTADStack->successor = 0;
    }
  }
}


void up ()

{
  if ((topOfTADStack)
      && (topOfTADStack->dir == 0))
    topOfTADStack = topOfTADStack->father;
  if (topOfTADStack == 0) {
    newTopOfTADStack();
    topOfTADStack->dir = -1;
    topOfTADStack->steps = 1;
  }
  else if (topOfTADStack->dir == 1 /* down */
    )
    if (topOfTADStack->steps > 1)
      topOfTADStack->steps--;
    else
      topOfTADStack = topOfTADStack->father;
  else /* up */
    topOfTADStack->steps++; /* one more step up */
}


void down ()

{
  if ((topOfTADStack == 0)
      || (topOfTADStack->dir != 1)) {
    newTopOfTADStack();
    topOfTADStack->dir = 1;
    topOfTADStack->steps = 1;
  }
  else
    topOfTADStack->steps++;
}


void right (unsigned steps)

{
  if ((topOfTADStack == 0)
      || (topOfTADStack->dir)) {
    newTopOfTADStack();
    topOfTADStack->dir = 0;
    topOfTADStack->steps = steps;
  }
  else
    topOfTADStack->steps += steps;
}


void flushAddressStack ()

{
  TADstackPtr stackPtr=bottomOfTADStack, tos=topOfTADStack;
  bool firstTime=true;

  if (!tos) return;
  
  topOfTADStack = 0;
  for (;;) {
    switch (stackPtr->dir) {

    case -1:
      if (firstTime)
        code("FIOpp.UP(");
      else
        code(" FIOpp.UP(");
      break;

    case 0:
      if (firstTime)
        code("FIOpp.RIGHT(");
      else
        code(" FIOpp.RIGHT(");
      break;

    case 1:
      if (firstTime)
        code("FIOpp.DOWN(");
      else
        code(" FIOpp.DOWN(");
      break;
    }
    codeCard(stackPtr->steps);
    code(");");
    firstTime = false;
    if (stackPtr == tos)
      break;
    else
      stackPtr = stackPtr->successor;
  }
  writeLn();
}


void appendIdToDesignator (TStringBufferPos sbp)
/* append identifier to designator */

{
  if (designator.Length() > 2)
    designator += ".";
  else designator += "->";
  
  designator.Append(&StringBuffer[sbp]);
}


void appendIndexToDesignator (TStringBufferPos sbp)
/* append array index to designator */

{
  designator += "[";
  designator.Append(&StringBuffer[sbp]);
  designator += ']';
}


void restoreDesignator (unsigned savedDesLength)
/* restore the original designator */

{
  designator[savedDesLength] = '\0';
  designator.l = savedDesLength;
}


void copyText (TreeNodePtr upToNode,
               bool lastCharToo)
/* copy contiguous non-pp statements to output file */

{
  char inputLine[300];
  char ch;
  unsigned lastPos;
  bool preserveCodePos;

  if (lastCharToo)
    lastPos = upToNode->SourceCol+1;
  else
    lastPos = upToNode->SourceCol;
  if (sourceLine < upToNode->SourceLine)
    preserveCodePos = true;
  else
    preserveCodePos = false;

  for (;;) {
    IO.Read(ch);
    switch (ch) {

    case ' ':
      sourceCol++;
      break;

    case HT:
      sourceCol += BlanksPerTab-((sourceCol-1) % BlanksPerTab);
      break;

    case EOF:
      return;

    case EOL:
      sourceLine++;
      sourceCol = 1;
      goto exit;

    default:
      goto exit;
    }
  } exit:

  if (ch != EOL) {
    IO.Again();
    if (sourceLine < upToNode->SourceLine) {
      IO.ReadLn(inputLine);
      if (!IO.Done)
        PCerrors.InternalError(InputBufferOfl);
      codeNl(inputLine);
      sourceLine++;
      sourceCol = 1;
    }
    else
      writeIndent();
  }

  while (sourceLine < upToNode->SourceLine) {
    IO.ReadLn(inputLine);
    IO.WriteString(inputLine);
    IO.WriteLn();
    sourceLine++;
    sourceCol = 1;
  }

  while (sourceCol < lastPos) {
    IO.Read(ch);
    if (ch == HT)
      sourceCol += BlanksPerTab-((sourceCol-1) % BlanksPerTab);
    else
      sourceCol++;
    IO.Write(ch);
  }
  if (preserveCodePos
      || lastCharToo)
    codePos = sourceCol-1;
  else {
    writeLn();
    writeIndent();
  }
}


void skipText (TreeNodePtr upToNode)
/* used to skip pre-processor macro up to and including the semicolon */

{
  char inputLine[300];
  char ch;

  if (sourceLine < upToNode->SourceLine) {
    while (sourceLine < upToNode->SourceLine) {
      IO.ReadLn(inputLine);
      sourceLine++;
    }
    sourceCol = 1;
  }

  while (sourceCol <= upToNode->SourceCol) {
    IO.Read(ch);
    if (ch == HT)
      sourceCol += BlanksPerTab-((sourceCol-1) % BlanksPerTab);
    else
      sourceCol++;
  }
}


void skipIncludeMacro (TreeNodePtr hFileNode)
/* used to skip $INCLUDE macro up to and including the concluding '"'
   of the include file name */

{
  DString hFileName(&StringBuffer[hFileNode->StringBufferPos]);
  char ch;

  skipText(hFileNode);
  for (unsigned i=0; i < (unsigned)hFileName.l+1; i++) {
    IO.Read(ch);
    if (ch == HT)
      sourceCol += BlanksPerTab-((sourceCol-1) % BlanksPerTab);
    else
      sourceCol++;
  }
}

/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void EC_INIT ()
{ /* ElemCode */
  if (__INITstarted) return;
  __INITstarted = true;

  Atoms_INIT();
  IntCode.INIT();
  PCerrors.INIT();
  IO.INIT();
  MachDep.INIT();

  indent = 0;
  codePos = 0;
  sourceLine = 1;
  sourceCol = 1;
  topOfTADStack = 0;
  bottomOfTADStack = 0;
}
