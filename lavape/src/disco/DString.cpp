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


#ifdef __GNUC__
#pragma implementation
#endif

/**********************************************************************

          implementation of classes DString/Bitstring

 **********************************************************************/


#include "DString.h"
#include "MachDep.h"
#include <ctype.h>
#include "MACROS.h"
#include <string.h>


/**********************************************************************/
/*             auxiliary functions                                    */
/**********************************************************************/

static unsigned length (const char *str)
{
  for (unsigned len = 0; ; len++)
    if (str[len] == '\0') return len;
}


static void copy (const char * const from,
      char *to,
      const unsigned len)
{
  unsigned i;
  
  for (i = 0; i < len; i++) 
    to[i] = from[i];
  to[len] = '\0';
}


/**********************************************************************/


void DString::init (const DString& str)
{
  l = str.l;
  m = str.m;
  if (str.c == 0) c = 0;
  else {
    c = new char [m+1];
    for (unsigned i = 0; i < l; i++)
      c[i] = str.c[i];
    c[l] = '\0';
  }
}


void DString::copy (const DString& from)
{
  if (from.l > m) Reset(from.l);
  
  for (unsigned i = 0; i < from.l; i++)
    c[i] = from.c[i];
  l = from.l;
  if (c) c[l] = '\0';
}

DString& DString::operator= (const DString& str)
{ copy(str); return *this; }


void Bitstring::init (const Bitstring& from)
{
  unsigned nBytes=((unsigned)from.l+7)/8;
  
  l = from.l;
  m = from.m;
  if (from.c == 0) c = 0;
  else {
    c = new char [m];
    for (unsigned i = 0; i < nBytes; i++)
      c[i] = from.c[i];
  }
}


void Bitstring::copy (const Bitstring& from)
{
  unsigned nBytes=((unsigned)from.l+7)/8;
  
  if (nBytes > m) Reset(nBytes);
  
  for (unsigned i = 0; i < nBytes; i++)
    c[i] = from.c[i];
  l = from.l;
}


/**********************************************************************/

  
DString::DString (const unsigned maxLen)
{ 
  l = 0; 
  m = maxLen; 
  if (m > 0) {
    c = new char [m+1];
    c[0] = '\0';
  }
  else
    c = 0;
}

DString::DString (const char * const s)
// '\0' terminates s
{
  if (s) {
    l = m = length(s);
    c = new char[m+1];
    ::copy(s,c,l);
  }
  else {
    l = m = 0;
    c = 0;
  }
}

DString::DString (const unsigned char * const s)
// '\0' terminates s
{
  if (s) {
    l = m = length((char*)s);
    c = new char[m+1];
    ::copy((char*)s,c,l);
  }
  else {
    l = m = 0;
    c = 0;
  }
}

DString::DString (const char * const s, const unsigned len)
// explicit length specification for s
{
  l = m = len;
  c = new char[m+1];
  ::copy(s,c,l);
}


DString::DString (const char ch, const unsigned count)
{
  l = m = count;
  c = new char[m+1];
  for (unsigned i = 0; i < count; i++)
    c[i] = ch;
  l = count;
  c[l] = '\0';
}



/**********************************************************************/



void DString::Append (const DString& str)
{
  unsigned i, l1=str.l;
  
  if (l1 == 0) return;
  
  if (l1+l > m) Expand(l1+l-m);
    
  for (i = 0; i < l1; i++)
    c[l+i] = str.c[i];
  l += l1;
  c[l] = '\0';
}


void DString::CAPS ()
{
  char offset = 'a'-'A';
  
  for (unsigned i = 0; i < l; i++)
    if (MachDep.IsLetter(c[i]) && (c[i] >= 'a'))
      c[i] -= offset;
}


int Compare (const DString& str1,
             const DString& str2)
{
  unsigned i, l1=str1.l, l2=str2.l, minLen=(l1 < l2 ? l1 : l2);

  if (l1 == 0)
    if (l2 == 0)
      return 0;
    else
      return 1;
  else if (l2 == 0)
    return -1;

  for (i=0; i < minLen; i++)
    if (str1.c[i] != str2.c[i]) break;
  
  if (i == minLen) 
    if (l1 < l2)
      return -1;
    else if (l1 == l2)
      return 0;
    else
      return 1;
  else
    if (str1.c[i] < str2.c[i])
      return -1;
    else
      return 1;
}


bool Equal (const DString& str1,
            const DString& str2)
{
  int i, l1=str1.l, l2=str2.l;

  if (l1 != l2)
     return false;
  else if (l1 == 0)
    return true;

  for (i=l1-1; i >= 0; i--)
    if (str1.c[i] != str2.c[i])
      return false;
  
  return true;
}


DString& DString::operator+= (const DString& app)
{
  unsigned l2 = app.l, i;

  if (l+l2 > m) Expand(l+l2-m);
  
  for (i = l; i < l+l2; i++)
    c[i] = app.c[i-l];
  l = l+l2;
  if (c) c[l] = '\0';
  return *this;
}


DString operator+ (const DString& str1, const DString& str2)
{
  DString result(str1.l+str2.l);
  // rather than DString to ensure automatic destruction
  
  result = str1;
  return result += str2;
}


bool DString::Contains (const DString& substr,
                          const unsigned start,
                          unsigned& location) const
{
  unsigned ls=substr.l, i;

  if (start >= l) return false;
  
  if (substr.l == 0) {
    location = start;
    return true;
  }
  
  if (start+ls > l)
    return false;

  location = start;
  for (;;) {
    while ((location+ls <= l)
           && (c[location] != substr.c[0]))
      location++;
    if (location+ls > l) return false;
    
    i = 1;
    while ((i < ls)
           && (c[location+i] == substr.c[i]))
      i++;
    if (i >= ls) return true;
    location++;
  }
}

bool DString::ContainsNoCase (const DString& substr,
                          const unsigned start,
                          unsigned& location) const
{
  unsigned ls=substr.l, i;

  if (start >= l) return false;
  
  if (substr.l == 0) {
    location = start;
    return true;
  }
  
  if (start+ls > l)
    return false;

  location = start;
  for (;;) {
    while ((location+ls <= l)
           && (tolower(c[location]) != tolower(substr.c[0])))
      location++;
    if (location+ls > l) return false;
    
    i = 1;
    while ((i < ls)
           && (tolower(c[location+i]) == tolower(substr.c[i])))
      i++;
    if (i >= ls) return true;
    location++;
  }
}


void DString::Delete (const unsigned start,
                     const unsigned count)
{
  unsigned count1=count;
  
  if ((l == 0) || (count == 0) || (start >= l)) return;
    
  if (start+count > l) count1 = l-start;
    
  for (unsigned i = start; i < l-count1; i++)
    c[i] = c[i+count1];
    
  l -= count1;
  c[l] = '\0';
}


void DString::Insert (const DString& substr,
         const unsigned start)
{
  unsigned ls=substr.l;
  int i;

  if (ls == 0) return;
  
  if (start > l) return;

  if (l+ls > m) Expand(l+ls-m);
  
  for (i = l-1; i >= (int)start; i--)
    c[i+ls] = c[i];
  l += ls;
  c[l] = '\0';
  
  /* now insert the substring */ ;
  for (i = start; i < (int)(start+ls); i++)
    c[i] = substr.c[i-start];
}


void DString::Repeat (const char ch, const unsigned count)
{
  if (count) {
    if (count > m) Expand(count-m);
  
    for (unsigned i = 0; i < count; i++) c[i] = ch;
  
    l = count;
    c[l] = '\0';
  }
  else
    Reset(0);
}


void DString::Replace (const DString& substr,
                      const unsigned start)
{
  unsigned ls=substr.l;

  if (substr.c == 0) return;
  
  if (start > l) return;
  
  if (start+ls > m) Expand(start+ls-m);
  
  /* now insert the substring */ ;
  for (unsigned i = start; i < start+ls; i++)
    c[i] = substr.c[i-start];
  
  l = (start+ls > l ? start+ls : l);
  c[l] = '\0';
}


DString DString::Substr (const unsigned start,
                         const unsigned count) const
{  
  unsigned count1=count;
  
  if ((l == 0) || (start >= l)) {
    return DString();
  }
  
  if ((count == 0) || (start+count > l))
    count1 = l-start;

  DString result(count1);
    
  for (unsigned i = 0; i < count1; i++)
    result.c[i] = c[start+i];
    
  result.l = count1;
  result.c[count1] = '\0';
  return result;
}


bool DString::StringMatch(const DString& str1, bool matchCase, bool wholeWord)
{
  unsigned loc;
  if (wholeWord && matchCase)
    return str1 == *this;
  else if (matchCase)
      return Contains (str1, 0, loc);
  else if (wholeWord)
    return !strcasecmp(str1.c,c);
  else
    return ContainsNoCase (str1, 0, loc);
}

void DString::Array (const unsigned count, char *a) const
{
  unsigned count1=count;
  
  if ((count == 0) || (count > l))
    count1 = l;
    
  for (unsigned i = 0; i < count1; i++)
    a[i] = c[i];
    
  a[count1] = '\0';
}


void DString::Expand (const unsigned by)
{
  char *cp;
  unsigned by1;
  
  if (by == 0) by1 = 100;
  else by1 =by;
  
  cp = new char [m+by1+1];
  
  if (c) {
    for (unsigned i = 0; i < l; i++)
      cp[i] = c[i];
    delete [] c;
  }
  
  c = cp;
  c[l] = '\0';
  m += by1;
}


void DString::Reset (const unsigned length)
{
  if (length > m) {
    if (c) delete [] c;
    c = new char [length+1];
    m = length;
  }
  else if (!c) {
    c = new char [2];
    m = 1;
  }
  l = 0;
  if (c) c[0] = '\0';
}

  
Bitstring::Bitstring (const unsigned maxLen)
{ 
  unsigned nBytes=(maxLen+7)/8;
  
  l = 0; 
  m = nBytes; 
  if (m > 0) c = new char [m]; 
  else c = 0;
}
