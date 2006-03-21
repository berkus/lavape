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


#include "Convert.h"
#pragma hdrstop


ConvertCLASS Convert;
static bool __INITstarted=false;


enum State {
    Start,
    Before,
    Dot1,
    After,
    Dot2,
    Fin,
    Stop};

enum STate {
    STart,
    BEfore,
    DOt1,
    AFter,
    STop};


void ConvertCLASS::CardToString (long unsigned x,
                                 DString& str)

{
  long unsigned butlast, last;

  butlast = x/10;
  last = x % 10;
  if (butlast)
    CardToString(butlast,str);
  else
    str.l = 0;
  if (str.l == str.m) str.Expand(10);
  str[str.l++] = char('0'+last);
  str[str.l] = '\0';
}


void ConvertCLASS::IntToString (long int x,
                                DString& str)

{
  long unsigned butlast, last, xabs;
  int sign;

  if (x < 0) {
    sign = -1;
    xabs = -x;
  }
  else {
    sign = 1;
    xabs = x;
  }
  butlast = xabs/10;
  last = xabs % 10;
  if (butlast != 0)
    IntToString(sign*int(butlast),str);
  else if (x < 0) {
    if (str.l == str.m) str.Expand(10);
    str[0] = '-';
    str.l = 1;
  }
  else
    str.l = 0;
  if (str.l == str.m) str.Expand(10);
  str[str.l++] = char('0'+last);
  str[str.l] = '\0';
}


void ConvertCLASS::StringToCard (const DString& str,
                                 long unsigned& x)

{
  unsigned i;

  x = 0;
  
  for (i = 0; i < str.l; i++) {
    if (str.c[i] != ' ') break;
  }
  
  for (; (str.c[i] >= '0') && (str.c[i] <= '9'); i++)
    x = x*10+(str.c[i]-'0');
}

  
void ConvertCLASS::StringToDate (const DString& str,
                                 unsigned& year,
                                 unsigned& month,
                                 unsigned& day)
{
  State state;
  char ch;
  unsigned ic, index;
  
  day   = 0;
  month = 0;
  year  = 0;
  if (str.l == 0) return;
  
  index = 0;
  
  state = Start;
  
  for (;;) {
    if (index <= str.MaxLength()-1) {
      ch = str[index];
      index++;
    }
    else ch = '\0';
    if (('0' <= ch) && (ch <= '9')) {
      ic = ch-'0';
      switch (state) {

      case Start:
        state = Before;
        day = ic;
        break;

      case Before:
        if ((day != 0))
             day = 10*day+ic;
        else day = ic;
        break;

      case Dot1:
        state = After;
        month = ic;
        break;

      case After:
        if ((month != 0))
             month = 10*month+ic;
        else month = ic;
        break;

      case Dot2:
        state = Fin;
        year = ic;
        break;

      case Fin:
        if ((year != 0))
             year = 10*year+ic;
        else year = ic;
        year = year+1900;
        state = Stop;
        goto exit;
      case Stop:
        goto exit;

      default:
        goto exit;
      }
    }   
    else if ((ch == '.')
          || (ch == '/')
          || (ch == '|')
          || (ch == ':')
          || (ch == ','))
          if (state <= Before)
            state = Dot1;
          else if (state <= After)
            state = Dot2;
          else {
            goto exit;
          }
    else if (ch == ' ')
      if (((state == Start) && (ch == ' ')) 
       || ((state == Dot1)  && (ch == ' '))
       || ((state == Dot2)  && (ch == ' ')))
        /* skip leading blanks: go on */
        ;
       else goto exit;
    else if (ch == '\0')  goto exit;
     
    else goto exit;
  } exit: ; /* LOOP */
  
    switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
      ;
      break;
    case 2:
       break;

    case 4:
    case 6:
    case 9:
    case 11:
      break;

    default:
      ;
    }
} // END OF StringToDate


void ConvertCLASS::StringToTime (const DString& str,
                                 unsigned& hour,
                                 unsigned& minute,
                                 unsigned& second)
{
  STate state;
  char ch;
  unsigned index, ic;

  hour = 0;
  minute = 0;
  
  if (str.l == 0) {
    second = 60;
    return;
  }
  else
    second = 0;

  index = 0;
  state = STart;
  for (;;) {
    if (index <= str.MaxLength()-1) {
      ch = str[index];
      index++;
    }
    else ch = '\0';
    if (('0' <= ch)  && (ch <= '9')) {
      ic = ch-'0';
      switch (state) {

      case STart:
        state = BEfore;
        hour = ic;
        break;

      case BEfore:
        if (hour)
             hour = 10*hour+ic;
        else hour = ic;
        break;

      case DOt1:
        state = AFter;
        minute = ic;
        break;

      case AFter:
        if (minute)
             minute = 10*minute+ic;
        else minute = ic;
        break;
      case STop :
        break;

      default:
        goto exit;
      }
    }
    else if ((ch == '.')
             || (ch == '/')
             || (ch == '|')
             || (ch == ':')
             || (ch == ','))
           if (state <= BEfore)
             state = DOt1;
           else 
             goto exit;
           
    else if (ch == ' ')
           if (((state == STart) && (ch == ' '))
           || ((state == DOt1)  && (ch == ' ')))
           /* skip leading blanks: go on */
           ;
          else
            goto exit;
    else goto exit;
  } exit: ; /* LOOP */
  
  if (state != STop && state < AFter) minute = 0;
} // END OF StringToTime


void ConvertCLASS::DateToString (unsigned year,
                                 unsigned month,
                                 unsigned day,
                                 DString& str)
{
  DString str1(2);
  
  if (year > 99) year = year % 100;
  str.Reset(8);
  if (year==0 && month==0 && day==0) return;
  
  CardToString(day,str1);
  if (day < 10) str1 = "0" + str1;
  str = str1 + ".";

  CardToString(month,str1);
  if (month < 10) str1 = "0" + str1;
  str += str1 + ".";
  
  CardToString(year,str1);
  str += str1;
} // END OF DateToString




void ConvertCLASS::TimeToString(unsigned hour,
                                unsigned minute,
                                unsigned second,
                                DString& str)
{
  DString str1(2);
  
  str.Reset(8);
  if (second==60) return;
  
  CardToString(hour,str1);
  if (hour < 10) str1 = "0" + str1;
  str = str1 + ":";

  CardToString(minute,str1);
  if (minute < 10) str1 = "0" + str1;
  str += str1;
  
  if (second) {
    CardToString(second,str1);
    str += ":" + str1;
  }
} // END OF TimeToString


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void ConvertCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

}
