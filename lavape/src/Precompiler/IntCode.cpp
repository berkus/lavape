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

               implementation of class IntCodeCLASS

 **********************************************************************/

#include "IntCode.h"


IntCodeCLASS IntCode;
static bool __INITstarted=false;

#ifdef WIN32
  char slash[2]="\\";
#else
  char slash[2]="/";
#endif


/************************************************************************/

void IntCodeCLASS::appendToStack ()

{

  /* append new node as root node, son, or right neighbour to tree */
  if (TreeRoot == 0)
    TreeRoot = treeNodePtr;
  else
    if (!stack[iStack]->Atomic
        && (stack[iStack]->Down == 0)) {
      stack[iStack++]->Down = treeNodePtr;
      /* Push new node down onto the stack */
      if (iStack >= stackSize)
        PCerrors.InternalError(StackOverflow);
    }
    else
      stack[iStack]->Right = treeNodePtr;
  stack[iStack] = treeNodePtr;
}


void IntCodeCLASS::appendAnnot ()

{

  /* append annotation to the current node's annotation chain */
  if (stack[iStack]->Flags.Contains(AnnotationNode))
    stack[iStack]->Right = treeNodePtr;
  else {
    stack[iStack]->Annotation = treeNodePtr;
    iStack++;
    if (iStack >= stackSize)
      PCerrors.InternalError(StackOverflow);
  }
  stack[iStack] = treeNodePtr;
}


void IntCodeCLASS::DefTypeIdent ()

{
  CheckPush(IdentifierA);
  CurrentDefType = TopOfStack();
}


void IntCodeCLASS::KeepDefType (TreeNodePtr node)

{
  CurrentDefType = node;
}


void IntCodeCLASS::RefTypeIdent ()

{
  CheckPush(IdentifierA);
  CurrentRefType = TopOfStack();
}


void IntCodeCLASS::Push (SyntacticType sT)

{
  if (PCerrors.Recovery) return;
  /* create and initialize new tree node */
  treeNodePtr = new TreeNode;
  treeNodePtr->Atomic = false;
  treeNodePtr->SynType = sT;
  treeNodePtr->Down = 0;
  treeNodePtr->Right = 0;
  treeNodePtr->Annotation = 0;
  treeNodePtr->SourceLine = AtomLine;
  treeNodePtr->SourceCol = AtomCol;
  treeNodePtr->Flags = SET();
  if (IncludeLevel) treeNodePtr->Flags.INCL(Included);
  appendToStack();
}
 
 
void IntCodeCLASS::Interpose (SyntacticType sT)
 
{
  if (PCerrors.Recovery) return;
  /* create and initialize new tree node */
  treeNodePtr = new TreeNode;
  treeNodePtr->Atomic = false;
  treeNodePtr->SynType = sT;
  treeNodePtr->Down = stack[iStack-1]->Down;
  treeNodePtr->Right = 0;
  treeNodePtr->Annotation = 0;
  treeNodePtr->SourceLine = AtomLine;
  treeNodePtr->SourceCol = AtomCol;
  treeNodePtr->Flags = SET();
  if (IncludeLevel) treeNodePtr->Flags.INCL(Included);
  stack[iStack-1]->Down = treeNodePtr;
  stack[iStack] = treeNodePtr;
}
 
 
void IntCodeCLASS::InterposeDontPop (SyntacticType sT)
 
{
  if (PCerrors.Recovery) return;
  if (!(stack[iStack]->Atomic || stack[iStack]->Down)) {
    Push(sT);
    return;
  }
  
  if (iStack+1 >= stackSize)
    PCerrors.InternalError(StackOverflow);
    
  /* create and initialize new tree node */
  treeNodePtr = new TreeNode;
  treeNodePtr->Atomic = false;
  treeNodePtr->SynType = sT;
  treeNodePtr->Down = stack[iStack-1]->Down;
  treeNodePtr->Right = 0;
  treeNodePtr->Annotation = 0;
  treeNodePtr->SourceLine = AtomLine;
  treeNodePtr->SourceCol = AtomCol;
  treeNodePtr->Flags = SET();
  if (IncludeLevel) treeNodePtr->Flags.INCL(Included);
  stack[iStack-1]->Down = treeNodePtr;
  stack[iStack+1] = stack[iStack];
  stack[iStack++] = treeNodePtr;
}


void IntCodeCLASS::PushAnnotation (SyntacticType sT)

{
  if (PCerrors.Recovery) return;
  /* create and initialize new tree node */
  treeNodePtr = new TreeNode;
  treeNodePtr->Atomic = false;
  treeNodePtr->SynType = sT;
  treeNodePtr->Down = 0;
  treeNodePtr->Right = 0;
  treeNodePtr->SourceLine = AtomLine;
  treeNodePtr->SourceCol = AtomCol;
  treeNodePtr->Flags.INCL(AnnotationNode);
  appendAnnot();
}


void IntCodeCLASS::PushAtom ()

{
  if (PCerrors.Recovery) return;
  /* create and initialize new tree node */
  treeNodePtr = new TreeNode;
  treeNodePtr->Atomic = true;
  treeNodePtr->Right = 0;
  treeNodePtr->Annotation = 0;
  treeNodePtr->SourceLine = AtomLine;
  treeNodePtr->SourceCol = AtomCol;
  treeNodePtr->Flags = SET();
  if (IncludeLevel) treeNodePtr->Flags.INCL(Included);
  treeNodePtr->Atom = Atom;
  switch (Atom) {

  case IdentifierA:
  case CharConst:
  case StringConst:
  case IntConst:
  case OctalConst:
  case HexConst:
  case FloatConst:
    treeNodePtr->StringBufferPos = StringBufferPos;
    break;
  }
  appendToStack();
}


void IntCodeCLASS::PushEmpty ()

{
  if (PCerrors.Recovery
      || stack[iStack]->Atomic
      || (stack[iStack]->Down != 0))
    return;

  treeNodePtr = new TreeNode;
  treeNodePtr->Atomic = true;
  treeNodePtr->Atom = NoAtom;
  treeNodePtr->Right = 0;
  treeNodePtr->Annotation = 0;
  treeNodePtr->SourceLine = AtomLine;
  treeNodePtr->SourceCol = AtomCol;
  treeNodePtr->Flags = SET();
  if (IncludeLevel) treeNodePtr->Flags.INCL(Included);
  appendToStack();
}


void IntCodeCLASS::Pop ()

{
  if (PCerrors.Recovery) return;
  if (iStack == 0)
    PCerrors.InternalError(StackUnderflow);
  else
    iStack--;
}


void IntCodeCLASS::PopAnnotation ()

{
  if (PCerrors.Recovery) return;
  if (stack[iStack]->Flags.Contains(AnnotationNode)) Pop();
}


TreeNodePtr IntCodeCLASS::TopOfStack ()

{
  if (PCerrors.Recovery) return 0;
  return stack[iStack];
}


unsigned IntCodeCLASS::DepthOfStack ()

{
  if (PCerrors.Recovery) return 0;
  return iStack;
}


void IntCodeCLASS::SaveStack (TreeNodePtr& tos)

{
  if (PCerrors.Recovery) return;
  tos = stack[iStack];
  tos->Down = TreeNodePtr(1);
}


void IntCodeCLASS::RestoreStack (TreeNodePtr tos)

{
  if (PCerrors.Recovery) return;
  stack[iStack] = tos;
  tos->Down = 0;
}


void IntCodeCLASS::ResetStack ()

{
  if (PCerrors.Recovery) return;
  iStack = 0;
}


TreeNodePtr IntCodeCLASS::FindAnnos (TreeNodePtr node,
                                     SyntacticType annoType)

{
  if (!(node->Flags.Contains(AnnotationNode)))
    node = node->Annotation;
  while (node && (node->SynType != annoType))
    node = node->Right;

  return node;
}


/************************************************************************/

unsigned IntCodeCLASS::hashCode (char *s)

{
  unsigned hcode, k;
  hcode = 0;
  for (k = 0; ; k++) {
    if (s[k] == '\0')
      return hcode % hashPrimeIC;
    hcode += s[k];
  }
}


/************************************************************************/

void IntCodeCLASS::LookUpType (TStringBufferPos id,
                               TypeTablePtr& entryFound)
{
  AtomPtr idPtr = &StringBuffer[id], entryIdPtr;

  hc = hashCode(idPtr);
  entryFound = hashTable[hc].firstEntry;
  for (;;) {
    if (entryFound == 0)
      break;
    entryIdPtr = &StringBuffer[entryFound->id];
    if (Compare(idPtr,entryIdPtr) == 0)
      break;
    entryFound = entryFound->nextEntry;
  }
}


void IntCodeCLASS::appendType (TypeTablePtr& newEntry)

{
  newEntry = new TypeTableEntry;

  /* new entry in hash table: */
  newEntry->nextEntry = 0;
  if (hashTable[hc].firstEntry == 0)
    hashTable[hc].firstEntry = newEntry;
  else
    hashTable[hc].lastEntry->nextEntry = newEntry;
  hashTable[hc].lastEntry = newEntry;

  /* new entry in global linear type table: */
  newEntry->successor = 0;
  if (TypeTable == 0)
    TypeTable = newEntry;
  else
    lastType->successor = newEntry;
  lastType = newEntry;
}


void IntCodeCLASS::TypeDef ()

{
  TypeTablePtr entryFound, newEntry;
  FlagSet generatedProcs;
  CHETypeTablePtr *variant;
  DString currentModule(CmdParms.CurrentIncludeFile);
  unsigned loc;

  if (PCerrors.Recovery) return;
  
  if (CDPfromEntry)
    for (;CDPfromEntry->CDPfrom;
          CDPfromEntry = CDPfromEntry->CDPfrom);
  if (SIGfromEntry)
    for (;SIGfromEntry->SIGfrom;
          SIGfromEntry = SIGfromEntry->SIGfrom);
        
  LookUpType(CurrentDefType->StringBufferPos,entryFound);
  if (!entryFound) {
    appendType(newEntry);
    newEntry->id = CurrentDefType->StringBufferPos;
    newEntry->baseType = BaseTypeEntry;
    if (BaseTypeEntry
        && (CurrentTypeSpecifier->SynType == STclass
          || CurrentTypeSpecifier->SynType == STstruct)) {
      variant = new CHETypeTablePtr;
      variant->data = newEntry;
      BaseTypeEntry->variantTypes.Append(variant);
    }
    newEntry->CDPfrom = CDPfromEntry;
    newEntry->SIGfrom = SIGfromEntry;
    newEntry->refModes = SET();
    newEntry->typeIndex = typeInd;
    newEntry->indexLevel = indexLevel;
    newEntry->isForm = IsForm;
    
    if (CmdParms.CurrentIncludeFile.l > 0) {
      newEntry->module = CmdParms.CurrentIncludeFile;
      while (newEntry->module.Contains(slash,0,loc))
      newEntry->module = newEntry->module.Substr(loc+1,0);
      newEntry->module.Contains(".",0,loc);
      newEntry->module = newEntry->module.Substr(0,loc);
    }
    
    newEntry->typeSpecifier = CurrentTypeSpecifier;
    newEntry->declarator = CurrentDeclarator;
    newEntry->attributes = attributes;
    newEntry->procGenAttributes = procGenAttributes;
    newEntry->hasConstructor = HasConstructor;
  }
  else {
    if (entryFound->typeSpecifier) {
      PCerrors.ContextError(DuplTypeDef,
          CurrentDefType->SourceLine,
          CurrentDefType->SourceCol);
      return;
    }
    if ((typeInd == 0)
        && entryFound->refModes.Contains(FormRef)) {
      PCerrors.ContextError(NoFormType,
          CurrentDefType->SourceLine,
          CurrentDefType->SourceCol);
      return;
    }
    entryFound->typeIndex = typeInd;
    entryFound->indexLevel = indexLevel;
    entryFound->isForm = IsForm;
    
    if (CmdParms.CurrentIncludeFile.l > 0) {
      entryFound->module = CmdParms.CurrentIncludeFile;
      while (entryFound->module.Contains(slash,0,loc))
        entryFound->module = entryFound->module.Substr(loc+1,0);
      entryFound->module.Contains(".",0,loc);
      entryFound->module = entryFound->module.Substr(0,loc);
    }
    else
      entryFound->module.Destroy();
    
    entryFound->CDPfrom = CDPfromEntry;
    entryFound->SIGfrom = SIGfromEntry;
    entryFound->typeSpecifier = CurrentTypeSpecifier;
    entryFound->declarator = CurrentDeclarator;
    entryFound->attributes = attributes;
    entryFound->procGenAttributes = procGenAttributes;
    entryFound->hasConstructor = HasConstructor;
  }
  CDPfromEntry = 0;
  SIGfromEntry = 0;
  HasConstructor = false;
  
  generatedProcs = procGenAttributes*attributes;
  if (generatedProcs.Contains(HasCDP))
    GlobalProperties.Flags.INCL(CDPgen);
  if (generatedProcs.Contains(HasFIO))
    GlobalProperties.Flags.INCL(FIOgen);
  if (generatedProcs.Contains(HasSIG)) {
    GlobalProperties.Flags.INCL(SIGNgen);
    GlobalProperties.Flags.INCL(CDPgen);
  }
  
  if ((GlobalProperties.Flags * SET(CDPgen,FIOgen,SIGNgen) != SET())
      && !IncludeLevel)
    FunctionsAreToBeGenerated = true;
}


void IntCodeCLASS::TypeRef (FlagSet& attrib)

{
  TypeTablePtr entryFound;

  if (PCerrors.Recovery) return;

  /* enter reference mode for referenced type */
  LookUpType(CurrentRefType->StringBufferPos,entryFound);
  if (entryFound) {
    if (entryFound->typeSpecifier) {
      attrib =
        entryFound->procGenAttributes * entryFound->attributes;
      if (entryFound->procGenAttributes.Contains(HasCDP))
        attrib.INCL(HasCDP);
      if (entryFound->attributes.Contains(HasSYN))
        attrib.INCL(HasSYN);
      /* procGenAttributes alone shall determine whether
         a referenced type has CDP:
         "$TYPE +CDP  struct T {...}-CDP;"
         shall mean that T has a (manually programmed)
         CDP elsewhere. */
      }
    else
      attrib = DefaultAttributes;
  }
  else
    attrib = DefaultAttributes;
  
  attrib -= SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1);
}


void IntCodeCLASS::CheckTypeDefined (bool noAlibi,
                                     bool shouldBeFormType)

{
//  DString currentRefType;

  if (PCerrors.Recovery || !CurrentRefType) return;

  LookUpType(CurrentRefType->StringBufferPos,CurrentTypeEntry);
  if (CurrentTypeEntry) {
    CurrentTypeEntry->refModes += RefModes;
    if (!CurrentTypeEntry->typeSpecifier) {
//      currentRefType = &StringBuffer[CurrentRefType->StringBufferPos];
//      currentDefType = &StringBuffer[CurrentDefType->StringBufferPos];
//      if ((Atom == Lparenth)
//          && (currentRefType == currentDefType))
//        return;
      if (noAlibi) {
        PCerrors.ContextError(UndefinedType,
                              CurrentRefType->SourceLine,
                              CurrentRefType->SourceCol);
        goto ret;
      }
    }
    else
      if (shouldBeFormType
          && !CurrentTypeEntry->isForm) { /* no form type */
        PCerrors.ContextError(NoFormType,
                              CurrentRefType->SourceLine,
                              CurrentRefType->SourceCol);
        goto ret;
      }

    if (CurrentTypeEntry->refModes.Contains(ChainAnyRef)
        && IncludeLevel)
      CurrentTypeEntry->refModes.INCL(ChainAnyRefElseWhere);      
    if (CurrentTypeEntry->refModes.Contains(NestedAnyRef)
        && IncludeLevel)
      CurrentTypeEntry->refModes.INCL(NestedAnyRefElseWhere);
    if (CurrentTypeEntry->refModes.Contains(NestedRef)
        && IncludeLevel)
      CurrentTypeEntry->refModes.INCL(NestedRefElseWhere);
    if (CurrentTypeEntry->refModes.Contains(Nested0Ref)
        && IncludeLevel)
      CurrentTypeEntry->refModes.INCL(Nested0RefElseWhere);
  }
  else
    if (RefModes * SET(ChainAnyRef,NestedAnyRef,-1) != SET()) {
//    if (RefModes * SET(ChainAnyRef,NestedAnyRef,Nested0Ref,NestedRef,-1) != SET()) {
      appendType(CurrentTypeEntry);
      CurrentTypeEntry->id = CurrentRefType->StringBufferPos;
      CurrentTypeEntry->baseType = 0;
      CurrentTypeEntry->attributes = SET();
      CurrentTypeEntry->procGenAttributes = procGenAttributes;
      CurrentTypeEntry->typeIndex = 0;
      CurrentTypeEntry->isForm = false;
      CurrentTypeEntry->refModes = RefModes;
      CurrentTypeEntry->typeSpecifier = 0;
      CurrentTypeEntry->declarator = 0;
      CurrentTypeEntry->CDPfrom = 0;
      CurrentTypeEntry->SIGfrom = 0;
      if (CurrentTypeEntry->refModes.Contains(ChainAnyRef)
          && IncludeLevel)
        CurrentTypeEntry->refModes.INCL(ChainAnyRefElseWhere);      
      if (CurrentTypeEntry->refModes.Contains(NestedAnyRef)
          && IncludeLevel)
        CurrentTypeEntry->refModes.INCL(NestedAnyRefElseWhere);
      if (CurrentTypeEntry->refModes.Contains(Nested0Ref)
          && IncludeLevel)
        CurrentTypeEntry->refModes.INCL(Nested0RefElseWhere);
      if (CurrentTypeEntry->refModes.Contains(NestedRef)
          && IncludeLevel)
        CurrentTypeEntry->refModes.INCL(NestedRefElseWhere);
    }
    else {
//      currentRefType = &StringBuffer[CurrentRefType->StringBufferPos];
//      currentDefType = &StringBuffer[CurrentDefType->StringBufferPos];
//      if ((Atom == Lparenth)
//          && (currentRefType == currentDefType))
//        return;
      if (noAlibi) {
        PCerrors.ContextError(UndefinedType,
                              CurrentRefType->SourceLine,
                              CurrentRefType->SourceCol);
      }
      goto ret;
    }
  
  if (IncludeLevel) goto ret;
  
  if ((CurrentTypeEntry->refModes.Contains(ChainAnyRef)
       && !CurrentTypeEntry->refModes.Contains(ChainAnyRefElseWhere))
      || (CurrentTypeEntry->refModes.Contains(NestedAnyRef)
      && !CurrentTypeEntry->refModes.Contains(NestedAnyRefElseWhere))
      ||
        ((CurrentTypeEntry->refModes.Contains(Nested0Ref)
          || CurrentTypeEntry->refModes.Contains(NestedRef))
         && !CurrentTypeEntry->refModes.Contains(Nested0RefElseWhere)
         && !CurrentTypeEntry->refModes.Contains(NestedRefElseWhere))) {
    FunctionsAreToBeGenerated = true;
    if (!CurrentTypeEntry->typeSpecifier)
//        && (procGenAttributes*attributes == SET()))
      CurrentTypeEntry->module.Reset(1);
      // to enforce Chain/Nested-generation in TypCoder.CHENSTs
  }
  ret:
  RefModes = SET();
  CurrentRefType = 0;
}


void IntCodeCLASS::CheckAllTypesDefined ()

{
  TypeTablePtr typeEntry;

  AtomLine--; /* last line of source */
  AtomCol = 1;
  typeEntry = TypeTable;
  for (typeEntry = TypeTable;
       typeEntry;
       typeEntry = typeEntry->successor)
    if (!typeEntry->typeSpecifier
        && (typeEntry->attributes*typeEntry->procGenAttributes != SET())
          && (typeEntry->refModes * SET(ChainAnyRef,NestedAnyRef,Nested0Ref,NestedRef,-1)
            == SET()))
      PCerrors.Missing(typeEntry->id);
}


void IntCodeCLASS::PrepareProcDcls ()

{
  CHETypeTablePtr *chp;

  if (PCerrors.Recovery) return;
  if ((CurrentTypeEntry->attributes
       * CurrentTypeEntry->procGenAttributes )
      == SET(-1)
      &&
      (CurrentTypeEntry->refModes
       * SET(ChainAnyRef,NestedAnyRef,Nested0Ref,NestedRef)
       == SET(-1)))
    return;
  
  chp = new CHETypeTablePtr;
  chp->data = CurrentTypeEntry;
  ProcTypes.Append(chp);
}


/* **********************************************************************/



void IntCodeCLASS::FindEndOfReferenceChain (TreeNodePtr startNode,
                                            TreeNodePtr& endNode,
                                            TreeNodePtr& indirRefNode,
                                            bool& isLast)

{
  TypeTablePtr entryFound;
  bool isLast1;

  if (PCerrors.Recovery) return;
  
  if ((startNode->SynType == STsimpleType)
      && (startNode->Down->Atom == IdentifierA)) {
    isLast = false;
    LookUpType(startNode->Down->StringBufferPos,entryFound);
    if (entryFound == 0) {
      endNode = 0;
      indirRefNode = 0;
      return;
    }

    FindEndOfReferenceChain(entryFound->typeSpecifier,endNode,indirRefNode,
          isLast1);
    if (isLast1)
      indirRefNode = startNode;
  }
  else {
    endNode = startNode;
    indirRefNode = startNode;
    isLast = true;
  }
}



/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void IntCodeCLASS::INIT ()
{  
  if (__INITstarted) return;
  __INITstarted = true;

  Atoms_INIT();
  PCerrors.INIT();
  
  IncludeLevel = 0;
  FunctionsAreToBeGenerated = false;
  HasConstructor = false;
  PopupNode = 0;
  iStack = 0;
  TreeRoot = 0;
  indent = 0;
  TypeTable = 0;
  BaseTypeEntry = 0;
  CDPfromEntry = 0;
  SIGfromEntry = 0;

  for (unsigned i = 0; i <= hashPrimeIC-1; i++)
    hashTable[i].firstEntry = 0;

}
