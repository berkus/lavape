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


#include "DIO.h"
#include "GUIProg.h"
#include "TreeSrch.h"

#pragma hdrstop


CHEFormNode* TreeSrchCLASS::FirstNode ()
{
  CHEFormNode* trp = GUIProg->Root;
  if (trp && trp->data.Atomic && !trp->data.StringValue.c)
      trp =(CHEFormNode*) trp->successor;
  return trp;
} // END OF FirstNode


void TreeSrchCLASS::NextNodeNotIGNORED (CHEFormNode*& chFrmNd,
                                         CHEFormNode*& syo)
{
  syo = 0;
  if (!chFrmNd) return;
  if (!chFrmNd->data.Atomic) {
    syo = (CHEFormNode*)chFrmNd->data.SubTree.first;
    if (syo && syo->data.IterFlags.Contains(Ignore))
      if ((CHEFormNode*)syo->successor) 
        for (syo = (CHEFormNode*)syo->successor;
             syo && syo->data.IterFlags.Contains(Ignore);
             syo = (CHEFormNode*)syo->successor);
      else  
        syo = 0;
  }
  if ((CHEFormNode*)chFrmNd->successor) {
    for (chFrmNd =(CHEFormNode*)chFrmNd->successor;
         chFrmNd && chFrmNd->data.IterFlags.Contains(Ignore); 
         chFrmNd =(CHEFormNode*) chFrmNd->successor);
  }
  else chFrmNd = 0;
} // END OF NextNodeNotIGNORED


void TreeSrchCLASS::NextNodeNotIGNOREDorEll (CHEFormNode*& chFrmNd, CHEFormNode*& syo)
{
  syo = 0;
  if (!chFrmNd) return;
  if (!chFrmNd->data.Atomic
      && !chFrmNd->data.IterFlags.Contains(Ellipsis)
      && !chFrmNd->data.IoSigFlags.Contains(DONTPUT) ) {
    syo = (CHEFormNode*)chFrmNd->data.SubTree.first;
    if (syo && syo->data.IterFlags.Contains(Ignore))
      if ((CHEFormNode*)syo->successor) {
        for (syo = (CHEFormNode*)syo->successor;
             syo && syo->data.IterFlags.Contains(Ignore);
             syo = (CHEFormNode*)syo->successor);
      }
      else
        syo = 0;
  }
  if ((CHEFormNode*)chFrmNd->successor) {
    for (chFrmNd = (CHEFormNode*)chFrmNd->successor;
         chFrmNd && chFrmNd->data.IterFlags.Contains(Ignore);
         chFrmNd = (CHEFormNode*)chFrmNd->successor);
  }
  else
    chFrmNd = 0;
} // END OF NextNodeNotIGNOREDorEll


void TreeSrchCLASS::nextUnprotected (CHEFormNode* trp,
                                      CHEFormNode* fmno,
                                      bool& begin,
                                      bool& found,
                                      CHEFormNode*& trpn)
{
  CHEFormNode* chFrmNd, *syo;

  chFrmNd = fmno;
  while (chFrmNd && !found) {
    if (begin) {
      if (chFrmNd->data.IoSigFlags.Contains(UnprotectedUser)) {
        trpn = chFrmNd;
        found = true;
        return;
      }
      else {
        if (chFrmNd->data.IoSigFlags.Contains(Signature)
               && chFrmNd->data.IoSigFlags.Contains(Flag_INPUT)) {
          trpn = chFrmNd;
          found = true;
          return;
        }
        else {
          if (!chFrmNd->data.Atomic && chFrmNd->data.IterFlags.Contains(Ellipsis)) {
            trpn = chFrmNd;
            found = true;
            return;
          }
        }
      }
    }
    if (chFrmNd == trp)
      begin = true;
    NextNodeNotIGNOREDorEll(chFrmNd,syo);
    if (syo && (syo->data.FIP.up))
      if (!syo->data.FIP.up->data.BasicFlags.Contains(PopUp)
            || syo->data.FIP.up->data.FIP.poppedUp)
        nextUnprotected(trp,syo,begin,found,trpn);
  }
} // END OF nextUnprotected


CHEFormNode* TreeSrchCLASS::NextUnprotected (CHEFormNode* trp, CHEFormNode* frmPtr)
{
  CHEFormNode* trpn, *trpa;
  bool found, begin;

  if (!trp )
    return 0;
  trpn = 0;
  found = false;
  begin = false;
  if (frmPtr)
    trpa = frmPtr;
  else
    trpa = GUIProg->Root;
  nextUnprotected(trp,trpa,begin,found,trpn);
  return trpn;
} // END OF NextUnprotected


void TreeSrchCLASS::nUnpInSubtree (CHEFormNode* trp,
                                    CHEFormNode* fmno,
                                    bool& begin,
                                    bool& found,
                                    CHEFormNode*& trpn)
{
  CHEFormNode* chFrmNd, *syo;

  chFrmNd = fmno;
  while (chFrmNd
         && !found) {
    if (begin)
      if (chFrmNd->data.IoSigFlags.Contains(UnprotectedUser)) {
        trpn = chFrmNd;
        found = true;
        return;
      }
      else if (chFrmNd->data.IoSigFlags.Contains(Signature)
               && chFrmNd->data.IoSigFlags.Contains(Flag_INPUT)) {
        trpn = chFrmNd;
        found = true;
        return;
      }
      else if (!chFrmNd->data.Atomic
               && chFrmNd->data.IterFlags.Contains(Ellipsis)) {
        trpn = chFrmNd;
        found = true;
        return;
      }
    if (chFrmNd == trp)
      begin = true;
    NextNodeNotIGNOREDorEll(chFrmNd,syo);
    if (syo)
      if (syo->data.FIP.up)
        if (!syo->data.FIP.up->data.BasicFlags.Contains(PopUp)
            || syo->data.FIP.up->data.FIP.poppedUp) {
          nUnpInSubtree(trp,syo,begin,found,trpn);
          if (syo->data.FIP.up == trp) {
            begin = false;
          }
        }
  }
} // END OF nUnpInSubtree

CHEFormNode* TreeSrchCLASS::NUnpInSubtree (CHEFormNode* trp)
{
  CHEFormNode* trpn, *trpa;
  bool found, begin;

  trpn = 0;
  if (!trp ) return trp;

  found = false;
  begin = false;
  trpa = GUIProg->Root;
  nUnpInSubtree(trp,trpa,begin,found,trpn);
  return trpn;
} // END OF NUnpInSubtree



void TreeSrchCLASS::prevUnprotected (CHEFormNode* trp,
                                      CHEFormNode* fmno,
                                      bool& begin,
                                      bool& found,
                                      CHEFormNode*& trpn)
{
  CHEFormNode* chFrmNd, *syo;

  chFrmNd = fmno;
  while (chFrmNd
         && (!found)) {
    if (chFrmNd == trp) {
      begin = true;
      found = true;
      return;
    }
    if (!begin
        && chFrmNd->data.IoSigFlags.Contains(UnprotectedUser))
      if (chFrmNd->data.FIP.up
          && (chFrmNd->data.FIP.up->data.BasicFlags.Contains(ButtonMenu)
              || chFrmNd->data.FIP.up->data.BasicFlags.Contains(OptionMenu)
              || chFrmNd->data.FIP.up->data.BasicFlags.Contains(PopUpMenu))) {
        trpn = chFrmNd;
      }
      else trpn = chFrmNd;
    else if (!begin
             && chFrmNd->data.IoSigFlags.Contains(Signature)
             && chFrmNd->data.IoSigFlags.Contains(Flag_INPUT)) {
      trpn = chFrmNd;
    }
    else if (!begin
             && !chFrmNd->data.Atomic
             && chFrmNd->data.IterFlags.Contains(Ellipsis)) {
      trpn = chFrmNd;
      return;
    }
    NextNodeNotIGNOREDorEll(chFrmNd,syo);
    if (syo)
      if (syo->data.FIP.up
          && (!syo->data.FIP.up->data.BasicFlags.Contains(PopUp)
              || syo->data.FIP.up->data.FIP.poppedUp))
        prevUnprotected(trp,syo,begin,found,trpn);
  }
} // END OF prevUnprotected

CHEFormNode* TreeSrchCLASS::PrevUnprotected (CHEFormNode* trp)
{
  CHEFormNode* trpn, *trpa;
  bool found, begin;

  trpn = 0;
  if (!trp ) return trp;

  found = false;
  begin = false;
  trpa = GUIProg->Root;

  prevUnprotected(trp,trpa,begin,found,trpn);

  return trpn;
} // END OF PrevUnprotected


void TreeSrchCLASS::nthUnprotected (CHEFormNode* fmno,
                                     bool& found,
                                     CHEFormNode*& trpn,
                                     unsigned n,
                                     unsigned& i)
{
  CHEFormNode* chFrmNd, *syo;

  chFrmNd = fmno;
  while (chFrmNd
         && (!found)) {
    if (chFrmNd->data.IoSigFlags.Contains(UnprotectedUser)
        || (chFrmNd->data.IoSigFlags.Contains(Signature)
            && chFrmNd->data.IoSigFlags.Contains(Flag_INPUT))
        || (!chFrmNd->data.Atomic
            && chFrmNd->data.IterFlags.Contains(Ellipsis))) {
      i++;
      if (i >= n) {
        found = true;
        trpn = chFrmNd;
        return;
      }
    }

    NextNodeNotIGNOREDorEll(chFrmNd,syo);
    if (syo)
      if (syo->data.FIP.up
          && (!syo->data.FIP.up->data.BasicFlags.Contains(PopUp)
              || syo->data.FIP.up->data.FIP.poppedUp))
        nthUnprotected(syo,found,trpn,n,i);
  }
} // END OF nthUnprotected

CHEFormNode* TreeSrchCLASS::NthUnprotected (unsigned n)
{
  CHEFormNode* trpn, *trp;
  unsigned i;
  bool found;

  trp = GUIProg->Root;
  trpn = 0;
  i = 0;
  found = false;

  nthUnprotected(trp,found,trpn,n,i);

  return trpn;
} // END OF NthUnprotected

void TreeSrchCLASS::lastUnprotected (CHEFormNode* fmno,
                                      bool& found,
                                      CHEFormNode*& trpn)
{
  CHEFormNode* chFrmNd, *syo;

  chFrmNd = fmno;
  while (chFrmNd
         && (!found)) {
    if (chFrmNd->data.IoSigFlags.Contains(UnprotectedUser)
        || (chFrmNd->data.IoSigFlags.Contains(Signature)
            && chFrmNd->data.IoSigFlags.Contains(Flag_INPUT)))
      trpn = chFrmNd;
    else if (!chFrmNd->data.Atomic
             && chFrmNd->data.IterFlags.Contains(Ellipsis)) {
      trpn = chFrmNd;
      return;
    }

    NextNodeNotIGNOREDorEll(chFrmNd,syo);
    if (syo)
      if (syo->data.FIP.up
          && (!syo->data.FIP.up->data.BasicFlags.Contains(PopUp)
              || syo->data.FIP.up->data.FIP.poppedUp))
        lastUnprotected(syo,found,trpn);
  }
} // END OF lastUnprotected

CHEFormNode* TreeSrchCLASS::LastUnprotected (CHEFormNode* trp)
{
  CHEFormNode* trpn;
  bool found;
  trpn = 0;
  if (!trp ) return trpn;
  found = false;
  lastUnprotected(trp,found,trpn);
  return trpn;
} // END OF LastUnprotected


void TreeSrchCLASS::INIT (CGUIProgBase *guiPr)
{
  GUIProg = guiPr;
  itertrp = 0;
  optrp = 0;
}

