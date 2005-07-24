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


#include "MACROS.h"
#include "DIO.h"
#include "string.h"
#include "GUIProg.h"
#include "Conv.h"
#include "qmessagebox.h"


/***************************************************************************/
/* Convert from  DString to Integer                                         */
/***************************************************************************/

ConvCLASS Conv;

void ConvCLASS::StringToInt (const DString& str,
                                 int& num,
                                 bool& done)
{
  unsigned ix, c;

  done = ConvToNum(str,10,true,c);
  if (done) {
    ix = 0;
    while (str.c[ix] == ' ')
      ix++;
    if (str.c[ix] == '-')
      if (c >= MaxIntAbs) {
        done = false;
        GUIProg->ErrPos = 0;
        GUIProg->ErrorCode = TruncationInt;
        return;
      }
      else if (c <= MaxIntAbs)
        num = -int(c);
      else num = MinInt;
    else {
      if (c > MaxIntAbs) {
        done = false;
        GUIProg->ErrPos = 0;
        GUIProg->ErrorCode = TruncationInt;
        return;
      }
      num = int(c);
    }
  }
} // END OF StringToInt


/***************************************************************************/
/* Convert from  DString to Boolean                                         */
/***************************************************************************/
void ConvCLASS::StringToBool (const DString& str,
                                  bool& boole,
                                  bool& done)
{
  unsigned i;
  bool tog;

  tog = done;
  if (str.Contains("T",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("t",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("J",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("j",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("W",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("w",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("Y",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("y",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("X",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("x",0,i)) {
    boole = true;
    done = true;
  }
  else if (str.Contains("F",0,i)) {
    boole = false;
    done = true;
  }
  else if (str.Contains("f",0,i)) {
    boole = false;
    done = true;
  }
  else if (str.Contains("N",0,i)) {
    boole = false;
    done = true;
  }
  else if (str.Contains("n",0,i)) {
    boole = false;
    done = true;
  }
  else {
    boole = false;
    if (tog) done = true;
    else done = false;
    /*    done := false;      ??? Toggle:Blank  2.2.92 */
  }
  if (done == false) {
    GUIProg->ErrPos = 0;
    GUIProg->ErrorCode = WrongBoolean;
  }
} // END OF StringToBool


/***************************************************************************/
/* Convert from  Boolean to DString                                         */
/***************************************************************************/
void ConvCLASS::BoolToString (bool boole,
                                  DString& str,
                                  bool& done)
{
  /* "StrInit(str,maxlength);" should be replaced with */
  /* declaration "DString str(maxlength);" */;
  str.Reset(1);
  
  done = true;
  if (boole == true)
    str.c[0] = 'y';
    //else str.c[0] = 'j';
  else str.c[0] = 'n';
  str.c[1] = '\0';
  str.l = 1;
} // END OF BoolToString

/***************************************************************************/
/* Convert from  Cardinal to DString                                        */
/***************************************************************************/
void ConvCLASS::CardToString (unsigned num,
                              DString& str,
                              unsigned width)

{
  ConvToStr(num,10,false,str,width);
} // END OF CardToString

/***************************************************************************/
/* Convert from  Integer to DString                                         */
/***************************************************************************/
void ConvCLASS::IntToString (int num,
                                 DString& str,
                                 unsigned width)
{
  ConvToStr(ABS(num),10,num < 0,str,width);
} // END OF IntToString



/***************************************************************************/
/* Convert from  Integer/Cardinal/Number to DString                         */
/***************************************************************************/
void ConvCLASS::ConvToStr (unsigned num,
                           BASE base,
                           bool withPlusMinus,
                           DString& str,
                           unsigned width)

{
  DString digits(17);
  unsigned cnt, ix, maxlen, dig;

  for (ix = 1; ix <= digits.MaxLength()-1; ix++)
    digits[ix] = '0';
  cnt = 0;
  do {
    cnt++;
    dig = num % base;
    num = num/base;
    if (dig < 10) dig = dig+'0';
    else dig = dig-10+'A';
    digits[cnt] = char(dig);
  } while (num);
  if (withPlusMinus) {
    cnt++;
    digits[cnt] = '-';
  }
  if (width <= 0)
    width = cnt;
  maxlen = width;
  str.Reset(width);
  
  if (cnt > width) cnt = width;
  ix = 0;
  while (maxlen > cnt) {
    str.c[ix] = ' ';
    ix++;
    maxlen--;
  }
  while (cnt > 0) {
    str.c[ix] = digits[cnt];
    ix++;
    cnt--;
  }
  if (ix <= width) str.c[ix] = '\0';
  str.l = ix;
} // END OF ConvToStr


/***************************************************************************/
/* Convert from  DString to Identifier                                      */
/***************************************************************************/
void ConvCLASS::ConvToId (DString& str,
                          bool& done)

{
  short unsigned ix;
  char ch;

  ix = 0;
  if (str.c)
    while ((ix <= str.l)
           && (str.c[ix] == ' '))
      ix++;

  if (!str.c || (ix == str.l+1)) {
    GUIProg->ErrPos = ix;
    GUIProg->ErrorCode = NoIdentifier;
    done = false;
    return;
  }

  ch = str.c[0];
  if ((('a' <= ch) && (ch <= 'z')) || (('A' <= ch) && (ch <= 'Z'))) {
    done = true;
    return;
  }
  else {
    GUIProg->ErrPos = 0;
    GUIProg->ErrorCode = NoIdentifier;
  }
  done = false;
} // END OF ConvToId

/***************************************************************************/
/* Convert from  Integer/Cardinal/Number to DString                         */
/***************************************************************************/
bool ConvCLASS::ConvToNum (const DString& str,
                                  BASE base,
                                  bool withPlusMinus,
                                  unsigned& num)

{
  short unsigned ord0, ordF, dig, ix, r1, r2;
  char maxdig, ch, minlet;

  ord0 = '0';
  ordF = ord0+base;
  if (base <= 10) {
    maxdig = char(ordF);
    minlet = maxdig;
  }
  else {
    minlet = 'A';
    ordF = ordF+6;
    maxdig = char(ordF);
  }

  ix = 0;
  if (str.c)
    while ((ix <= str.l)
           && (str.c[ix] == ' '))
      ix++;

  if (!str.c || !str.l || (ix == str.l+1)) {
    GUIProg->ErrPos = ix;
    GUIProg->ErrorCode = NoNumbers;
    return false;
  }
  else if ((str.c[ix] == '-') || (str.c[ix] == '+')) {
    if ((str.c[ix] == '-')  && (!withPlusMinus)) {
      GUIProg->ErrPos = ix;
      GUIProg->ErrorCode = NotNegative;
      return false;
    }
    ix++;
    while ((ix <= str.l) && (str.c[ix] == ' '))
      ix++;
  }
  if (ix == str.l+1) {
    GUIProg->ErrPos = ix;
    GUIProg->ErrorCode = WrongNumber;
    return false;
  }
  else {
    num = 0;
    r1 = MaxCard/base;
    r2 = MaxCard % base;

    do {
      ch = CAP(str.c[ix]);   
            
      if (base != 0X10)
        if ((ch >= maxdig)
            || (ch < '0')
            || (('9' < ch)
                && (minlet > ch))) {
          GUIProg->ErrPos = ix;
          GUIProg->ErrorCode = WrongNumber;
          return false;
        }
      dig = ch-ord0;
      if (dig >= 10) dig -= 7;
      if (num > r1) {
        GUIProg->ErrPos = ix;
        GUIProg->ErrorCode = TruncationCard;
        return false;
      }
      else if (num == r1)
        if (dig > r2) {
          GUIProg->ErrPos = ix;
          GUIProg->ErrorCode = TruncationCard;
          return false;
        }
      num = num*base+dig;
      ix++;
    }
    while (!(((ix >= str.l) || (str.c[ix] == '\0'))));
    return true;
  }
} // END OF ConvToNum

/********************************************************************/
double ConvCLASS::exp10 (int e)
{
  double x;
  int i;

  x = one;
  if (e > 0)
    for (i = 1; i <= e; i++)
      x = x*ten;
  else
    for (i = 1; i <= -e; i++)
      x = x/ten;
  return x;
} // END OF exp10
/********************************************************************/


void ConvCLASS::convDigit (double& r,
                           DString& str,
                           unsigned& location)
{
  int y;

  y = TRUNC(r);
  str[location] = char(y+'0');
  location++;
  r = r-double(y);
} // END OF convDigit
/********************************************************************/

/********************************************************************/



/*--------------------------------------------------------------------------*/
/*------- RTBS -------------------------------------------------------------*/
/*---------------------------------------------------------------exported---*/
void ConvCLASS::RTBS (DString& str)
{
  int i;

  if (!str.c)
    return;
  else {
    if (!str.l)
      return;
    i = str.l-1;
    while (i >= 0)
      if ((str.c[i] == ' ')
          || (str.c[i] == '_'))
        if (i > 0)
          i = i-1;
        else {
          str.c[0] = '\0';
          str.l = 0;
        }
      else {
        if (i < str.l-1) {
          str.c[i+1] = '\0';
          str.l = i+1;
          str.m = i+2;
        }
        return;
      }
  }
} // END OF RTBS

/*--------------------------------------------------------------------------*/
/*------- RLBS -------------------------------------------------------------*/
/*---------------------------------------------------------------exported---*/
void ConvCLASS::RLBS (DString& str)
{
  short unsigned i;

  if (!str.c) return;
  else {
    if (!str.l) return;
    i = 0;
    while (i < str.l-1)
      if ((str.c[i] == ' ')
          || (str.c[i] == '_'))
        i = i+1;
      else {
        if (i > 0) str = str.Substr(i,0);
        return;
      }
  }
} // END OF RLBS

void ConvCLASS::Trim (DString& str)
{
  RLBS(str);
  RTBS(str);
} // END OF Trim


void ConvCLASS::INIT (CGUIProgBase *guiPr)
{
  GUIProg = guiPr;
  DString str;
  MaxCard = INT_MAX;
  MaxInt = INT_MAX;
  MinInt = -MaxInt;
  MaxIntAbs = MaxInt;
  ConvToStr(MaxCard,10,false,str,11);
  ConvToStr(MaxIntAbs,10,false,str,11); 

}


/*--------------------------------------------------------------------------*/
/*------------------ FieldLength
 *
 * provides length of a field and for cond-mode = 1 detectes field
 * length errors.
 *
 */
/*--------------------------------------------------------------------------*/

void ConvCLASS::FieldLength (CHEFormNode* trp,
                                 unsigned cond)
{
  if (!(trp->data.Atomic)) return;
  else
    if (trp->data.IterFlags.Contains(Ellipsis)) {
      trp->data.FIP.leng = 3;
      return;
    }

  if (!trp->data.StringValue.Defined())
    if ((trp->data.FormSyntax->Annotation.ptr->Length.FieldBoundRel == LE)
        || (trp->data.FormSyntax->Annotation.ptr->Length.FieldBoundRel == LG))
      if ((trp->data.FormSyntax->Annotation.ptr->Length.DataBoundRel != LE)
          && (trp->data.FormSyntax->Annotation.ptr->Length.Data > 0))
        trp->data.FIP.leng = trp->data.FormSyntax->Annotation.ptr->Length.Data;
      else trp->data.FIP.leng = 1;
    else trp->data.FIP.leng = trp->data.GetLengthField();
  else {
    trp->data.FIP.leng = trp->data.GetLengthField();
    switch (trp->data.FormSyntax->Annotation.ptr->Length.FieldBoundRel) {

    case LE:
      if (!cond)
        if (trp->data.StringValue.l < trp->data.FormSyntax->Annotation.ptr->Length.Data)
          trp->data.FIP.leng = trp->data.FormSyntax->Annotation.ptr->Length.Data;
        else trp->data.FIP.leng = trp->data.StringValue.l;
      else {
//        if (trp->data.StringValue.l >= trp->data.GetLengthField())
//          GUIProg->Warning = NoExpand;
        trp->data.FIP.leng = trp->data.GetLengthField();
      }
      break;

    case GE:
      if (!cond )
        if (trp->data.StringValue.l > trp->data.FIP.leng)
          trp->data.FIP.leng = trp->data.StringValue.l;
        else
          if (trp->data.StringValue.l < trp->data.FormSyntax->Annotation.ptr->Length.Data)
            trp->data.FIP.leng = trp->data.FormSyntax->Annotation.ptr->Length.Data;
          else trp->data.FIP.leng = trp->data.StringValue.l;
      else trp->data.FIP.leng = 2*trp->data.FIP.leng;
      break;

    case LG:
      if (!cond )
        trp->data.FIP.leng = trp->data.StringValue.l;
      else
        if (trp->data.StringValue.l < trp->data.FIP.leng)
          trp->data.FIP.leng = trp->data.GetLengthField();
        else trp->data.FIP.leng = 2*trp->data.FIP.leng;
      break;

    case EQ:
      if (cond == 1)
        if (trp->data.StringValue.l >= trp->data.GetLengthField())
          GUIProg->Warning = NoExpand;
        else trp->data.FIP.leng = trp->data.GetLengthField();
      break;

    default:
      ;
    }
  }
} // END OF FieldLength

/*--------------------------------------------------------------------------*/
/*------------------ DataLengthOK
 *
 * checks field length and provides data bound errors, the error position
 * within field (ErrPos global variable) and ErrorCode.
 *
 */
/*--------------------------------------------------------------------------*/
bool ConvCLASS::DataLengthOK (CHEFormNode* trp)
{
  bool ok;
  unsigned laenge;
  DString str;

  ok = true;
  laenge = 0;
  if (trp->data.StringValue.c)
    laenge = trp->data.StringValue.l;
  if (trp->data.FIP.up
      && trp->data.FIP.up->data.IterFlags.Contains(Ellipsis))
    return ok;
  if (ok) {
    if (trp->data.FormSyntax->Annotation.ptr->Length.Data > 0)
      switch (trp->data.FormSyntax->Annotation.ptr->Length.DataBoundRel) {

      case LE:
        if (laenge > trp->data.FormSyntax->Annotation.ptr->Length.Data) {
          ok = false;
          GUIProg->ErrPos = trp->data.FormSyntax->Annotation.ptr->Length.Data;
          GUIProg->ErrorCode = LongDataLength; /* Datenlaenege ueberschritten */
        }
        break;

      case EQ:
        if (laenge != trp->data.FormSyntax->Annotation.ptr->Length.Data) {
          ok = false;
          if (trp->data.FormSyntax->Annotation.ptr->Length.Data > laenge) {
            GUIProg->ErrPos = laenge;
            GUIProg->ErrorCode = ShortDataLength; /* Datenlaenge unterschritten */
          }
          else {
            GUIProg->ErrPos = trp->data.FormSyntax->Annotation.ptr->Length.Data;
            GUIProg->ErrorCode = LongDataLength;
          }
        }
        break;

      case GE:
        if (laenge < trp->data.FormSyntax->Annotation.ptr->Length.Data) {
          GUIProg->ErrPos = laenge;
          GUIProg->ErrorCode = ShortDataLength;
          ok = false;
        }
        break;

      case LG:
        if (laenge < trp->data.FormSyntax->Annotation.ptr->Length.Data) {
          GUIProg->ErrPos = laenge;
          GUIProg->ErrorCode = LongDataLength;
          ok = false;
        }
        break;
      }
    if (ok)
      StringLengthOK(trp,laenge,trp->data.GetLengthField(),
                     ok);
  }
  if (!ok) {
    if (trp->data.IoSigFlags.Contains(UnprotectedUser)) {
      if (trp->data.FIP.up
               && (trp->data.FIP.up->data.BasicFlags.Contains(ButtonMenu)
                   || trp->data.FIP.up->data.BasicFlags.Contains(OptionMenu)
                   || trp->data.FIP.up->data.BasicFlags.Contains(PopUpMenu)))
        GUIProg->ErrorCode = PressButton;
      else if (GUIProg->ErrPos < laenge)
        if ((trp->data.StringValue.c[GUIProg->ErrPos+1] == '\12' ) //EOL)
            || (laenge - GUIProg->ErrPos == 1)) {
          GUIProg->ErrPos = laenge;
        }
    }
    else {
      if (!GUIProg->Warning) {
      }
    }
  }

  return ok;
} // END OF DataLengthOK

/*--------------------------------------------------------------------------*/
/*------------------ StringLengthOK
 *
 * checks if BTpe = DString and FieldLength not greater then `Array bounds'
 *
 */
/*--------------------------------------------------------------------------*/
void ConvCLASS::StringLengthOK (CHEFormNode* trp,
                                     unsigned laenge,
                                     unsigned field,
                                     bool& ok)
{
  if ((!(SET(/*VLBytes,VLBitstring,*/VLString,-1).Contains(trp->data.BType)))
      && (laenge > field))
    if (trp->data.GetLengthField() > 0)
      switch (trp->data.FormSyntax->Annotation.ptr->Length.FieldBoundRel) {

      case LE:
        ;
        break;

      case EQ:
        GUIProg->ErrPos = field;
        GUIProg->ErrorCode = LongFieldLength;
        ok = false;
        break;

      default:
        ;
      }
} // END OF StringLengthOK



void ConvCLASS::ErrorMessage(QWidget* errWin, DString errText)
{
  QMessageBox::critical(wxTheApp->mainWidget(), wxTheApp->name(), errText.c,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
  //AfxMessageBox(errText.c, MB_ICONINFORMATION + MB_OK);
  if (errWin)
    errWin->setFocus();
}
