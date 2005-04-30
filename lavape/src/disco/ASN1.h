#ifndef __ASN1
#define __ASN1
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "DString.h"
#include <qstring.h>


enum TClass {
  Universal,
  Application,
  Context,
  Private};

enum TForm {
  Primitive,
  Constructor};

enum TLengthForm {
  ShortLength,
  LongLength,
  IndefiniteLength,
  DefiniteLength};
/* In input direction "Short", "Long", or "Indefinite" is returned,
   while in output direction you may specify only "Definite" or
   "Indefinite". In case "Definite" the "PutHeader" routine decides
   whether "Short" or "Long" applies */

struct Header {
  /* On input only such ids are supported, which fit into a CARDINAL
     variable.
     On output only such ids and lengths are supported, which
     fit into a CARDINAL variable. */
  TClass Class;
  TForm Form;
  unsigned Id;
  /* this module supports only id's which fit into a CARDINAL
     variable */
  TLengthForm LengthForm;
  unsigned Len;
  /* in "put" direction Len need not be specified if LengthForm is
     Indefinite */

  /* the following fields are automatically filled in in "get"
     direction and need not be specified in "put" direction */
  unsigned HeaderLen;
  /* total length of the id plus length field of the element */
  unsigned ContentsBytesRead;
  /* number of bytes of the current element contents
     that have been read so far */
};

typedef Header *HdrPtr;


/* **********************************************************************/


enum destinationType {
  none,
  buffer,
  putProcedure};

enum ErrCode {
  GetHdrOoc,
  IdTooLong,
  IndefLength,
  WrongLength,
  SLtooLarge,
  NoSoc,
  StackEmpty,
  LenTooLong,
  DecimalTooLong,
  ArrayTruncation,
  WrongElRep,
  WrongOctetString,
  NegCard,
  nErrorCode};

typedef struct stackElem *stackPtr;

struct stackElem {
  Header header;
  stackPtr father;
  /* points to the preceding stack element */
  stackPtr successor;
  /* points to the next stack element */
  /* this header does not yet count as "got" if true; cf. "Preview" */
  bool explicitAltTag; /* has alternative ASN.1 tag */
};


/**********************************************************************/
/*                                                                    */
/*                        DEFINITION OF CLASS                         */
/*                                                                    */
/**********************************************************************/

// This is an abstract base class for user-defined derived classes
// which define the pure virtual functions putChar/getChar for
// input/output of single ASN.1-bytes, and errorExitProc for the 
// user-controlled treatment of errors


class DISCO_DLL ASN1 {

  virtual void putChar (const unsigned char&);
  virtual void getChar (unsigned char&);
  virtual void errorExitProc ();

  friend class ASN1ppCLASS;
  friend class CDPppCLASS;
  //friend class FIOppCLASS;
  
  
  stackPtr topOfStack;
  /* stack pointer: points to the top element of the ASN.1 object
     analysis stack, see above */
  stackPtr bottomOfStack;
  /* points to the first element of the ASN.1 object
     analysis stack (cf. "Close" */
  unsigned char headerBuffer[20];
  /* the header must be kept for the "previewed"/GetElement case */
  unsigned octetStringNestingLevel, altID, nestedCASEs;
  int wordSize;
  DString contentsBuffer;  
  TClass altClass;
protected:
  bool skip, wrongElemStop, wrongElem, previewed,
          altTag, implicit;
/* **********************************************************************/

public:

  bool HeaderTrace, Silent, isBigEndian;
  /* every ASN.1 header is printed (cf. "PrintHeader" below) if and as
     long as HeaderTrace is true;
     error messages are printed if and as long as Silent is false;
     both variables default to false */

  int Release;

  ASN1 ();

  virtual ~ASN1 ();


  void SetSkip (const bool onOff);


  inline bool Skip () { return skip; }


  void WrongElemStop (const bool onOff);

  /* IF "WrongElemStop(cid,false)" has been called then error messages are
     suppressed and a normal RETURN is performed if an ASN.1 element of
     unexpected type (Id, Class, Form, Length) is encountered by any of the
     "GetXXX" procedures defined below, where XXX
     stands for any specific element type (whose name consists of capital
     letters only).

     The error condition is remembered by ASN1 and must be removed 
     thereafter by calling the appropriate "GetXXX" procedure.

     If there is more than one possible alternative then you can call
     "Preview" and then examine the header in question to determine the
     appropriate "Get..." procedure .

     Example: read a sequence of CARDINALS of unknown length:

     cid.GetSEQUENCE();
     cid.WrongElemStop(false);
     LOOP
       cid->GetCARDINAL(x);
       IF cid.WrongElem() THEN EXIT END;
       ...
     END;
     cid.WrongElemStop(true);
     cid.GetEOC();
     ...
  */

  bool WrongElem ();

  /* Returns true if the above-mentioned "wrong element" condition has
     occurred and not been reset so far on this connection, else false */

  bool TagFieldValid ();

  void INCnestedCASEs ();

  void DECnestedCASEs ();

  /* used to decide in CDP's and FIO's whether CASE-variants
     should be NILed */


  void CallErrorExit ();

  /* Used in FIOpp to invoke the ASN1 error exit procedure
     in case of FIO errors */


  void GetHeader (HdrPtr& hdrPtr);
  /* reads the header (= ID + length field) of the next ASN.1 element;
     even if its contents is of length 0 the latter must be at least
     skipped (cf. "SkipContents" below) in order to complete the
     processing of the element */


  void Preview (HdrPtr& hdrPtr);
  /* like "GetHeader" but the header does not yet count as "got"; it has
     to be got by a subsequent "Get..." call which expects a header to
     follow */


  void CheckType (HdrPtr& hdrPtr,
      TClass asn1Class,
                  TForm form,
                  unsigned id);
  /* reads the next ASN.1 header and checks whether it has the specified
     asn1Class, form and id */


  void GetElement (DString& s,
                   bool& eoc);
  /* copies the subsequent ASN.1 element from source to array s, beginning
     at position s[sLength]; sLength is updated;
     eoc := true if the encountered element is an "end of contents"
     element, else false */


  void GetEOElem ();
  /* reads all concluding EOCs up to the highest level; reports an error
     if a non-EOC element is encountered */


  void SkipElement (bool& eoc);
  /* skips the subsequent ASN.1 element;
     eoc := true if the encountered element is an "end of contents"
     element, else false */


  void CopyElement (ASN1 *destination,
                    bool& eoc);
  /* copies the subsequent ASN.1 element from source to destination;
     eoc := true if the encountered element is an "end of contents"
     element, else false */


  void GetContents (DString& s,
                    bool& empty);
  /* copies the remainder of the contents of the current ASN.1 element
     from source to ARRAY OF CHAR s, beginning at position s[sLength];
     sLength is updated; empty := true if the element is constructor but
     consists only of an end-of-contents sub-element; else false.  If the
     current element is an "end of contents" element then "GetContents"
     completes processing of this and the father (constructor) element */


  void SkipContents (bool& empty);
  /* skips the remainder of the contents of the current ASN.1 element;
     empty := true if the element is constructor but consists only of an
     end-of-contents sub-element; else false.  If the current element is
     an "end of contents" element then "SkipContents" completes processing
     of this and the father (constructor) element */


  void SkipObject ();
  /* skips the remainder of the current ASN.1 object;
     has no effect if called "between" two objects */


  void CopyContents (ASN1 *destination, bool& empty);
  /* copies the contents portion of the current ASN.1 element from source
     to destination;
     empty := true if the element is constructor but consists
     only of an end-of-contents sub-element; else false */


  void PutHeader (Header& header);
  /* puts an ASN.1 element header to the ASN.1 output buffer, whose
     components may be specified in record "header". VAR is used in order
     to avoid copying the record when calling "PutHeader" */


  void PutBytes (const DString& s);
  /* copies s.l bytes to *this; the programmer is himself responsible
     for the correct structure of the resulting ASN.1 stream */


  void PrintHeader ();

  /* prints Id, Form, Class, and Len of the last ASN.1 element that has
     been read but not yet completely processed so far.

     (Cf. also "VAR HeaderTrace" above) */


  /* **********************************************************************/


  void GetSEQUENCE ();
  /* reads the next ASN.1 header and checks whether it starts a
     ASN.1 "SEQUENCE" element */


  void GetSETTYPE ();
  /* reads the next ASN.1 header and checks whether it starts a
     ASN.1 "SET" element */


  void GetEOC ();
  /* reads the next ASN.1 header and checks whether it represents an
     "end of contents" element;
     if the element is EOC then it is considered as completely processed
     thereafter */


  bool EOC ();
  /* checks whether the last ASN.1 header that has been encountered so far
     belonged to an "end of contents" element */


  void GetNULL ();
  /* reads the next ASN.1 element and checks whether it represents a
     NULL element;
     if the element is NULL then it is considered as completely processed
     thereafter */


  void PutSEQUENCE ();
  /* puts the header of a "sequence" element to the ASN.1 buffer
     (Class = Universal, Form = Constructor, Id = 16, Length = indefinite)
  */


  void PutSETTYPE ();
  /* puts the header of a "sequence" element to the ASN.1 buffer
     (Class = Universal, Form = Constructor, Id = 17, Length = indefinite)
  */


  void PutEOC ();
  /* puts an "end of contents" element */


  void PutNULL ();
  /* puts a NULL element (Class = Universal, Form = Primitive, Id = 5,
     Length = 0) */

  void AltTag (TClass asn1Class,
               unsigned id,
               bool impl);

  void PutOptionalAbsent ();

  bool GotOptional ();

  /* **********************************************************************/

  /* HIGHER LEVEL PROCEDURES
     (CONVERSION OF ELEMENTARY DATA TYPES): */


  void GETunsigned (unsigned& x);

  void GETunsigned (unsigned long& x);

  unsigned FGETunsigned ();

  void GETint (int& x);

  void GETint (long int& x);

  int FGETint ();

  void GETfloat (float& x);
  
  void GETdouble (double& x);

  void GETboolean (bool& b);

  void GETset (SET& x);

  void GETbyte (unsigned char& c);

  void GETbytes (DString& s);
  /* in contrast to GetSTRING: doesn't perform any code translation and
     doesn't append a concluding 0C. It expects an octet string whose length
     doesn't exceed that of the receiving array. Returns the actual length of
     the string in VAR l as a side effect */

  void GETbits (struct Bitstring& s);
  /* gets a variable length string (cf. module DString) into VAR s,
     but s^.l = #bits! */

  void GETchar (char& c);
  /* CHAR's are considered to represent readable text coded in the
     specific code of the respective local computer system (mostly ASCII or
     EBCDIC). The ASN.1 representation uses ASCII. */

  void GETstring (DString& s);

  void PUTunsigned (const long unsigned x);

  void PUTint (const long int x);

  void PUTfloat (const float x);

  void PUTdouble (const double x);

  void PUTboolean (const bool b);

  void PUTset (const SET& x);

  void PUTbyte (const unsigned char c);

  void PUTbytes (const DString& s);
  /* in contrast to PutSTRING: doesn't perform any code translation */

  void PUTbits (const Bitstring& s);

  void PUTchar (const char c);
  /* CHAR's are considered to represent readable text coded in the
     specific code of the respective local computer system (mostly ASCII or
     EBCDIC). The ASN.1 representation uses ASCII. */

  void PUTstring (const DString& s); // translates to ASCII code 
  

/**********************************************************************/

protected:

  void error (ErrCode errCode,
              char *callingProcedure);


  void getHeader (destinationType dT,
                  ASN1 *destination,
      DString& s,
                  HdrPtr& hdrPtr);


  void putHeader (Header& header);


  void popUp (unsigned contentsLength);


  bool tagOK (TClass asn1Class, unsigned id);


  int convInteger (const DString& s);


  long int convLongInteger (const DString& s);


  void getString (TClass asn1Class,
                  unsigned id,
      DString& s,
                  bool& eoc,
                  unsigned& unusedBits);

  QString toHex (QString sIn);
};

#endif
