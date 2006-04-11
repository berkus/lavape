#ifndef __CDPpp
#define __CDPpp
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "ASN1pp.h"
#include "Halt.h"
#include "SETpp.h"
#include "CHAINANY.h"
#include "NESTANY.h"


class DISCO_DLL CDPppCLASS {

public:

  void INIT ();

  bool Return;


  /* **********************************************************************/

  /* ELEMENTARY CONVERSION PROCEDURES USED BY THE DISCO PRE-PROCESSOR */


  void CVTSEQUENCE (PutGetFlag pgf,
                    ASN1 *cid);

  void CVTEOC (PutGetFlag pgf,
               ASN1 *cid);

  void CVTNULL (PutGetFlag pgf,
                ASN1 *cid);

  void CVTunsigned (PutGetFlag pgf,
                    ASN1 *cid,
                    unsigned& x);

  void CVTint (PutGetFlag pgf,
         ASN1 *cid,
         int& x);

  void CVTfloat (PutGetFlag pgf,
         ASN1 *cid,
         float& x);

  void CVTdouble (PutGetFlag pgf,
         ASN1 *cid,
         double& x);

  void CVTbool (PutGetFlag pgf,
                ASN1 *cid,
                bool& x);

  void CVTbyte (PutGetFlag pgf,
                ASN1 *cid,
                byte& x);

  void CVTchar (PutGetFlag pgf,
                ASN1 *cid,
                char& x);
/*
  void CVTVLBYTES1 (PutGetFlag pgf,
                    ASN1 *cid,
                    unsigned& length,
                    address& a);
*/


  void TagFieldError (ASN1 *cid);


  void SKIP (PutGetFlag pgf,
             ASN1 *cid);
};

extern DISCO_DLL CDPppCLASS CDPpp;




/* **********************************************************************/

/* CODING/DECODING PROCEDURES USED BY THE DISCO PRE-PROCESSOR */


extern DISCO_DLL void CDPbool (PutGetFlag pgf,
              ASN1 *cid,
              address a,
              bool baseCDP=false);


extern DISCO_DLL void CDPbyte (PutGetFlag pgf,
              ASN1 *cid,
              address a,
              bool baseCDP=false);


extern DISCO_DLL void CDPchar (PutGetFlag pgf,
              ASN1 *cid,
              address a,
              bool baseCDP=false);


extern DISCO_DLL void CDPint (PutGetFlag pgf,
             ASN1 *cid,
             address a,
             bool baseCDP=false);


extern DISCO_DLL void CDPfloat (PutGetFlag pgf,
               ASN1 *cid,
               address a,
               bool baseCDP=false);


extern DISCO_DLL void CDPdouble (PutGetFlag pgf,
               ASN1 *cid,
               address a,
               bool baseCDP=false);


extern DISCO_DLL void CDPunsigned (PutGetFlag pgf,
                  ASN1 *cid,
                  address a,
                  bool baseCDP=false);


extern DISCO_DLL void CDPARBITRARY (PutGetFlag pgf,
                   ASN1 *cid,
                   address a,
                   bool baseCDP=false);


extern DISCO_DLL void CDPBYTES (PutGetFlag pgf,
               ASN1 *cid,
               address a,
               bool baseCDP=false);


extern DISCO_DLL void CDPSET (PutGetFlag pgf,
             ASN1 *cid,
             address a,
             bool baseCDP=false);


extern DISCO_DLL void CDPSTRING (PutGetFlag pgf,
                ASN1 *cid,
                address a,
                bool baseCDP=false);


extern DISCO_DLL void CDPBITSTRING (PutGetFlag pgf,
                   ASN1 *cid,
                   address a,
                   bool baseCDP=false);


/**********************************************************************/
/*              CHE-tyes for the basic builtin types:                 */
/**********************************************************************/


struct DISCO_DLL CHEunsigned : ChainAnyElem {
  unsigned data;

  ChainAnyElem* Clone ()
  { return new CHEunsigned(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEunsigned*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPunsigned(pgf,cid,(address)&data); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAunsigned(pgf,cid,data); }
};

struct DISCO_DLL NST0unsigned {
  unsigned *ptr;

  NST0unsigned () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPunsigned(pgf,cid,(address)ptr); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAunsigned(pgf,cid,*ptr); }
};

struct DISCO_DLL NSTunsigned : NST0unsigned {
  NSTunsigned () : NST0unsigned() {}

  virtual ~NSTunsigned () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEunsigned ();



struct DISCO_DLL CHEint : ChainAnyElem {
  int data;

  ChainAnyElem* Clone ()
  { return new CHEint(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEint*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPint(pgf,cid,(address)&data); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAint(pgf,cid,data); }
};

struct DISCO_DLL NST0int {
  int *ptr;

  NST0int () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPint(pgf,cid,(address)ptr); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAint(pgf,cid,*ptr); }
};

struct DISCO_DLL NSTint : NST0int {
  NSTint () : NST0int() {}

  virtual ~NSTint () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEint ();



struct DISCO_DLL CHEfloat : ChainAnyElem {
  float data;

  ChainAnyElem* Clone ()
  { return new CHEfloat(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEfloat*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPfloat(pgf,cid,(address)&data); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAfloat(pgf,cid,data); }
};

struct DISCO_DLL NST0float {
  float *ptr;

  NST0float () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPfloat(pgf,cid,(address)ptr); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAfloat(pgf,cid,*ptr); }
};

struct DISCO_DLL NSTfloat : NST0float {
  NSTfloat () : NST0float() {}

  virtual ~NSTfloat () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEfloat ();



struct DISCO_DLL CHEdouble : ChainAnyElem {
  double data;

  ChainAnyElem* Clone ()
  { return new CHEdouble(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEdouble*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPdouble(pgf,cid,(address)&data); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAdouble(pgf,cid,data); }
};

struct DISCO_DLL NST0double {
  double *ptr;

  NST0double () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPdouble(pgf,cid,(address)ptr); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAdouble(pgf,cid,*ptr); }
};

struct DISCO_DLL NSTdouble : NST0double {
  NSTdouble () : NST0double() {}

  virtual ~NSTdouble () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEdouble ();



struct DISCO_DLL CHEboolean : ChainAnyElem {
  bool data;

  ChainAnyElem* Clone ()
  { return new CHEboolean(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEboolean*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPbool(pgf,cid,(address)&data); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAboolean(pgf,cid,data); }
};

struct DISCO_DLL NST0boolean {
  bool *ptr;

  NST0boolean () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPbool(pgf,cid,(address)ptr); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAboolean(pgf,cid,*ptr); }
};

struct DISCO_DLL NSTboolean : NST0boolean {
  NSTboolean () : NST0boolean() {}

  virtual ~NSTboolean () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEboolean ();



struct DISCO_DLL CHEbyte : ChainAnyElem {
  byte data;

  ChainAnyElem* Clone ()
  { return new CHEbyte(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEbyte*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPbyte(pgf,cid,(address)&data); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAbyte(pgf,cid,data); }
};

struct DISCO_DLL NST0byte {
  byte *ptr;

  NST0byte () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPbyte(pgf,cid,(address)ptr); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAbyte(pgf,cid,*ptr); }
};

struct DISCO_DLL NSTbyte : NST0byte {
  NSTbyte () : NST0byte() {}

  virtual ~NSTbyte () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEbyte ();



struct DISCO_DLL CHEchar : ChainAnyElem {
  char data;

  ChainAnyElem* Clone ()
  { return new CHEchar(*this); }

  void  CopyData (ChainAnyElem* from)
  { data = ((CHEchar*)from)->data; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPchar(pgf,cid,(address)&data); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAchar(pgf,cid,data); }
};

struct DISCO_DLL NST0char {
  char *ptr;

  NST0char () { ptr = 0; }

  virtual void Destroy () { delete ptr; ptr = 0; }

  void CDP (PutGetFlag pgf, ASN1 *cid)
  { CDPchar(pgf,cid,(address)ptr); }

  //void FIO (PutGetFlag pgf, ASN1 *cid)
  //{ FIOpp.EOAchar(pgf,cid,*ptr); }
};

struct DISCO_DLL NSTchar : NST0char {
  NSTchar () : NST0char() {}

  virtual ~NSTchar () { Destroy(); }
};

extern DISCO_DLL ChainAnyElem* NewCHEchar ();


#endif
