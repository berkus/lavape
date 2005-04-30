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


#include "Scanner.h"

#include "CmdParms.h"
#include "CmdError.h"
#include "IntCode.h"
#include "PCerrors.h"
#include "MachDep.h"
#include "DIO.h"
#include "DString.h"
#include <ctype.h>


static bool __INITstarted=false;


static unsigned iString;
static DString msgText;

static unsigned currentCol, currentLine, furtherBlanks;
static char currentChar, subsequentChar;


static void nextChar ();
static void skipBlanksComments ();
static void skipComment ();
static void skip1LineComment ();
static void charConst ();
static void strConst ();
static void number ();
static void fractionPart ();


/************************************************************************/


/* END analyseChars */


/* *********************************************************************/


static inline void putGetChar ()
{
  StringBuffer[iString++] = currentChar;
  nextChar();
}


void NextAtom ()

{
  if (PCerrors.Recovery) return;
  /* after an error no atoms are delivered until error recovery begins */
  if (Atom == Eof) return;
  
  AtomLine = currentLine;
  AtomCol = currentCol;
  if (StringBufferPos >MaxAtomStore-100)
    PCerrors.InternalError(StringBufferOverflow);
  StringBufferPos = iString;

  if (isalpha(currentChar) || (currentChar == '_')) {
    putGetChar();
    for (;;) {
      if (isalnum(currentChar) || (currentChar == '_')) putGetChar();
      else break;
    }
    StringBuffer[iString++] = '\0';
    if (IsReservedWord())
      iString = StringBufferPos;
      /* reset iString, since Atom # identifier */
    else
      Atom = IdentifierA;
  }
  else

    switch (currentChar) {

    case '!':
      nextChar();
      if (currentChar == '=') {
        Atom = NotEqual;
        nextChar();
      }
      else Atom = LogNot;
      break;

    case '%':
      nextChar();
      if (currentChar == '=') {
        Atom = ModulusAssign;
        nextChar();
      }
      else Atom = Modulus;
      break;

    case '^':
      nextChar();
      if (currentChar == '=') {
        Atom = ExclOrAssign;
        nextChar();
      }
      else Atom = ExclOr;
      break;

    case '&':
      nextChar();
      switch (currentChar) {
      case '&':
        Atom = LogAnd;
        nextChar();
        break;
      case '=':
        Atom = AndAssign;
        nextChar();
        break;
      default: Atom = And;
      }
      break;

    case '*':
      nextChar();
      if (currentChar == '=') {
        Atom = MultAssign;
        nextChar();
      }
      else Atom = Mult;
      break;

    case '(':
      Atom = Lparenth;
      nextChar();
      break;

    case ')':
      Atom = Rparenth;
      nextChar();
      break;

    case '-':
      nextChar();
      switch (currentChar) {
      case '>':
        nextChar();
        if (currentChar == '*') {
          Atom = MemberByPointer;
          nextChar();
        }
        else Atom = PointerTo;
        break;
      case '-':
        Atom = MinusMinus;
        nextChar();
        break;
      case '=':
        Atom = MinusAssign;
        nextChar();
        break;
      default: Atom = Minus;
      }
      break;

    case '+':
      nextChar();
      switch (currentChar) {
      case '+':
        Atom = PlusPlus;
        nextChar();
        break;
      case '=':
        Atom = PlusAssign;
        nextChar();
        break;
      default: Atom = Plus;
      }
      break;

    case '=':
      nextChar();
      switch (currentChar) {
      case '=':
        Atom = EqualA;
        nextChar();
        break;
      default: Atom = Assign;
      }
      break;

    case '{':
      Atom = Lbrace;
      nextChar();
      break;

    case '}':
      Atom = Rbrace;
      nextChar();
      break;

    case '|':
      nextChar();
      switch (currentChar) {
      case '|':
        Atom = LogOr;
        nextChar();
        break;
      case '=':
        Atom = OrAssign;
        nextChar();
        break;
      default: Atom = Or;
      }
      break;

    case '~':
      Atom = Not;
      nextChar();
      break;

    case '[':
      Atom = Lbracket;
      nextChar();
      break;

    case ']':
      Atom = Rbracket;
      nextChar();
      break;

    case ';':
      Atom = Semicolon;
      nextChar();
      break;

    case '\'':
      charConst();
      break;

    case ':':
      nextChar();
      if (currentChar == ':') {
        nextChar();
        if (currentChar == '*') {
          Atom = ClassMemberPtr;
          nextChar();
        }
        else Atom = ClassMember;
      }
      else Atom = Colon;
      break;

    case '"':
      strConst();
      break;

    case '<':
      nextChar();
      switch (currentChar) {
      case '<':
        nextChar();
        if (currentChar == '=') {
          Atom = LshiftAssign;
          nextChar();
        }
        else Atom = Lshift;
        break;
      case '=':
        Atom = LessEqual;
        nextChar();
        break;
      default: Atom = LessThan;
      }
      break;

    case '>':
      nextChar();
      switch (currentChar) {
      case '>':
        nextChar();
        if (currentChar == '=') {
          Atom = RshiftAssign;
          nextChar();
        }
        else Atom = Rshift;
        break;
      case '=':
        Atom = GreaterEqual;
        nextChar();
        break;
      default: Atom = GreaterThan;
      }
      break;

    case '?':
      Atom = QuestionMark;
      nextChar();
      break;

    case ',':
      Atom = Comma;
      nextChar();
      break;

    case '.':
      if (isdigit(subsequentChar)) number();
      else {
        nextChar();
        if (currentChar == '*') {
          Atom = MemberByOffset;
        nextChar();
        } 
        else Atom = Period;
      }
      break;

    case '/':
      nextChar();
      if (currentChar == '=') {
        Atom = DivAssign;
        nextChar();
        break;
      }
      else Atom = Slash;
      break;

    case '$':
      Atom = PPescape;
      nextChar();
      break;

    case '#':
      Atom = NumberSign;
      nextChar();
      break;

    case EOF:
      Atom = Eof;
      break;

    default:
      if (isdigit(currentChar))
        number();
      else {
        PCerrors.LexicalError(IllegalChar);
        nextChar();
      }
    }

  skipBlanksComments();
}


static void specialChar ()

{
  nextChar();
  switch (currentChar) {
  case 'n':
  case 't':
  case 'v':
  case 'b':
  case 'r':
  case 'f':
  case 'a':
  case '\\':
  case '?':
  case '\'':
  case '\"':
    StringBuffer[iString++] = currentChar;
    nextChar();
    break;
    
  case 'x':
    StringBuffer[iString++] = currentChar;
    nextChar();
    while (isxdigit(currentChar)) putGetChar();
    break;
  
  default:
    while (isdigit(currentChar) && (currentChar<'8')) putGetChar();
  }
}


static void skipSpecialChar ()

{
  nextChar();
  switch (currentChar) {
  case 'n':
  case 't':
  case 'v':
  case 'b':
  case 'r':
  case 'f':
  case 'a':
  case '\\':
  case '?':
  case '\'':
  case '\"':
    nextChar();
    break;
    
  case 'x':
    nextChar();
    while (isxdigit(currentChar)) nextChar();
    break;
  
  default:
    while (isdigit(currentChar) && (currentChar<'8')) nextChar();
  }
}


static void charConst ()

{
  char oldCh;

  Atom = CharConst;
  CommentLine = currentLine;
  CommentCol = currentCol;
  /* keep position for potential error messages */
  oldCh = currentChar;
  putGetChar();
  for (;;) {
    switch (currentChar) {
    case '\\':
      StringBuffer[iString++] = currentChar;
      specialChar();
      break;
    case EOL:
      PCerrors.LexicalError(StringExceedsLine);
      goto exit;
    default:
      if (currentChar == oldCh) goto exit;
      else putGetChar();
    }
  } exit:
  putGetChar();
  StringBuffer[iString++] = '\0';
}


static void strConst ()

{
  char oldCh;

  Atom = StringConst;
  CommentLine = currentLine;
  CommentCol = currentCol;
  /* keep position for potential error messages */
  oldCh = currentChar;
  nextChar();
  for (;;) {
    switch (currentChar) {
    case '\\':
      StringBuffer[iString++] = currentChar;
      specialChar();
      break;
    case EOL:
      PCerrors.LexicalError(StringExceedsLine);
      return;
    default:
      if (currentChar == oldCh) goto exit;
      else putGetChar();
    }
  } exit:
  nextChar();
  StringBuffer[iString++] = '\0';
}


static void number ()

{
  StringBuffer[iString++] = currentChar;
  switch (currentChar) {
  case '0':
    nextChar();
    if ((currentChar == 'x')
    || (currentChar == 'X')) {
      Atom = HexConst;
      putGetChar();
      if (!isxdigit(currentChar)) {
        PCerrors.LexicalError(WrongHexadecimal);
        return;
      }
      while (isxdigit(currentChar)) putGetChar();
    }
    else
      if (isdigit(currentChar)) {
        Atom = OctalConst;
        while (isdigit(currentChar)) {
          if (currentChar >= '8') {
            PCerrors.LexicalError(WrongOctal);
            return;
          }
          putGetChar();
        }
      }
      else Atom = IntConst;
    switch (currentChar) {
    case 'l':
    case 'L':
      putGetChar();
      switch (currentChar) {
      case 'u':
      case 'U':
        putGetChar();
      }
      break;
    case 'u':
    case 'U':
      putGetChar();
      switch (currentChar) {
      case 'l':
      case 'L':
        putGetChar();
      }
    }
    StringBuffer[iString++] = '\0';
    break;
    
  case '.':
    fractionPart();
    break;
  
  default:
    nextChar();
    while (isdigit(currentChar)) putGetChar();
    if (currentChar == '.') fractionPart();
    else {
      Atom = IntConst;
      StringBuffer[iString++] = '\0';
    }
  }
}


static void fractionPart ()
{
  Atom = FloatConst;
  putGetChar();
  while (isdigit(currentChar)) putGetChar();
  if ((currentChar != 'e')
  && (currentChar != 'E')) {
    StringBuffer[iString++] = '\0';
    return;
  }
  putGetChar();
  if ((currentChar == '+')
  || (currentChar == '-'))
    putGetChar();
  if (!isdigit(currentChar)) {
    PCerrors.LexicalError(WrongExponent);
    return;
  }
  while (isdigit(currentChar)) putGetChar(); // exponent digits
  switch (currentChar) {
  case 'l':
  case 'L':
  case 'f':
  case 'F':
    putGetChar();
  }
  StringBuffer[iString++] = '\0';
}


void SkipUntilPPescape ()

{
  char oldCh;

  if (Atom != PPescape)
    for (;;)
      switch (currentChar) {

      case '$':
        Atom = PPescape;
        AtomLine = currentLine;
        AtomCol = currentCol;
        nextChar();
        goto exit;

      case '/':
  switch (subsequentChar) {
  case '*':
          CommentLine = currentLine;
          CommentCol = currentCol;
          /* keep position for potential error messages */
          nextChar();
          nextChar();
          skipComment();
    break;

  case '/':
          CommentLine = currentLine;
          CommentCol = currentCol;
          /* keep position for potential error messages */
          nextChar();
          nextChar();
          skip1LineComment();
    break;
    
  default:
          nextChar();
        }
        break;

      case '"':
      case '\'':
        oldCh = currentChar;
        CommentLine = currentLine;
        CommentCol = currentCol;
        nextChar();
        for (;;) {
    switch (currentChar) {
    case '\\':
      skipSpecialChar();
      break;
    case EOL:
      PCerrors.LexicalError(StringExceedsLine);
      goto exit2;
    default:
      if (currentChar == oldCh) goto exit2;
      else nextChar();
    }
  } exit2:
        nextChar();
        break;

      case EOF:
        Atom = Eof;
        AtomLine = currentLine;
        AtomCol = currentCol;
        goto exit;

      default:
        nextChar();
      }
      exit: /* LOOP*/

  if (PCerrors.Recovery) {
    PCerrors.Recovery = false;
    IntCode.ResetStack();
  }
}


void Check (AtomType at)

{
  if (Atom != at) {
    msgText = "\"";
    msgText += ATOMSTR[at];
    msgText += "\"";
    PCerrors.Expected(msgText);
  }
  NextAtom();
}


void CheckPush (AtomType at)

{
  if (Atom != at) {
    msgText = "\"";
    msgText += ATOMSTR[at];
    msgText += "\"";
    PCerrors.Expected(msgText);
    return;
  }
  IntCode.PushAtom();
  NextAtom();
}

void nextChar ()

{
  if (currentChar == EOF)
    return;

  if (currentChar == EOL) {
    currentLine++;
    currentCol = 1;
  }
  else
    currentCol++;

  if (furtherBlanks)
    furtherBlanks--;
    /* currentChar is already = ' ' */
  else {
    currentChar = subsequentChar;
    while ((currentChar == CR) && IO.Done)
      IO.Read(currentChar);
    if (!IO.Done) currentChar = EOF;
    else {
      IO.Read(subsequentChar);
      if (!IO.Done) subsequentChar = EOF;
    }
    if (currentChar == HT) {
      currentChar = ' ';
      furtherBlanks = BlanksPerTab-((currentCol-1) % BlanksPerTab+1);
    }
  }
}


void skipBlanksComments ()

{
  for (;;)
    switch (currentChar) {

    case ' ':
    case EOL:
      do
        nextChar();
      while ((currentChar == ' ') || (currentChar == EOL));
      break;

    case '/':
      switch (subsequentChar) {
      case '*':
  CommentLine = currentLine;
  CommentCol = currentCol;
  /* keep position for potential error messages */
  nextChar();
  nextChar();
  skipComment();
  break;

      case '/':
  CommentLine = currentLine;
  CommentCol = currentCol;
  /* keep position for potential error messages */
  nextChar();
  nextChar();
  skip1LineComment();
  break;

      default: return;
      }
      break;

    default:
      return;
    }
}


void skipComment ()

{
  for (;;)
    switch (currentChar) {

    case '*':
      if (subsequentChar == '/') {
        nextChar();
        nextChar();
        return;
      }
      else
        nextChar();
      break;

    case EOF:
      PCerrors.LexicalError(OpenComment);
      return;

    default:
      nextChar();
    }
}


void skip1LineComment ()

{
  for (;;)
    switch (currentChar) {

    case EOL:
      nextChar();
      return;

    case EOF:
      return;

    default:
      nextChar();
    }
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/


void init ()
{
  do
    IO.Read(currentChar);
  while (IO.Done && (currentChar == CR));
  if (!IO.Done)
    CmdError.CommandError(SourceEmpty);
  IO.Read(subsequentChar);
  if (!IO.Done)
    subsequentChar = EOF;
  furtherBlanks = 0;
  currentCol = 1;
  currentLine = 1;

  skipBlanksComments();
  NextAtom();
}


void Scanner_INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  Atoms_INIT();
  CmdParms.INIT();
  CmdError.INIT();
  IntCode.INIT();
  PCerrors.INIT();
  MachDep.INIT();
  IO.INIT();

  IO.OpenInput(CmdParms.SourceFile);
  if (!IO.Done)
    CmdError.CommandError(OpenErrorSource);
  init();
  iString = 0;
}


void SaveSourcePosition (SourcePosition& sp)

{
  sp.Atom = Atom;
  sp.AtomLine = AtomLine;
  sp.AtomCol = AtomCol;
  sp.currentLine = currentLine;
  sp.currentCol = currentCol;
  sp.furtherBlanks = furtherBlanks;
  sp.currentChar = currentChar;
  sp.subsequentChar = subsequentChar;

  init();
}

void RestoreSourcePosition (SourcePosition& sp)

{
  Atom = sp.Atom;
  AtomLine = sp.AtomLine;
  AtomCol = sp.AtomCol;
  currentLine = sp.currentLine;
  currentCol = sp.currentCol;
  furtherBlanks = sp.furtherBlanks;
  currentChar = sp.currentChar;
  subsequentChar = sp.subsequentChar;
}


