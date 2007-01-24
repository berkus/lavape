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


#include "ChainA.h"
#pragma hdrstop


void ChainAny0::init (const ChainAny0 &c)

{
  ChainAnyElem *e1 = c.first, *e2;

  first = 0;
  last = 0;
  for (; e1; e1 = e1->successor) {
    e2 = e1->Clone();
    Append(e2);
  }
}


void ChainAny0::copy (const ChainAny0 &c)

{
  ChainAnyElem *e1 = c.first, *e2 = first, *oldElem=0;
  
  for (; e1; e1 = e1->successor, e2 = e2->successor) {
    if (!e2) {
      e2 = e1->Clone();
      Append(e2);
    }
    else
      e2->CopyData(e1);
    oldElem = e2;
  }
  if (e2) Cut(oldElem);
}


void ChainAny0::Append (ChainAnyElem * const newElem)

{
  if (!newElem) return;

  if (first) last->successor = newElem;
  else first = newElem;
  newElem->successor = 0;
  newElem->predecessor = last;
  last = newElem;
}

 
void ChainAny0::Prepend (ChainAnyElem * const newElem)

{
  if (!newElem) return;

  newElem->successor = first;
  newElem->predecessor = 0;
  if (first) first->predecessor = newElem;
  first = newElem;
  if (!last) last = newElem;
}


void ChainAny0::Insert (ChainAnyElem * const afterElem,
                        ChainAnyElem * const newElem)

{
  if (!newElem) return;
  
  if (!afterElem) {
    Prepend(newElem);
    return;
  }
  
  newElem->successor = afterElem->successor;
  if (afterElem->successor)
    afterElem->successor->predecessor = newElem;
  newElem->predecessor = afterElem;
  afterElem->successor = newElem;
  if (afterElem == last) last = newElem;
} 


void ChainAny0::Remove (ChainAnyElem * const afterElem)

{
  ChainAnyElem *elem;
  
  if (afterElem) {
    elem = afterElem->successor;
    if (!elem) return;
    if (elem == last)  last = afterElem;
    afterElem->successor = elem->successor;
  }
  else {
    elem = first;
    if (first == last) last = 0;
    if (first) first = first->successor;
    else return;
  }
  if (elem) {
    if (elem->successor)
      elem->successor->predecessor = afterElem;
    delete elem;
  }
} 


ChainAnyElem* ChainAny0::Uncouple(ChainAnyElem *elem)
  //remove elem from chain, return removed elem
{
  ChainAnyElem *pred, *succ;

  if (!elem) return 0;
  pred = elem->predecessor;
  succ = elem->successor;
  if (pred) pred->successor = elem->successor;
  else first = elem->successor;
  if (succ) succ->predecessor = elem->predecessor;
  else last = elem->predecessor;
  return elem;
}


ChainAnyElem* ChainAny0::Pop (ChainAnyElem **firstElem)

{
  ChainAnyElem *result;
  
  result = first;
  if (firstElem) *firstElem = first;
  first = first->successor;
  if (first)
    first->predecessor = 0;
  else
    last = 0;
  return result;
} 


void ChainAny0::Cut (ChainAnyElem * const afterElem)

{
  ChainAnyElem *elem, *next;
  
  if (afterElem) {
    elem = afterElem->successor;
    afterElem->successor = 0;
  }
  else {
    elem = first;
    first = 0;
  }
  
  while (elem) {
    next = elem->successor;
    delete elem;
    elem = next;
  }
  last = afterElem;
} 

ChainAnyElem* ChainAny0::GetNth(int nn)
{
  ChainAnyElem* elem=0;
  if (nn) {
    int ii = 1;
    elem = first;
    while ( (ii < nn) && elem ) {
      elem = elem->successor;
      ii += 1;
    }
  }
  return elem;
}

bool ChainAny0::AddNth(int nn, ChainAnyElem * newElem)
{
  ChainAnyElem* afterElem=0;
  if (nn == 1)
    Prepend(newElem);
  else {
    if (nn > 1) {
      int ii = 1;
      afterElem = first;
      while ( (ii < nn-1) && afterElem ) {
        afterElem = afterElem->successor;
        ii += 1;
      }
      if (afterElem)
        Insert(afterElem, newElem);
      else
        return false;
    }
  }
  return true;
}

ChainAnyElem* ChainAny0::UncoupleNth(int nn)
{
  ChainAnyElem* elem=0;
  if (nn > 0) {
    int ii = 1;
    elem = first;
    while ( (ii < nn) && elem ) {
      elem = elem->successor;
      ii += 1;
    }
  }
  if (elem)
    Uncouple(elem);
  return elem;
}

void ChainAny0::Destroy()

{
  ChainAnyElem *elem, *next;
  
  for (elem = first; elem; elem = next) {
    next = elem->successor;
    delete elem;
  }
  first = 0;
  last = 0;
}

void CHE0::CopyData (ChainAnyElem *c)

{
  if (!((CHE0*)c)->data) {
    delete data;
    data = 0;
  }
  else {
    if (data)
      delete data;
    data = (DObject*)((CHE0*)c)->data->Clone();
  }
}
