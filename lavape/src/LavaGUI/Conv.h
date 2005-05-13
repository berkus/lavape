#ifndef __Conv
#define __Conv


#include "SYSTEM.h"
#include "STR.h"
#include "GUIProgBase.h"


/* Conversion between numbers and strings

  Conventions for the routines that convert a string to
  a number:
   - Leading blanks are skipped.
   - A plus sign ('+') preceeding the number is always
     accepted, a minus sign ('-') is only accepted when
     converting to INTEGER.
   - Blanks between the plus or minus sign and the number
     are skipped.
   - The last character in the string must belong to the
     number to be converted. No trailing blanks or other
     trailing charatcers are allowed.
   - 'done/okay' returns true if the conversion is successful.

  Conventions for the routines that convert a number to
  a string:
   - If the string is too small, the number is truncated.
   - If less than 'width' digits are needed to represent
     the number, leading blanks are added.
*/

const int MaxBase = 16;
typedef int BASE;

static const float zero = (float)0.0;
static const float one  = (float)1.0;
static const float ten  = (float)10.0;
static const int maxexp = 308;
static const int minexp = -306;


class ConvCLASS {
public:
  void INIT (CGUIProgBase *guiPr);
  CGUIProgBase *GUIProg;
  int MinInt, MaxInt;
  unsigned MaxIntAbs, MaxCard;



  void StringToInt (const DString& str, int& num, bool& done);
  /* - Convert a string to an INTEGER number.
  in:    str    string to convert
  out:   num    converted number
         done    true if successful conversion,
                 false if  number out of range,
                 or contents of string non numeric. */


  void StringToBool (const DString& str, bool& boole, bool& done);
  /* - Convert a string to a bool Value.
  in:   str    string to convert
  out:  boole  converted bool
        done   true if successful conversion,
               false  or number out of range,
               or contents of string not within base. */

  void BoolToString (bool boole, DString& str, bool& done);
  /* - Convert a bool value to a string.
  in:   boole  converted bool
  out:  str    "true"/"false" */

  void CardToString (unsigned num, DString& str, unsigned width);
  /* - Convert a CARDINAL number to a string.
  in:   num    number to convert
        width  width of the returned string
  out:  str    returned string representation of the number */

  void IntToString (int num, DString& str, unsigned width);
  /* - Convert an INTEGER number to a string.
  in:   num    number to convert
        width  width of the returned string
  out:  str    returned string representation of the number */


  void ConvToId (DString& str, bool& done);

/* from Stamps and RealConv (DRdir ************************************** */

//  void StringToReal (DString& str, int digits, unsigned width, bool& okay);

//  void RealToString (double r, int Digits, int width, DString& str, bool& okay);
  void Trim (DString& str); //  removes leading and trailing blanks for a string 
  void RTBS (DString& str); //  removes trailing blanks for a string
  void RLBS (DString& str); //  removes leading blanks for a string

private:


  // Convert from  Integer/Cardinal/Number to DString                         
  void ConvToStr (unsigned num, BASE base, bool withPlusMinus, DString& str, unsigned width);

  // Convert from  Integer/Cardinal/Number to DString                         
  bool ConvToNum (const DString& str, BASE base, bool withPlusMinus, unsigned& num);

  enum ConvState {
    start,
    numsign,
    before,
    dot,
    after,
    exp,
    expsign,
    expnum,
    stop};

  /********************************************************************/
  double exp10 (int e);
  /********************************************************************/


  void convDigit (double& r, DString& str, unsigned& location);
/*
  void convertnumber (unsigned num,
                      unsigned len,
                      DString& str,
                      unsigned& location);                 
*/ 
public:
  
  /**/ void ErrorMessage(QWidget* errWin, DString errText);
  void FieldLength (CHEFormNode* trp, unsigned cond);
  bool DataLengthOK (CHEFormNode* trp);
  void StringLengthOK (CHEFormNode* trp, unsigned laenge, unsigned field, bool& ok);

};

extern  ConvCLASS Conv;

#endif
