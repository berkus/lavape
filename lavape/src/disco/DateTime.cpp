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

/*
#ifdef __GNUC__
#pragma implementation
#endif
*/

#include "SYSTEM.h"

#include <time.h>
#include "DateTime.h"
#include "ASN1pp.h"
#include "CDPpp.h"
#include "Convert.h"


static long tval;
DateTimePtr dateTime ()

{
  tval = time(0);
  return (DateTimePtr)localtime(&tval);
}


DATE_::DATE_(const DString& str)
{
  date_s = str;
  Convert.StringToDate(str,year,month,day);
}


DATE_& DATE_::operator= (const DString& str)
{
  date_s = str;
  Convert.StringToDate(str,year,month,day);
  return *this;
}


DATE_& DATE_::operator= (const DATE_& date)
{
  date_s = date.date_s;
  year = date.year;
  month = date.month;
  day = date.day;
  return *this;
}


TIME_::TIME_()
{
  hour=0;
  minute=0;
  second=0;
  Convert.TimeToString(hour,minute,second,time_s);
}


TIME_::TIME_(const DString& str)
{
  time_s = str;
  Convert.StringToTime(str,hour,minute,second);
}


TIME_& TIME_::operator= (const DString& str)
{
  time_s = str;
  Convert.StringToTime(str,hour,minute,second);
  return *this;
}


TIME_& TIME_::operator= (const TIME_& time)
{
  time_s = time.time_s;
  hour = time.hour;
  minute = time.minute;
  second = time.second;
  return *this;
}


YEAR_::YEAR_(const DString& str)
{
  unsigned long y;
  
  year_s = str;
  Convert.StringToCard(str,y);
  year = y;
}


YEAR_& YEAR_::operator= (const DString& str)
{
  unsigned long y;
  
  year_s = str;
  Convert.StringToCard(str,y);
  year = y;
  return *this;
}


YEAR_& YEAR_::operator= (const YEAR_& year_struct)
{
  year_s = year_struct.year_s;
  year = year_struct.year;
  return *this;
}


void CDPDATE_ (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool baseCDP)

{
  DATE_ *vp = (DATE_*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTunsigned(pgf,cid,vp->year);
    CDPpp.CVTunsigned(pgf,cid,vp->month);
    CDPpp.CVTunsigned(pgf,cid,vp->day);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
  if (pgf == GET)
    Convert.DateToString(vp->year,vp->month,vp->day,vp->date_s);
} // END OF CDPDATE_

/*
void FIODATE_ (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool)

{
  DATE_ *vp = (DATE_*)varAddr;
  
  if (cid->Skip()) return;

  if (pgf == GET) {
    FIOpp.EOAstring(GET,cid,vp->date_s);
    Convert.StringToDate(vp->date_s,vp->year,vp->month,vp->day);
  }
  else // put direction
    FIOpp.EOAstring(pgf,cid,vp->date_s);
} // END OF FIODATE_
*/

void CDPYEAR_ (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool baseCDP)

{
  YEAR_ *vp = (YEAR_*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTunsigned(pgf,cid,vp->year);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
  if (pgf == GET)
    Convert.CardToString(vp->year,vp->year_s);
} // END OF CDPYEAR_

/*
void FIOYEAR_ (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool)

{
  YEAR_ *vp = (YEAR_*)varAddr;
  
  unsigned long y=0;
  
  if (cid->Skip()) return;

  if (pgf == GET) {
    FIOpp.EOAstring(GET,cid,vp->year_s);
    if (vp->year_s.l)
      Convert.StringToCard(vp->year_s,y);
    vp->year = y;
  }
  else // put direction
    FIOpp.EOAstring(pgf,cid,vp->year_s);
} // END OF FIOYEAR_
*/

void CDPTIME_ (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool baseCDP)

{
  TIME_ *vp = (TIME_*)varAddr;
  if (cid->Skip()) return;

  if (!baseCDP) CDPpp.CVTSEQUENCE(pgf,cid);
    CDPpp.CVTunsigned(pgf,cid,vp->hour);
    CDPpp.CVTunsigned(pgf,cid,vp->minute);
    CDPpp.CVTunsigned(pgf,cid,vp->second);
  if (!baseCDP) CDPpp.CVTEOC(pgf,cid);
  if (pgf == GET)
    Convert.TimeToString(vp->hour,vp->minute,vp->second,vp->time_s);
} // END OF CDPTIME_

/*
void FIOTIME_ (PutGetFlag pgf, ASN1* cid, address varAddr,
               bool)

{
  TIME_ *vp = (TIME_*)varAddr;
  
  if (cid->Skip()) return;

  if (pgf == GET) {
    FIOpp.EOAstring(GET,cid,vp->time_s);
    Convert.StringToTime(vp->time_s,vp->hour,vp->minute,vp->second);
  }
  else // put direction
    FIOpp.EOAstring(pgf,cid,vp->time_s);
} // END OF FIOTIME_
*/

