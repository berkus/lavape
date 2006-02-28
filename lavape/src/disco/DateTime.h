#ifndef __DateTime
#define __DateTime
/*
#ifdef __GNUC__
#pragma interface
#endif
*/

#include "SYSTEM.h"
#include "ASN1pp.h"


struct DISCO_DLL DATE_ {
  unsigned year, month, day;
  
  DString date_s;

  DATE_()
  {
    year=0;
    month=0;
    day=0;
  }
  
  DATE_(const DString& str);
  
  DATE_& operator= (const DString& str);
  
  DATE_& operator= (const DATE_& date);

  friend void CDPDATE_ (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP=false);
/*
  friend void FIODATE_ (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseFIO=false);*/
};


struct DISCO_DLL YEAR_ {
  unsigned year;
  
  DString year_s;

  YEAR_()
  {
    year=0;
  }
  
  YEAR_(const DString& str);
  
  YEAR_& operator= (const DString& str);
  
  YEAR_& operator= (const YEAR_& year);

  friend void CDPYEAR_ (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP=false);
/*
  friend void FIOYEAR_ (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseFIO=false);*/
};

struct DISCO_DLL TIME_ {
  unsigned hour, minute, second;
  
  DString time_s;

  TIME_();
  
  TIME_(const DString& str);
  
  TIME_& operator= (const DString& str);
  
  TIME_& operator= (const TIME_& time);

  friend void CDPTIME_ (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseCDP=false);
/*
  friend void FIOTIME_ (PutGetFlag pgf, ASN1* cid, address varAddr,
                        bool baseFIO=false);*/
};


struct DISCO_DLL TDateTime {
  unsigned Sec, Min, Hour, MDay, Mon, Year, WDay, YDay, IsDST;
};

typedef TDateTime *DateTimePtr;


//extern DISCO_DLL DateTimePtr dateTime ();


#endif
