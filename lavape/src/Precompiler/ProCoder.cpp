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


#include "ProCoder.h"

#include "IntCode.h"
#include "TypCoder.h"
#include "CdpCoder.h"
#include "FioCoder.h"
#include "SigCoder.h"

ProcCoderCLASS ProcCoder;



void ProcCoderCLASS::CodeProcs (TreeNodePtr node)

{
  for (; node; node = node->Right) {
    DeclSpecifiers(node->Down->Down);
    if (node->Down->Right) declaratorList(node->Down->Right->Down);
  }
}


void ProcCoderCLASS::DeclSpecifiers (TreeNodePtr node)

{
  for (; node; node = node->Right)
    if (node->Atomic)
      { if (node->Atom == NoAtom) {
    IntCode.CurrentTypeSpecifier = 0;
    return;
  }
      }
    else {
      if (IntCode.ScopeNestingLevel)
        IntCode.CurrentTypeSpecifier = node;
      else
  switch (node->SynType) {
  case STclass:
  case STstruct:
  case STenumeration:
    IntCode.SetCurrentTypeEntry(node->Down);
//    switch (node->SynType) {
//    case STclass:
//    case STstruct:
//      if (!IntCode.CurrentTypeEntry->baseType)
//        TypCoder.CodeAssOperator();
//    }
    TypCoder.CodeCHENST();
    CdpCoder.CDProcGeneration();
    FioCoder.FIOProcGeneration();
    SigCoder.SIGProcGeneration();
  }
      break;
    }
}


void ProcCoderCLASS::declaratorList (TreeNodePtr node)

{
  TreeNodePtr decNode;
  
  for (; node; node = node->Right) {
    decNode=node->Down;
    if (decNode->Atomic) {
      if (decNode->Atom != IdentifierA) continue;
    }
    else {
      if (decNode->SynType == STpopup)
        decNode = decNode->Down;
      else
        continue;
    }
    IntCode.SetCurrentTypeEntry(decNode);
    TypCoder.CodeCHENST();
    CdpCoder.CDProcGeneration();
    FioCoder.FIOProcGeneration();
    SigCoder.SIGProcGeneration();
  }
}


void ProcCoderCLASS::CodeDeferredProcs ()

{
  CHETypeTablePtr *currentTTElistElem;
  
  for (currentTTElistElem = (CHETypeTablePtr*)IntCode.ProcTypes.first;
       currentTTElistElem;
       currentTTElistElem = (CHETypeTablePtr*)currentTTElistElem->successor) {
    IntCode.CurrentTypeEntry = currentTTElistElem->data;
    TypCoder.CodeCHENST();
    CdpCoder.CDProcGeneration();
    FioCoder.FIOProcGeneration();
    SigCoder.SIGProcGeneration();
  }
  IntCode.ProcTypes.Destroy();
}
