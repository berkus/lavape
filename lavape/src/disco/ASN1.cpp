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


#include "ASN1.h"
#include "MachDep.h"
#include "Halt.h"

#include <qglobal.h>
#include <qapplication.h>
#include <qmessagebox.h>
#pragma hdrstop


using namespace std;

static const DString ASN1Emsg[]={
  "called within contents field of primitive element",
  "element id's longer than one word are not supported",
  "indefinite contents length not allowed in primitive elements",
  "erroneous definite length field in constructor element",
  "sLength parameter exceeds HIGH(array) parameter",
  "called before reading any ASN.1 header",
  "ASN.1 element stack empty",
  "lengths which do not fit into an integer variable are not supported",
  "integer values longer than one word are not supported",
  "receiving ARRAY OF CHAR too short",
  "ASN.1 element of improper type encountered",
  "composite octet string contains non-octet-string element",
  "negative integer value received"};


static const unsigned contBuffSize = 20;
static const unsigned PrintableString = 4; /* actually 19 */
static const unsigned bitsPerCard=BitsPerCARDINAL;
static DString dummy;

/************************************************************************/


ASN1::ASN1 ()
{ 
  HeaderTrace = false;
  Silent = false;
  topOfStack = 0;
  bottomOfStack = 0;
  skip = false;
  wrongElemStop = true;
  wrongElem = false;
  previewed = false;
  octetStringNestingLevel = 0;
  altTag = false;
  nestedCASEs = 0;
  contentsBuffer.Reset(100);
  wordSize = QSysInfo::WordSize;
  isBigEndian = (QSysInfo::ByteOrder == QSysInfo::BigEndian);
}


ASN1::~ASN1 ()
{
  stackPtr tos, tosNew;

  tos = bottomOfStack;
  while (tos != 0) {
    tosNew = tos->successor;
    delete tos;
    tos = tosNew;
  }
}

/**********************************************************************/


void ASN1::error (ErrCode errCode,
                  const char *callingProcedure)
{
  if ((errCode == WrongElRep)
      && !wrongElemStop) {
    wrongElem = true;
    previewed = true;
    return;
  }

  QString errMsg=QString("Error in input file:\n\nDetails:\n+++++ ")+QString(callingProcedure)+QString(": ")+QString(ASN1Emsg[errCode].c);
  if (!Silent) {
    qDebug(qPrintable(errMsg));
    QMessageBox::critical(0,"Lava/LavaPE",errMsg,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
  }
  errorExitProc();
  skip = true;
}


void ASN1::getHeader (destinationType dT,
          ASN1 *destination,
          DString& s,
          HdrPtr& hdrPtr)
/* needed only internally by GetHeader/CopyElement/GetElement:
   copies header bytes also to destination as a side effect if
   destination # NIL, else to DString s, beginning at position s[s.l] */
{
  unsigned i, k, lenlen;
  stackPtr oldSP;
  unsigned char inChar;

  if (previewed) {
    hdrPtr = &topOfStack->header;
    previewed = false;
    switch (dT) {

    case putProcedure:
      for (k = 0; k <= topOfStack->header.HeaderLen-1; k++)
        destination->putChar(headerBuffer[k]);
      break;

    case buffer:
      if (s.l+topOfStack->header.HeaderLen > s.m)
        s.Expand(s.l+topOfStack->header.HeaderLen - s.m);
      s.Append(DString((char*)headerBuffer,topOfStack->header.HeaderLen));
      break;
    default: ;
    }
    return;
  }

  if ((topOfStack != 0)
      && (topOfStack->header.Form == Primitive))
    error(GetHdrOoc,"GetHeader");
  /* "GetHeader" called out of proper context */

  getChar(inChar);
  if (skip) return;
  switch (dT) {

  case putProcedure:
    destination->putChar(inChar);
    break;

  case buffer:
    if (s.l == s.m)
      if (s.l < 100) s.Expand(100);
      else s.Expand(s.l);
    s[s.l++] = (char)inChar;
    break;

  case none:
    headerBuffer[0] = inChar;
    k = 1;
    break;
  }

  if (bottomOfStack == 0) {
    topOfStack = new stackElem;
    bottomOfStack = topOfStack;
    topOfStack->father = 0;
    topOfStack->explicitAltTag = false;
    bottomOfStack->successor = 0;
  }
  else if (topOfStack == 0)
    topOfStack = bottomOfStack;
  else {
    oldSP = topOfStack;
    topOfStack = topOfStack->successor;
    if (topOfStack == 0) {
      topOfStack = new stackElem;
      oldSP->successor = topOfStack;
      topOfStack->father = oldSP;
      topOfStack->explicitAltTag = false;
      topOfStack->successor = 0;
    }
  }

  previewed = false;
  topOfStack->header.HeaderLen = 1;

  topOfStack->header.Class = TClass(inChar>>6);

  if (inChar & 0x20)
    topOfStack->header.Form = Constructor;
  else
    topOfStack->header.Form = Primitive;

  topOfStack->header.Id = unsigned(inChar & '\x1f');
  if (topOfStack->header.Id == 31) {
    /* multi-octet identifier */
    
    topOfStack->header.Id = 0;
    i = 7; /* # of id bits */
    for (;;) {
      getChar(inChar);
      switch (dT) {

      case putProcedure:
        destination->putChar(inChar);
        break;

      case buffer:
        if (s.l == s.m)
          if (s.l < 100) s.Expand(100);
          else s.Expand(s.l);
        s[s.l++] = (char)inChar;
        break;

      case none:
        headerBuffer[k++] = inChar;
        break;
      }
      topOfStack->header.HeaderLen++;
      topOfStack->header.Id
        = (topOfStack->header.Id<<7)+(inChar & '\x7f');
      if (!(inChar>>7)) break;
      i += 7;
      if (i > bitsPerCard)
        error(IdTooLong,"GetHeader");
    }
  }

  /* extraction of the length field: */
  getChar(inChar);
  switch (dT) {

  case putProcedure:
    destination->putChar(inChar);
    break;

  case buffer:
    if (s.l == s.m)
      if (s.l < 100) s.Expand(100);
      else s.Expand(s.l);
    s[s.l++] = (char)inChar;
    break;

  case none:
    headerBuffer[k++] = inChar;
  }
  
  topOfStack->header.HeaderLen++;
  if (inChar < 128) {
    topOfStack->header.LengthForm = ShortLength;
    topOfStack->header.Len = inChar;
  }
  else if (inChar == 128)
    if (topOfStack->header.Form == Primitive)
      error(IndefLength,"GetHeader");
    else
      topOfStack->header.LengthForm = IndefiniteLength;
  else {
    topOfStack->header.LengthForm = LongLength;
    lenlen = inChar & '\x7f';
    topOfStack->header.Len = 0;
    if (lenlen <= BytesPerCARDINAL) {
      for (i = 1; i <= lenlen; i++) {
        getChar(inChar);
        switch (dT) {

        case putProcedure:
          destination->putChar(inChar);
          break;

        case buffer:
          if (s.l == s.m)
            if (s.l < 100) s.Expand(100);
            else s.Expand(s.l);
          s[s.l++] = (char)inChar;
          break;

        case none:
          headerBuffer[k++] = inChar;
          break;
        }
        topOfStack->header.Len
          = (topOfStack->header.Len<<8)+inChar;
      }
      topOfStack->header.HeaderLen += lenlen;
    }
    else
      error(LenTooLong,"GetHeader");
      /* lengths which do not fit into a unsigned variable
         are not supported */
  }
  topOfStack->header.ContentsBytesRead = 0;

  if (altTag && !implicit)
    topOfStack->explicitAltTag = true;

  if (HeaderTrace)
    PrintHeader();

  hdrPtr = &topOfStack->header; /* actual result */
}


void ASN1::putHeader (Header& header)
{
  unsigned n, len, outCard;
  int i;
  unsigned char outChar;
  unsigned char reverseString[16];

  outCard = (header.Class<<6) + (header.Form<<5);
  if (header.Id < 31) {
    outChar = (unsigned char)(outCard+header.Id);
    putChar(outChar);
  }
  else {
    /* multi-octet identifier */
    outChar = (unsigned char)(outCard+31);
    putChar(outChar);

    reverseString[0] = (unsigned char)(header.Id & 0x7f);
    header.Id = header.Id>>7;
    n = 1;
    while (header.Id > 0) {
      reverseString[n++] = (unsigned char)((header.Id & 0x7f)+0x80);
      header.Id = header.Id>>7;
    }
    for (i = n-1; i >= 0; i--) {
//      outChar = reverseString[i];
      putChar(reverseString[i]);
    }
  }

  /* insertion of the length field: */
  if (header.LengthForm == IndefiniteLength) {
    outChar = (unsigned char)'\x80';
    putChar(outChar);
  }
  else if (header.Len < 128) {
    outChar = (unsigned char)(header.Len);
    putChar(outChar);
  }
  else {
    n = 0;
    len = header.Len;
    do {
      reverseString[n++] = (unsigned char)(len % 256);
      len = len>>8;
    }
    while (len);
    outChar = (unsigned char)(0x80|n);
    putChar(outChar);
    for (i = n-1; i >= 0; i--)
      putChar(reverseString[i]);
  }
}


void ASN1::popUp (unsigned contentsLength)
/* return to the next higher level, i.e., to the containing "father"
   element, i.e., perform a "pop-up" operation on the element stack;
   the length of the popped element is added to "ContentsBytesRead"
   of the father element */
{
  stackPtr oldSP;

  altTag = false;
  if (topOfStack == 0)
    error(StackEmpty,"popUp");
  oldSP = topOfStack;
  topOfStack = topOfStack->father;
  /* return to the containing "father" element */
  if (topOfStack != 0)
    topOfStack->header.ContentsBytesRead
    += oldSP->header.HeaderLen+contentsLength;
  if ((topOfStack) && (topOfStack->explicitAltTag))
    popUp(
        topOfStack->header.HeaderLen
        +topOfStack->header.ContentsBytesRead);
}


bool ASN1::tagOK (TClass asn1Class, unsigned id)
{
  HdrPtr hdrPtr;

  if (altTag)
    if ((topOfStack->header.Class == altClass)
        && (topOfStack->header.Id == altID)) {
      if (implicit) return true;
      GetHeader(hdrPtr);
      if ((hdrPtr->Class == asn1Class)
          && (hdrPtr->Id == id))
        return true;
      else
        return false;
    }
    else
      return false;

  if ((topOfStack->header.Class != asn1Class))
    return false;

  if ((topOfStack->header.Id != 4))
    if ((topOfStack->header.Id == id))
      return true;
    else
      return false;
  else if ((id == 4)
           || (id == PrintableString))
    return true;
  else
    return false;
}


long int ASN1::convLongInteger (const DString& s)
{
  long unsigned card=0;
  unsigned i, signChars;
  unsigned char signChar;

  if ((unsigned char)s[0] < 128)
    signChar = '\0';
  else
    signChar = (unsigned char)('\xff');
  signChars = sizeof(card)-s.l;
  if (signChars > 0)
    for (i = 0; i <= signChars-1; i++)
      card = (card<<8)+signChar;
  for (i = 0; i < s.l; i++)
    card = (card<<8)+(unsigned char)s[i];
  return (long int)card;
}


int ASN1::convInteger (const DString& s)
{
  unsigned card=0;
  unsigned i, signChars;
  unsigned char signChar;

  if ((unsigned char)s[0] < 128)
    signChar = '\0';
  else
    signChar = (unsigned char)('\xff');
  signChars = sizeof(card)-s.l;
  if (signChars > 0)
    for (i = 0; i <= signChars-1; i++)
      card = (card<<8)+signChar;
  for (i = 0; i < s.l; i++)
    card = (card<<8)+(unsigned char)s[i];
  return (int)card;
}


void ASN1::getString (TClass asn1Class,
          unsigned id,
          DString& s,
          bool& eoc,
          unsigned& unusedBits)
{
  bool eoc1;
  bool empty;
  unsigned unusedBits1;
  HdrPtr hdrPtr;
  unsigned char inChar;

  if (!s.l && s.c) s[0] = '\0';
  unusedBits1 = 0;
  getHeader(none,0,dummy,hdrPtr);
  if (skip) return;
  if (octetStringNestingLevel > 0)
    if (EOC()) {
      eoc = true;
      popUp(0);
      popUp(topOfStack->header.ContentsBytesRead);
      return;
    }
    else
      eoc = false;
  if (!tagOK(asn1Class,id))
    if (octetStringNestingLevel > 0)
      error(WrongOctetString,"GetSTRING");
    else {
      error(WrongElRep,"GetSTRING");
      return;
    }
  else
    wrongElem = false;
  if (topOfStack->header.Form == Primitive) {
    if ((asn1Class == Universal)
        && (id == 3)) {
      getChar(inChar);
      unusedBits1 = inChar;
      topOfStack->header.ContentsBytesRead++;
    }
    GetContents(s,empty);
  }
  else {
    octetStringNestingLevel++;
    do
      getString(asn1Class,id,s,eoc1,unusedBits1);
    while (!eoc1);
    octetStringNestingLevel--;
  }
  if (unusedBits1)
    unusedBits = unusedBits1;
}

/**********************************************************************/

//public:


void ASN1::SetSkip (const bool onOff)
{
  skip = onOff;
}


void ASN1::WrongElemStop (const bool onOff)
{
  if (skip) return;
  wrongElemStop = onOff;
}


bool ASN1::WrongElem ()
{
  if (skip) return true;
  return wrongElem;
}


bool ASN1::TagFieldValid ()
{
  if (skip) return true;
  return (nestedCASEs == 1);
}


void ASN1::INCnestedCASEs ()
{
  if (skip) return;
  nestedCASEs++;
}


void ASN1::DECnestedCASEs ()
{
  if (skip) return;
  nestedCASEs--;
}


void ASN1::putChar (const unsigned char&)
{
  Halt.HaltPMD();
}


void ASN1::getChar (unsigned char&)
{
  Halt.HaltPMD();
}


void ASN1::errorExitProc ()
{
  Halt.HaltPMD();
}


void ASN1::CallErrorExit ()
{
  errorExitProc();
  /* should not return normally */
  skip = true;
}


void ASN1::GetHeader (HdrPtr& hdrPtr)
{
  if (skip) return;
  getHeader(none,0,dummy,hdrPtr);
}


void ASN1::Preview (HdrPtr& hdrPtr)
{
  if (skip) return;
  if (previewed) {
    hdrPtr = &topOfStack->header;
    return;
  }
  getHeader(none,0,dummy,hdrPtr);
  previewed = true;
}


void ASN1::CheckType (HdrPtr& hdrPtr,
          TClass asn1Class,
          TForm form,
          unsigned id)
{
  if (skip) return;
  getHeader(none,0,dummy,hdrPtr);
  if (skip) return;
  if (!tagOK(asn1Class,id)
      || (topOfStack->header.Form != form)) {
    error(WrongElRep,"CheckType");
    return;
  }
  else
    wrongElem = false;
}


void ASN1::GetElement (DString& s,
                       bool& eoc)
{
  bool empty;
  HdrPtr hdrPtr;

  if (skip) return;
  getHeader(buffer,0,s,hdrPtr);
  if (skip) return;
  wrongElem = false;
  eoc = EOC();
  GetContents(s,empty);
}


void ASN1::GetEOElem ()
/* reads all concluding EOCs of the current ASN.1 element on the
   highest level; reports an error if a non-EOC element is encountered */
{
  if (skip) return;
  while (topOfStack) {
    GetEOC();
    if (skip) return;
  }
}


void ASN1::SkipElement (bool& eoc)
{
  bool empty;
  HdrPtr hdrPtr;

  if (skip) return;
  getHeader(none,0,dummy,hdrPtr);
  if (skip) return;
  eoc = EOC();
  SkipContents(empty);
}


void ASN1::CopyElement (ASN1 *destination,
                        bool& eoc)
{
  bool empty;
  HdrPtr hdrPtr;

  if (skip) return;
  getHeader(putProcedure,destination,dummy,hdrPtr);
  if (skip) return;
  eoc = EOC();
  CopyContents(destination,empty);
}


void ASN1::GetContents (DString& s,
                        bool& empty)
{
  bool eoc;
  unsigned i;
  unsigned char inChar;

  if (skip) return;
  if (topOfStack == 0)
    error(NoSoc,"GetContents");

  if (topOfStack->header.LengthForm == IndefiniteLength) {
    empty = true;
    do {
      GetElement(s,eoc);
      if (skip) return;
      if (!eoc) empty = false;
    }
    while (!eoc);
  }
  else if (topOfStack->header.Len == 0) {
    empty = true;
    if (EOC()) {
      popUp(0);
      if (skip) return;
      popUp(topOfStack->header.ContentsBytesRead);
      if (skip) return;
    }
    else {
      popUp(0);
      if (skip) return;
    }
  }
  else if (topOfStack->header.ContentsBytesRead > topOfStack->header.Len)
    error(WrongLength,"GetContents");
  else {
    if (topOfStack->header.Len-topOfStack->header.ContentsBytesRead
           > (unsigned)s.m-s.l)
      s.Expand(topOfStack->header.Len-topOfStack->header.ContentsBytesRead
       -(s.m-s.l));
    empty = false;
    if (topOfStack->header.ContentsBytesRead < topOfStack->header.Len)
      for (i = 1; i++ <= topOfStack->header.Len-topOfStack->header.ContentsBytesRead;) {
        getChar(inChar);
        s[s.l++] = (char)inChar;
      }
    popUp(topOfStack->header.Len);
  }
}


void ASN1::SkipContents (bool& empty)
{
  bool eoc;
  unsigned i;
  unsigned char inChar;

  if (skip) return;
  if (topOfStack == 0)
    error(NoSoc,"SkipContents");
  if (topOfStack->header.LengthForm == IndefiniteLength) {
    empty = true;
    do {
      SkipElement(eoc);
      if (skip) return;
      if (!eoc) empty = false;
    }
    while (!eoc);
  }
  else if (topOfStack->header.Len == 0) {
    empty = true;
    if (EOC()) {
      popUp(0);
      if (skip) return;
      popUp(topOfStack->header.ContentsBytesRead);
      if (skip) return;
    }
    else {
      popUp(0);
      if (skip) return;
    }
  }
  else if (topOfStack->header.ContentsBytesRead > topOfStack->header.Len)
    error(WrongLength,"SkipContents");
  else {
    empty = false;
    if (topOfStack->header.ContentsBytesRead < topOfStack->header.Len)
      for (i = 1;
           i++ <= topOfStack->header.Len
           -topOfStack->header.ContentsBytesRead;)
        getChar(inChar);
    popUp(topOfStack->header.Len);
  }
}


void ASN1::SkipObject ()
/* skips the remainder of the current ASN.1 object;
   has no effect if called "between" two objects */
{
  bool empty;

  if (skip) return;
  while (topOfStack) {
    SkipContents(empty);
    if (skip) return;
  }
}


void ASN1::CopyContents (ASN1 *destination,
                         bool& empty)
{
  bool eoc;
  unsigned i;
  unsigned char inChar;

  if (skip) return;
  if (topOfStack == 0)
    error(NoSoc,"CopyContents");
  if (topOfStack->header.LengthForm == IndefiniteLength) {
    empty = true;
    do {
      CopyElement(destination,eoc);
      if (skip) return;
      if (!eoc) empty = false;
    }
    while (!eoc);
  }
  else if (topOfStack->header.Len == 0) {
    empty = true;
    if (EOC()) {
      popUp(0);
      if (skip) return;
      popUp(topOfStack->header.ContentsBytesRead);
      if (skip) return;
    }
    else {
      popUp(0);
      if (skip) return;
    }
  }
  else if (topOfStack->header.ContentsBytesRead > topOfStack->header.Len)
    error(WrongLength,"CopyContents");
  else {
    empty = false;
    if (topOfStack->header.ContentsBytesRead < topOfStack->header.Len)
      for (i = 1; i++ <= topOfStack->header.Len
                 -topOfStack->header.ContentsBytesRead;) {
        getChar(inChar);
        destination->putChar(inChar);
      }
    popUp(topOfStack->header.Len);
  }
}


void ASN1::PutHeader (Header& header)
{
  Header altHeader;

  if (skip) return;
  if (altTag) {
    if (implicit) {
      header.Class = altClass;
      header.Id = altID;
      putHeader(header);
    }
    else {
      altHeader = header;
      altHeader.Class = altClass;
      altHeader.Id = altID;
      if (header.LengthForm == DefiniteLength)
        altHeader.Len += 2;
      putHeader(altHeader);
      putHeader(header);
    }
    altTag = false;
  }
  else
    putHeader(header);
}


void ASN1::PutBytes (const DString& s)
/* copies the first sLength bytes from array s to destination; the
   programmer is himself responsible for the correct structure of the
   resulting ASN.1 stream */
{
  if (skip) return;
  for (unsigned i = 0; i < s.l;)
    putChar((unsigned char)s[i++]);
}


void ASN1::PrintHeader ()
{
  QString msg;

  if (skip) return;
  msg += "::: ";
  if (EOC()) {
    msg += "EOC";
    qDebug(qPrintable(msg));
    return;
  }

  msg += "Class=";
  switch (topOfStack->header.Class) {

  case Universal:
    msg += "Universal";
    break;

  case Application:
    msg += "Application";
    break;

  case Context:
    msg += "Context";
    break;

  case Private:
    msg += "Private";
    break;
  }
  msg += ", Form=";
  switch (topOfStack->header.Form) {

  case Primitive:
    msg += "Primitive";
    break;

  case Constructor:
    msg += "Constructor";
    break;
  }
  msg += ", Id=";
  msg += QString("%1").arg(topOfStack->header.Id);
  if (topOfStack->header.LengthForm != IndefiniteLength) {
    msg += ", Length=";
    msg += QString("%1").arg(topOfStack->header.Len,1);
  }
  qDebug(qPrintable(msg));
}


/* **********************************************************************/


void ASN1::GetSEQUENCE ()
{
  HdrPtr hdrPtr;

  if (skip) return;
  getHeader(none,0,dummy,hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,16)
      || (topOfStack->header.Form != Constructor)) {
    error(WrongElRep,"GetSEQUENCE");
    return;
  }
  else
    wrongElem = false;
  if (HeaderTrace)
    qDebug("::: ASN1.GetSEQUENCE");
}


void ASN1::GetSETTYPE ()
{
  HdrPtr hdrPtr;

  if (skip) return;
  getHeader(none,0,dummy,hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,17)
      || (topOfStack->header.Form != Constructor)) {
    error(WrongElRep,"GetSETTYPE");
    return;
  }
  else
    wrongElem = false;
  if (HeaderTrace)
    qDebug("::: ASN1.GetSEQUENCE");
}


void ASN1::GetEOC ()
{
  HdrPtr hdrPtr;

  if (skip) return;
  getHeader(none,0,dummy,hdrPtr);
  if (skip) return;
  if (!EOC()) {
    error(WrongElRep,"GetEOC");
    return;
  }
  else
    wrongElem = false;
  popUp(0);
  if (skip) return;
  if (topOfStack == 0) {
    error(StackEmpty,"GetEOC");
    return;
  }
  popUp(topOfStack->header.ContentsBytesRead);
  if (HeaderTrace)
    qDebug("::: ASN1.GetEOC");
}


bool ASN1::EOC ()
{
  if (skip) return true;
  return (topOfStack->header.Class == Universal)
         && (topOfStack->header.Id == 0)
         && (topOfStack->header.Form == Primitive)
         && (topOfStack->header.Len == 0);
}


void ASN1::GetNULL ()
{
  HdrPtr hdrPtr;

  if (skip) return;
  getHeader(none,0,dummy,hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,5)
      || (topOfStack->header.Form != Primitive)
      || (topOfStack->header.LengthForm != ShortLength)
      || (topOfStack->header.Len != 0)) {
    error(WrongElRep,"GetNULL");
    return;
  }
  else
    wrongElem = false;
  popUp(0);
  if (HeaderTrace)
    qDebug("::: ASN1.GetNULL");
}


void ASN1::PutSEQUENCE ()
/* is used by the pre-processor to begin a new record or array */
{
  unsigned char outChar;

  if (skip) return;
  outChar = '\x30';
  /* Class := Universal, Form := Constructor, Id := 16*/
  putChar(outChar);
  outChar = (unsigned char)'\x80';
  /* length Form = indefinite */
  putChar(outChar);
  altTag = false;
}


void ASN1::PutSETTYPE ()
/* is used by the pre-processor to begin a new record or array */
{
  unsigned char outChar;

  if (skip) return;
  outChar = '\x31';
  /* Class := Universal, Form := Constructor, Id := 17*/
  putChar(outChar);
  outChar = (unsigned char)'\x80';
  /* length Form = indefinite */
  putChar(outChar);
  altTag = false;
}


void ASN1::PutEOC ()
/* "put end of contents element; is called in order to
   mark the end of a constructor element of indefinite length */
{
  unsigned char outChar;

  if (skip) return;
  outChar = '\0';
  /* end of contents indicator */
  putChar(outChar);
  putChar(outChar);
}


void ASN1::PutNULL ()
{
  unsigned char outChar;

  if (skip) return;
  outChar = '\5';
  /* Class=Universal, Form=Primitive, Id=5 */
  putChar(outChar);
  outChar = '\0';
  /* Length=0 */
  putChar(outChar);
  altTag = false;
}


void ASN1::AltTag (TClass asn1Class,
                   unsigned id,
                   bool impl)
{
  if (altTag) return;
  altTag = true;
  altClass = asn1Class;
  altID = id;
  implicit = impl;
}


void ASN1::PutOptionalAbsent ()
{
  if (skip) return;
  if (!altTag)
    PutNULL();
  else
    altTag = false;
}


bool ASN1::GotOptional ()
{
  HdrPtr hdrPtr;

  if (skip) return true;
  Preview(hdrPtr);
  if (altTag) {
    Preview(hdrPtr);
    if (tagOK(altClass,altID))
      return true;
    else
      return false;
  }

  WrongElemStop(false);
  GetNULL();
  WrongElemStop(true);
  if (WrongElem())
    return true;
  else
    return false;
}


/* **********************************************************************/

/* HIGHER LEVEL PROCEDURES
   (CONVERSION OF ELEMENTARY DATA TYPES): */


void ASN1::GETunsigned (unsigned& x)
{
  int y;

  if (skip) return;
  GETint(y);
  if (skip) return;
  if (wrongElem) return;
  x = (unsigned)y;
}


void ASN1::GETunsigned (long unsigned& x)
{
  long int y;

  if (skip) return;
  GETint(y);
  if (skip) return;
  if (wrongElem) return;
  x = (long unsigned)y;
}


unsigned ASN1::FGETunsigned ()
{
  unsigned card;

  if (skip) return 0;
  GETunsigned(card);
  return card;
}


void ASN1::GETint (long int& x)
{
  HdrPtr hdrPtr;
  bool empty;

  if (skip) return;
  GetHeader(hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,2)
    || (topOfStack->header.LengthForm != ShortLength)) {
    error(WrongElRep,"GETint/GETunsigned");
    return;
  }
  else
    wrongElem = false;
  if (topOfStack->header.Len > BytesPerCARDINAL)
    error(DecimalTooLong,"GETint/GETunsigned");

  contentsBuffer.Reset(100);
  GetContents(contentsBuffer,empty);
  if (skip) return;
  x = convLongInteger(contentsBuffer);
  if (HeaderTrace)
    qDebug(qPrintable(QString("::: ASN1.GETint: ")+QString("%1").arg(int(x))));
}


void ASN1::GETint (int& x)
{
  HdrPtr hdrPtr;
  bool empty;

  if (skip) return;
  GetHeader(hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,2)
    || (topOfStack->header.LengthForm != ShortLength)) {
    error(WrongElRep,"GETint/GETunsigned");
    return;
  }
  else
    wrongElem = false;
  if (topOfStack->header.Len > BytesPerCARDINAL)
    error(DecimalTooLong,"GETint/GETunsigned");

  contentsBuffer.Reset(100);
  GetContents(contentsBuffer,empty);
  if (skip) return;
  x = convInteger(contentsBuffer);
  if (HeaderTrace)
    qDebug(qPrintable(QString("::: ASN1.GETint: ")+QString("%1").arg(int(x))));
}


int ASN1::FGETint ()
{
  long int i;

  if (skip) return 0;
  GETint(i);
  return int(i);
}


void ASN1::GETfloat (float& x)
{
  HdrPtr hdrPtr;
  bool empty;
  unsigned char *floatChar=(unsigned char *)&x;
  unsigned i, len=Release?4:5;

  if (skip) return;
  GetHeader(hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,9)
    || topOfStack->header.LengthForm != ShortLength
    || topOfStack->header.Len != len) {
    error(WrongElRep,"GETfloat/GETdouble");
    return;
  }
  else
    wrongElem = false;

  contentsBuffer.Reset(130);
  GetContents(contentsBuffer,empty);
  if (skip) return;
  if (!Release) {
    if (contentsBuffer[0] != (char)0x80) { // quick and dirty, incorrect ASN1 code!!
      error(WrongElRep,"GETfloat/GETdouble");
      return;
    }
    for (i=0; i<=3; i++)
      *(floatChar+i) = contentsBuffer[i+1];
  }
  else if (isBigEndian)
    for (i=0; i<=3; i++)
      *(floatChar+i) = contentsBuffer[i];
  else
    for (i=0; i<=3; i++)
      *(floatChar+i) = contentsBuffer[3-i];
}


void ASN1::GETdouble (double& x)
{
  HdrPtr hdrPtr;
  bool empty;
  unsigned char *floatChar=(unsigned char *)&x;
  unsigned i, len=Release?8:9;

  if (skip) return;
  GetHeader(hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,9)
    || topOfStack->header.LengthForm != ShortLength
    || topOfStack->header.Len != len) {
    error(WrongElRep,"GETdouble");
    return;
  }
  else
    wrongElem = false;

  contentsBuffer.Reset(130);
  GetContents(contentsBuffer,empty);
  if (skip) return;
  if (!Release) {
    if (contentsBuffer[0] != (char)0x80) { // quick and dirty, incorrect ASN1 code!!
      error(WrongElRep,"GETfloat/GETdouble");
      return;
    }
    for (i=0; i<=7; i++)
      *(floatChar+i) = contentsBuffer[i+1];
  }
  else if (isBigEndian)
    for (i=0; i<=7; i++)
      *(floatChar+i) = contentsBuffer[i];
  else
    for (i=0; i<=7; i++)
      *(floatChar+i) = contentsBuffer[7-i];
}


void ASN1::GETboolean (bool& b)
{
  HdrPtr hdrPtr;
  bool empty;

  if (skip) return;
  GetHeader(hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,1)
      || (topOfStack->header.Form == Constructor)
      || (topOfStack->header.LengthForm != ShortLength)
      || (topOfStack->header.Len != 1)) {
    error(WrongElRep,"GetBOOLEAN");
    return;
  }
  else
    wrongElem = false;
  contentsBuffer.Reset(100);
  GetContents(contentsBuffer,empty);
  if ((unsigned char)contentsBuffer[0] == '\0')
    b = false;
  else
    b = true;
  if (HeaderTrace) {
    if ((unsigned char)contentsBuffer[0] != '\0')
      qDebug("::: ASN1.GetBOOLEAN: true");
    else
      qDebug("::: ASN1.GetBOOLEAN: false");
  }
}


void ASN1::GETset (SET& x)
{
  long unsigned bits;

  if (skip) return;
  GETunsigned(bits);
  x = SET(bits);
}


void ASN1::GETbyte (unsigned char& c)
{
  HdrPtr hdrPtr;
  QString msg;
  bool empty;

  if (skip) return;
  GetHeader(hdrPtr);
  if (skip) return;
  if (!tagOK(Universal,4)
      || (topOfStack->header.Form == Constructor)
      || (topOfStack->header.LengthForm != ShortLength)
      || (topOfStack->header.Len != 1)) {
    error(WrongElRep,"GetCHAR");
    return;
  }
  else
    wrongElem = false;
  contentsBuffer.Reset(100);
  GetContents(contentsBuffer,empty);
  if (skip) return;
  c = (unsigned char)contentsBuffer[0];
  if (HeaderTrace) {
    msg = "::: ASN1.GetBYTE: " + toHex(contentsBuffer.c);
    qDebug(qPrintable(msg));
  }
}


void ASN1::GETbytes (DString& s)
{
  bool eoc;
  unsigned unusedBits;
  QString msg;

  if (skip) return;
  s.l = 0;
  getString(Universal,4,s,eoc,unusedBits);
  if (HeaderTrace) {
    msg = "::: ASN1.GetBYTES: " + toHex(s.c);
    qDebug(qPrintable(msg));
  }
}


void ASN1::GETbits (Bitstring& s)
{
  bool eoc;
  unsigned unusedBits, nBits;
  QString msg;

  if (skip) return;
  unusedBits = 0;
  s.l = 0;
  getString(Universal,3,s,eoc,unusedBits);
  nBits = 8*s.l-unusedBits;
  s.l = nBits;
  if (HeaderTrace) {
    msg = "::: ASN1.GetBITS: " + toHex(s.c);
    qDebug(qPrintable(msg));
  }
}


void ASN1::GETchar (char& c)
{
  HdrPtr hdrPtr;
  bool empty;

  if (skip) return;
  GetHeader(hdrPtr);
  if (!tagOK(Universal,PrintableString)
      || (topOfStack->header.Form == Constructor)
      || (topOfStack->header.LengthForm != ShortLength)
      || (topOfStack->header.Len != 1)) {
    error(WrongElRep,"GetCHAR");
    return;
  }
  else
    wrongElem = false;
  contentsBuffer.Reset(100);
  GetContents(contentsBuffer,empty);
  c = MachDep.FromASCII[(unsigned char)contentsBuffer[0]];
  if (HeaderTrace)
    qDebug(qPrintable(QString("::: ASN1.GetCHAR: \"")+QString(c)+QString("\"")));
}


void ASN1::GETstring (DString& s)
{
  bool eoc;
  unsigned i, unusedBits;

  if (skip) return;
  s.l = 0;
  getString(Universal,PrintableString,s,eoc,unusedBits);
  if (skip) return;
  if (s.l > 0) {
    for (i = 0; i < s.l; i++)
      s[i] = (char)MachDep.FromASCII[(unsigned char)s[i]];
    s[s.l] = '\0';
  }
  if (HeaderTrace)
    qDebug(qPrintable(QString("::: ASN1.GetSTRING: \"")+QString(s.c)+QString("\"")));
}


void ASN1::PUTunsigned (const long unsigned x)
{
  if (skip) return;
  PUTint(x);
}


void ASN1::PUTint (const long int x)
{
  unsigned char signChar, sign;
  bool putSignChar;
  unsigned card, n, i;
  Header header;

  if (skip) return;
  card = unsigned(x);
  if (x < 0) {
    sign = 1;
    signChar = (unsigned char)'\xff';
  }
  else {
    sign = 0;
    signChar = '\0';
  }
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = 2;
  header.LengthForm = ShortLength;

  n = 1;
  while ((hibyte(card) == signChar)
         && (n < BytesPerCARDINAL)) {
    card = card<<8;
    n++;
  }
  n--;
  if ((hibyte(card)>>7) == sign) {
    header.Len = BytesPerCARDINAL-n;
    putSignChar = false;
  }
  else {
    header.Len = BytesPerCARDINAL-n+1;
    putSignChar = true;
  }
  PutHeader(header);
  if (putSignChar)
    putChar(signChar);
  for (i = n+1; i <= BytesPerCARDINAL; i++) {
    putChar(hibyte(card));
    card = card<<8;
  }
}


void ASN1::PUTfloat (const float x)
{
  Header header;
  unsigned char /*outChar,*/ *floatChar=(unsigned char *)&x;
  unsigned i;

  if (skip)
    return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = 9;
  header.LengthForm = DefiniteLength;
  header.Len = 4;
  PutHeader(header);
  if (isBigEndian)
    for (i = 0; i <= 3; i++)
      putChar(*(floatChar+i));
  else
    for (i = 3; i >= 0; i--)
      putChar(*(floatChar+i));
}


void ASN1::PUTdouble (const double x)
{
  Header header;
  unsigned char /*outChar,*/ *floatChar=(unsigned char *)&x;
  unsigned i;

  if (skip)
    return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = 9;
  header.LengthForm = DefiniteLength;
  header.Len = 8;
  PutHeader(header);
  if (isBigEndian)
    for (i = 0; i <= 7; i++)
      putChar(*(floatChar+i));
  else
    for (i = 7; i >= 0; i--)
      putChar(*(floatChar+i));
}


void ASN1::PUTboolean (const bool b)
{
  Header header;

  if (skip) return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = 1;
  /* BOOLEAN */
  header.LengthForm = ShortLength;
  header.Len = 1;
  PutHeader(header);
  if (b) putChar((unsigned char)'\xff');
  else putChar('\0');
}


void ASN1::PUTset (const SET& x)
{
  if (skip) return;
  PUTint((int)x.BITS());
}


void ASN1::PUTbyte (const unsigned char c)
{
  Header header;

  if (skip) return;

  header.Class = Universal;
  header.Form = Primitive;
  header.Id = 4;
  /* octet string */
  header.LengthForm = ShortLength;
  header.Len = 1;
  PutHeader(header);
  putChar(c);
}


void ASN1::PUTbytes (const DString& s)
{
  Header header;
  unsigned i;

  if (skip) return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = 4;
  /* octet string */
  header.LengthForm = DefiniteLength;
  header.Len = s.l;
  PutHeader(header);
  for (i = 0; i < s.l;)
    putChar((unsigned char)s[i++]);
}


void ASN1::PUTbits (const Bitstring& s)
{
  Header header;
  unsigned i, l, unusedBits;

  if (skip) return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = 3;
  /* bit string */
  header.LengthForm = DefiniteLength;
  l = ((unsigned)s.l+7)/8;
  header.Len = l+1;
  PutHeader(header);
  unusedBits = s.l % 8;
  if (unusedBits > 0)
    unusedBits = 8-unusedBits;
  putChar((unsigned char)unusedBits);
  for (i = 0; i < l;)
    putChar((unsigned char)s[i++]);
}


void ASN1::PUTchar (const char c)
{
  Header header;
  unsigned char outChar;

  if (skip)
    return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = PrintableString;
  /* PrintableString */
  header.LengthForm = ShortLength;
  header.Len = 1;
  PutHeader(header);
  outChar = MachDep.ToASCII[(unsigned char)c];
  putChar(outChar);
}


void ASN1::PUTstring (const DString& s)
{
  Header header;
  unsigned char outChar;
  unsigned i;

  if (skip)
    return;
  header.Class = Universal;
  header.Form = Primitive;
  header.Id = PrintableString;
  /* PrintableString */
  header.LengthForm = DefiniteLength;
  header.Len = s.l;
  PutHeader(header);
  if (header.Len > 0)
    for (i = 0; i <= header.Len-1; i++) {
      outChar = MachDep.ToASCII[(unsigned char)s[i]];
      putChar(outChar);
    }
}


QString ASN1::toHex (QString sIn)
{
  QChar ch;
  unsigned char uch;
  QString sOut;

  for (unsigned i = 0; (int)i < sIn.length(); i++) {
	uch = QChar(sIn.at(i)).toAscii();
    uch = uch>>4;
    uch = (uch < 10 ? '0'+uch : 'A'+(uch-10));
    sOut += uch;
    
    uch = QChar(sIn.at(i)).toAscii();
    uch = uch & '\x0f';
    uch = (uch < 10 ? '0'+uch : 'A'+(uch-10));
    sOut += uch;
  }
  return sOut;
}
