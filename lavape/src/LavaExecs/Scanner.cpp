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


#include <limits>
#include "Scanner.h"
#include "LavaBaseStringInit.h"

#include "MachDep.h"
#include "DString.h"
#include "qstring.h"
#include <ctype.h>
#include <errno.h>

#pragma hdrstop


using namespace std;


static int *iStringp;
static QString *strp;
static TToken *tokenp;
QString **msgp;

static QChar currentChar, subsequentChar;


static void init ();
static void nextChar ();
static bool charConst ();
static bool strConst ();
static bool number ();
static bool fractionPart ();


bool IsOK (QString &txt,TToken &token,int &pos,QString *&msg, CComboBar *comboBar)

{
  bool ok;
  strp = &txt;
  iStringp = &pos;
  msgp = &msg;
  tokenp = &token;

  init();

  if (currentChar.isLetter()) {
    if (token == Const_T) {
      *msgp = &ERR_const_expected;
      return false;
    }
    nextChar();
    for (;;) {
      if (currentChar.isLetterOrNumber() || (currentChar == '_')) nextChar();
      else break;
    }
    token = Identifier_T;
    if (currentChar != '\0') {
      *msgp = &ERR_Odd_char;
      return false;
    }
    else if (txt == "self") {
      *msgp = &ERR_ReservedName;
      return false;
    }
    else {
      QString cstr(txt);
      if (cstr.indexOf("temp") >= 0) {
        cstr = cstr.mid(4);
        if (cstr.isEmpty()) {
          *msgp = &ERR_ReservedName;
          return false;
        }
        else {
          long l = cstr.toLong(&ok);
          //QString span=cstr.SpanIncluding("0123456789");
          //if (span.length() == cstr.length()) {
          if (ok) {
            *msgp = &ERR_ReservedName;
            return false;
          }
          else if (false/*comboBar->UsedName(DString(txt))*/) {
            *msgp = &ERR_NameInUse;
            return false;
          }
        }
      }
      else if (comboBar->UsedName(qPrintable(txt))) {
        *msgp = &ERR_NameInUse;
        return false;
      }
    }
    return true;
  }
  else {
    if (token == VarName_T) {
      *msgp = &ERR_Var_expected;
      return false;
    }
    switch (currentChar.toLatin1()) {
    case '.':
      return number();

    case '\'':
      return charConst();

    case '"':
      return strConst();

    default:
      if (currentChar.isDigit())
        return number();
      else {
        *msgp = &ERR_Odd_char;
        return false;
      }
    }
  }
  return true;
}


static bool specialChar ()

{
  nextChar();
  switch (currentChar.toLatin1()) {
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
    break;
    
  case 'x':
    nextChar();
    if (!isxdigit(currentChar.toLatin1())) {
      *msgp = &ERR_Wrong_spcl_char;
      return false;
    }
    while (isxdigit(currentChar.toLatin1())) nextChar();
    break;
  
  default:
    if (!currentChar.isDigit() || (currentChar>='8')) {
      *msgp = &ERR_Wrong_spcl_char;
      return false;
    }
    while (currentChar.isDigit() && (currentChar<'8')) nextChar();
  }
  return true;
}


static bool charConst ()

{
  char oldCh;

  *tokenp = CharConst_T;
  oldCh = currentChar.toLatin1();
  nextChar();
  if (currentChar == '\\' && !specialChar())
    return false;
  nextChar();
  if (currentChar != oldCh) {
    *msgp = &ERR_Apostr_missing;
    return false;
  }
  nextChar();
  if (currentChar != '\0') {
    *msgp = &ERR_Odd_char;
    return false;
  }
  return true;
}


static bool strConst ()

{
  char oldCh;

  *tokenp = StringConst_T;
  oldCh = currentChar.toLatin1();
  nextChar();
  for (; currentChar != '\0';) {
    switch (currentChar.toLatin1()) {
    case '\\':
      if (!specialChar())
        return false;
      nextChar();
      if (currentChar == oldCh) goto exit;
      break;
    default: ;
      if (currentChar == oldCh) goto exit;
      nextChar();
    }
  } exit:
  if (currentChar == '\0') {
    *msgp = &ERR_QMark_missing;
    return false;
  }
  nextChar();
  if (currentChar != '\0') {
    *msgp = &ERR_Odd_char;
    return false;
  }
  return true;
}


static bool number ()

{
  int n;
  float f;
  double d;

  switch (currentChar.toLatin1()) {
  case '0':
    nextChar();
    if ((currentChar == 'x')
    || (currentChar == 'X')) {
      *tokenp = HexConst_T;
      nextChar();
      if (!isxdigit(currentChar.toLatin1())) {
        *msgp = &ERR_Wrong_hex;
        return false;
      }
      while (isxdigit(currentChar.toLatin1())) nextChar();
      if (strp->length()-2 > 2*sizeof(unsigned)) {
        *msgp = &ERR_HexTooLong;
        return false;
      }
    }
    else if ((currentChar == 'y')
    || (currentChar == 'Y')) {
      *tokenp = BitConst_T;
      nextChar();
      if (!isxdigit(currentChar.toLatin1())) {
        *msgp = &ERR_Wrong_hex;
        return false;
      }
      while (isxdigit(currentChar.toLatin1())) nextChar();
      if (currentChar != '\0') {
        *msgp = &ERR_Odd_char;
        return false;
      }
      if (strp->length()-2 > 2*sizeof(unsigned)) {
        *msgp = &ERR_HexTooLong;
        return false;
      }
      return true; // overflow check not required
    }
    else if (currentChar.isDigit()) {
      *tokenp = OctalConst_T;
      while (currentChar.isDigit()) {
        if (currentChar >= '8') {
          *msgp = &ERR_Wrong_oct;
          return false;
        }
        nextChar();
      }
    }
    else if (currentChar == '.') {
      if (!fractionPart())
        return false;
    }
    else
      *tokenp = IntConst_T;
/*
    switch (currentChar) {
    case 'l':
    case 'L':
      ulCount++;
      nextChar();
      switch (currentChar) {
      case 'u':
      case 'U':
        ulCount++;
        nextChar();
      }
      break;
    case 'u':
    case 'U':
      ulCount++;
      nextChar();
      switch (currentChar) {
      case 'l':
      case 'L':
        ulCount++;
        nextChar();
      }
    }
*/
    break;
    
  case '.':
    if (!fractionPart())
      return false;
    break;

  default:
    while (currentChar != '\0' && currentChar.isDigit())
      nextChar();
    switch (currentChar.toLatin1()) {
    case '.':
      if (!fractionPart())
        return false;
      break;
    case '\0':
      *tokenp = IntConst_T;
      break;
    default:
      *msgp = &ERR_Odd_char;
      return false;
    }
  }

  if (currentChar != '\0') {
    *msgp = &ERR_Odd_char;
    return false;
  }

// over/underflow check:
  errno = 0;
  switch (*tokenp) {
  case IntConst_T:
  case OctalConst_T:
    n = strp->toInt();
//    n = strtol(strp->ascii(),0,0);
    break;
  case FloatConst_T:
    f = strp->toFloat();
//    f = strtod(strp->ascii(),0);
    if (errno == ERANGE
      || f == numeric_limits<float>::infinity()) {
      *msgp = &ERR_OverOrUnderflow;
      return false;
    }
    break;
  case DoubleConst_T:
    d = strp->toDouble();
//    d = strtod(strp->ascii(),0);
    if (errno == ERANGE) {
      *msgp = &ERR_OverOrUnderflow;
      return false;
    }
    break;
  }
  return true;
}


static bool fractionPart ()
{
  *tokenp = DoubleConst_T;
  nextChar();
  while (currentChar.isDigit()) nextChar();
  if (currentChar == '\0')
    return true;
  if (currentChar == 'e'
  || currentChar == 'E') {
    nextChar();
    if (currentChar == '+'
    || currentChar == '-')
      nextChar();
    if (!currentChar.isDigit()) {
      *msgp = &ERR_Wrong_exp;
      return false;
    }
    while (currentChar.isDigit()) nextChar(); // exponent digits
  }
  if (currentChar == '\0')
    return true;
  switch (currentChar.toLatin1()) {
//  case 'l':
//  case 'L': LongDouble_T
  case 'f':
  case 'F':
  *tokenp = FloatConst_T;
    nextChar();
    if (currentChar == '\0')
      return true;
    break;
  default: ;
  }
  *msgp = &ERR_Odd_char;
  return false;
}


static void nextChar ()

{
  if (currentChar == '\0')
    return;

  currentChar = subsequentChar;
  if (++(*iStringp) < (int)strp->length())
    subsequentChar = strp->at(*iStringp);
  else
    subsequentChar = '\0';
}


static void init ()
{
  *iStringp = 1;
  currentChar = (*strp)[0];
  if (strp->length() > 1)
    subsequentChar = (*strp)[1];
  else
    subsequentChar = '\0';
  return;
}

