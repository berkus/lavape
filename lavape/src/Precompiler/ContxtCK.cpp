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

               implementation of class ContextCKCLASS

 **********************************************************************/

#include "ContxtCK.h"


ContextCKCLASS ContextCK;
static bool __INITstarted=false;


/*************************************************************************/
/* context dependent checks: */


void ContextCKCLASS::CheckCaseLabelExpr (TreeNodePtr node,
           TreeNodePtr tagFieldNode)

{
  TreeNodePtr nodeSign, endNode, indirRefNode, enumItemNode, idNode;
  bool ignore;
  unsigned ord, ordIncr;
  DString label, enumId;

  if (PCerrors.Recovery || !IntCode.IsForm)
    return;
    
  if ((node->Atom == Plus) || (node->Atom == Minus)) {
    nodeSign = node;
    node = node->Right;
  }
  else nodeSign = 0;
  
  if (tagFieldNode->Down->Atom != IdentifierA)
    endNode = tagFieldNode;
  else
    IntCode.FindEndOfReferenceChain(tagFieldNode,endNode,
                                    indirRefNode,ignore);

  switch (node->Atom) {

  case IntConst:
    if (endNode->SynType != STsimpleType) {
      PCerrors.ContextError(CLincompatTF,node->SourceLine,
          node->SourceCol);
      return;
    }
    if (endNode->Down->Atom == Unsigned) {
      if (nodeSign && (nodeSign->Atom == Minus)) {
  PCerrors.ContextError(CLincompatTF,nodeSign->SourceLine,
            nodeSign->SourceCol);
  return;
      }
    }
    else if (endNode->Down->Atom != Int) {
      PCerrors.ContextError(CLincompatTF,node->SourceLine,
          node->SourceCol);
      return;
    }
    break;

  case OctalConst:
  case HexConst:
    if ((endNode->SynType != STsimpleType)
        || ((endNode->Down->Atom != Unsigned)
            && (endNode->Down->Atom != Int))) {
      PCerrors.ContextError(CLincompatTF,node->SourceLine,
          node->SourceCol);
      return;
    }
    break;

  case True:
  case False:
    if ((endNode->SynType != STsimpleType)
        || (endNode->Down->Atom != BooleanA)) {
      PCerrors.ContextError(CLincompatTF,node->SourceLine,
          node->SourceCol);
      return;
    }
    break;

  case CharConst:
    if ((endNode->SynType != STsimpleType)
        || (endNode->Down->Atom != CharA)) {
      PCerrors.ContextError(CLincompatTF,node->SourceLine,
          node->SourceCol);
      return;
    }
    break;
  
  case IdentifierA:
    if (!endNode) return;
    if (endNode->SynType != STenumeration) {
      PCerrors.ContextError(CLincompatTF,node->SourceLine,
          node->SourceCol);
      return;
    }
    
    label = &StringBuffer[node->StringBufferPos];
    enumItemNode = endNode->Down->Right;
    if (enumItemNode->SynType == STbaseVariant)
      enumItemNode = enumItemNode->Right; // STmenu node
    enumItemNode = enumItemNode->Right; // first enum. item node
    ord = 0;
    for (;;) {
      ordIncr = 1;
      if (enumItemNode->Down->Atom == StringConst) {
  idNode = enumItemNode->Down->Right;
  if (idNode == 0)
    ordIncr = 0;
  else if (idNode->Atom == StringConst) {
    idNode = 0;
    ordIncr = 0;
  }
  else if (!(idNode->Flags.Contains(HasSYN)))
    idNode = 0;
      }
      else idNode = enumItemNode->Down;
      
      if (idNode) {
  enumId = &StringBuffer[idNode->StringBufferPos];
  if (label == enumId) break;
      }
      enumItemNode = enumItemNode->Right;
      if (enumItemNode == 0) break;
      ord += ordIncr;
    }
    
    if (enumItemNode == 0) {
      PCerrors.ContextError(TFNotItem,node->SourceLine,
          node->SourceCol);
      return;
    }
    node->CaseLabelOrd = ord;
  }
} // END OF CheckCaseLabelExpr


/*
void ContextCKCLASS::CheckIndexType (TreeNodePtr indexNode,
                                   TreeNodePtr referencedIndexType)

{
  if (PCerrors.Recovery) return;
  if (referencedIndexType->SynType == STsimpleType) {
    referencedIndexType = referencedIndexType->Down;
    switch (referencedIndexType->Atom) {

    case bool:
    case byte:
    case char:
      PCerrors.ContextError(NotSupported,indexNode->SourceLine,
                            indexNode->SourceCol);
      break;

    default:
      PCerrors.ContextError(NotAllowed,indexNode->SourceLine,
                            indexNode->SourceCol);
    }
  }
  else if (referencedIndexType->SynType == STqualident)
    if (referencedIndexType->Right != 0) // qualified subrange
      PCerrors.ContextError(NotAllowed,indexNode->Right->SourceLine,
                            indexNode->Right->SourceCol);
    else if (referencedIndexType->Down->Right != 0) // qualified id
      PCerrors.ContextError(NotAllowed,indexNode->SourceLine,
                            indexNode->SourceCol);
    else  // imported type
    PCerrors.ContextError(NoLocalType,indexNode->SourceLine,
                          indexNode->SourceCol);
  else if (referencedIndexType->SynType == STsubrange)
    if (IntCode.IsForm) {
      CheckDecimalConstExpression(referencedIndexType->Down->Down);
      // 1st expression
      CheckDecimalConstExpression(referencedIndexType->Down->Right->Down);
      // 2nd expr.
    }
  else if (referencedIndexType->SynType != STenumeration)
    PCerrors.ContextError(NotAllowed,indexNode->SourceLine,
                          indexNode->SourceCol);
} // END OF CheckIndexType


void ContextCKCLASS::CheckDecimalConstExpression (TreeNodePtr node)

{
  TreeNodePtr node1;

  if (PCerrors.Recovery) return;
  // expression components:
  node1 = node->Right;
  if (node1 != 0)
    PCerrors.ContextError(NotAllowed,node1->SourceLine,
                          node1->SourceCol);
  node = node->Down;

  // simple expression components:
  if (node->Atomic)
    node = node->Right;
  node1 = node->Right;
  if (node1 != 0)
    PCerrors.ContextError(NotAllowed,node1->SourceLine,
                          node1->SourceCol);
  node = node->Down;

  // term components:
  node1 = node->Right;
  if (node1 != 0)
    PCerrors.ContextError(NotAllowed,node1->SourceLine,
                          node1->SourceCol);

  // factor:
  node = node->Down;
  if (node->Atom != decimal)
    PCerrors.ContextError(NotAllowed,node->SourceLine,
                          node->SourceCol);
} // END OF CheckDecimalConstExpression


void ContextCKCLASS::CheckDecimalConstSimpleExpression (TreeNodePtr node)

{
  TreeNodePtr node1;

  if (PCerrors.Recovery) return;
  // simple expression components:
  if (node->Atomic) {
    PCerrors.ContextError(NotAllowedInForm,node->SourceLine,
                          node->SourceCol);
    node = node->Right;
  }
  node1 = node->Right;
  if (node1 != 0)
    PCerrors.ContextError(NotAllowedInForm,node1->SourceLine,
                          node1->SourceCol);
  node = node->Down;

  // term components:
  node1 = node->Right;
  if (node1 != 0)
    PCerrors.ContextError(NotAllowedInForm,node1->SourceLine,
                          node1->SourceCol);

  // factor:
  node = node->Down;
  if (node->Atom != decimal)
    PCerrors.ContextError(NotAllowedInForm,node->SourceLine,
                          node->SourceCol);
} // END OF CheckDecimalConstSimpleExpression


void ContextCKCLASS::CheckTypeCompatibility (TreeNodePtr formType,
                                           TreeNodePtr baseType)

{
  TreeNodePtr node1;

  if (!formType->Atomic
      && (formType->SynType == STpopup))
    formType = formType->Down;
  if (!baseType->Atomic
      && (baseType->SynType == STpopup))
    baseType = baseType->Down;
  if (PCerrors.Recovery) return;
  if ((formType->SynType == STqualident)
      || (formType->SynType == STsimpleType)
      || (baseType->SynType == STqualident)
      || (baseType->SynType == STsimpleType)) {
    if (!Equivalent(formType,baseType))
      PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                            formType->SourceCol);
    return;
  }

  if (formType->SynType != baseType->SynType) {
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);
    return;
  }

  switch (formType->SynType) {

  case STarray:
    cmpArrayType(formType->Down,baseType->Down);
    break;

  case STbytes:
  case STstring:
    cmpStringBuffer(formType->Down->Down,baseType->Down->Down);
    break;

  case STchain:
    cmpChainType(formType->Down,baseType->Down);
    break;

  case STenumeration:
    cmpEnumeration(formType->Down,baseType->Down);
    break;

  case STnested:
    CheckTypeCompatibility(formType->Down,baseType->Down);
    break;

  case STrecord:
    cmpRecordType(formType->Down,baseType->Down);
    break;

  case STsubrange:
    cmpSubrange(formType->Down,baseType->Down);
    break;

  default:
    ;
  }
} // END OF CheckTypeCompatibility


bool ContextCKCLASS::Equivalent (TreeNodePtr formType,
                                  TreeNodePtr baseType)

{
  TreeNodePtr defDown;
  AtomType fAtom, bAtom;
  TStringBufferPos fId, bId;
  TypeTablePtr entryFound;
  AtomPtr strPtrF, strPtrB;

  if (((formType->SynType != STqualident)
       && (formType->SynType != STsimpleType))
      || ((baseType->SynType != STqualident)
          && (baseType->SynType != STsimpleType)))
    return false;

  defDown = formType->Down;
  for (;;) {
    fAtom = defDown->Atom;
    if (fAtom == identifier) {
      fId = defDown->StringBufferPos;
      IntCode.LookUpType(fId,entryFound);
      if ((entryFound == 0)) goto exit;

      while ((entryFound->baseType != 0))
        entryFound = entryFound->baseType;
      if (entryFound->locallyDefined)
        if (entryFound->def == 0) {
          PCerrors.ContextError(UndefinedType,formType->SourceLine,
                                formType->SourceCol);
          return false;
        }
        else if ((entryFound->def->SynType == STsimpleType)
                 || (entryFound->def->SynType == STqualident))
          defDown = entryFound->def->Down;
        else {
          fId = entryFound->id;
          goto exit;
        }
      else {
        fId = entryFound->id;
        goto exit;
      }
    }
    else goto exit;
  } exit: ;

  defDown = baseType->Down;
  for (;;) {
    bAtom = defDown->Atom;
    if (bAtom == identifier) {
      bId = defDown->StringBufferPos;
      IntCode.LookUpType(bId,entryFound);
      if ((entryFound == 0)) goto exit;

      while ((entryFound->baseType != 0))
        entryFound = entryFound->baseType;
      if (entryFound->locallyDefined)
        if (entryFound->def == 0) {
          PCerrors.ContextError(UndefinedType,baseType->SourceLine,
                                baseType->SourceCol);
          return false;
        }
        else if ((entryFound->def->SynType == STsimpleType)
                 || (entryFound->def->SynType == STqualident))
          defDown = entryFound->def->Down;
        else {
          bId = entryFound->id;
          goto exit;
        }
      else {
        bId = entryFound->id;
        goto exit;
      }
    }
    else goto exit;
  } exit: ;

  if (fAtom != bAtom) return false;
  if (fAtom != identifier) return true;

  strPtrF = Atoms.AtomPtr((address)&Atoms.StringBuffer+fId);
  strPtrB = Atoms.AtomPtr((address)&Atoms.StringBuffer+bId);
  if (Compare(*strPtrF,*strPtrB) == 0) return true;
  else return false;
} // END OF Equivalent


void ContextCKCLASS::cmpStringBuffer (TreeNodePtr formType,
                                    TreeNodePtr baseType)

{
  unsigned if, ib;

  if = formType->StringBufferPos;
  ib = baseType->StringBufferPos;
  while ((Atoms.StringBuffer[if] != '\0')) {
    if ((Atoms.StringBuffer[if] != Atoms.StringBuffer[ib])) {
      PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                            formType->SourceCol);
      return;
    }
    if++;
    ib++;
  }
  if (Atoms.StringBuffer[ib] != '\0')
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);
} // END OF cmpStringBuffer


void ContextCKCLASS::cmpArrayType (TreeNodePtr formType,
                                 TreeNodePtr baseType)

{
  cmpArrayIndices(formType->Down,baseType->Down);
  CheckTypeCompatibility(formType->Right,baseType->Right);
} // END OF cmpArrayType


void ContextCKCLASS::cmpArrayIndices (TreeNodePtr formType,
                                    TreeNodePtr baseType)

{
  do {
    CheckTypeCompatibility(formType,baseType);
    if (succExistenceDifference(formType,baseType))
      return;
  }
  while (!(formType == 0));
} // END OF cmpArrayIndices


bool ContextCKCLASS::existenceDifference (TreeNodePtr formType,
                                           TreeNodePtr baseType,
                                           TreeNodePtr precedingFormType)

{
  if ((formType != 0)
      && (baseType == 0)) {
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);
    return true;
  }
  else if ((baseType != 0)
           && (formType == 0)) {
    PCerrors.ContextError(MissingCont,precedingFormType->SourceLine,
                          precedingFormType->SourceCol);
    return true;
  }
  return false;
} // END OF existenceDifference


bool ContextCKCLASS::succExistenceDifference (TreeNodePtr& formType,
                                               TreeNodePtr& baseType)

{
  TreeNodePtr precedingFormType;

  precedingFormType = formType;
  formType = formType->Right;
  baseType = baseType->Right;
  if ((formType != 0)
      && (baseType == 0)) {
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);
    return true;
  }
  else if ((baseType != 0)
           && (formType == 0)) {
    PCerrors.ContextError(MissingCont,precedingFormType->SourceLine,
                          precedingFormType->SourceCol);
    return true;
  }
  return false;
} // END OF succExistenceDifference


void ContextCKCLASS::cmpChainType (TreeNodePtr formType,
                                 TreeNodePtr baseType)

{
  if (formType->SynType == STlengthSpec)
    formType = formType->Right;
  if (baseType->SynType == STlengthSpec)
    baseType = baseType->Right;
  CheckTypeCompatibility(formType,baseType);
} // END OF cmpChainType


void ContextCKCLASS::skipPureTextItems (TreeNodePtr& item,
                                      TreeNodePtr& id,
                                      TreeNodePtr& precedingItem,
                                      TreeNodePtr& precedingId)

{
  if (item == 0) return;
  for (;;) {
    id = item->Down;
    precedingId = id;
    if (id->Atom == StringConst) {
      id = id->Right;
      if ((id != 0)
          && !id->Atomic) {
        precedingId = id;
        id = id->Right;
      }
      if (id == 0) {
        precedingItem = item;
        item = item->Right;
        if (item == 0) return;
      }
      else return;
    }
    else return;
  } exit: ;
} // END OF skipPureTextItems


void ContextCKCLASS::cmpEnumeration (TreeNodePtr formType,
                                   TreeNodePtr baseType)

{
  TreeNodePtr nodeF, nodeB, precedingFormType, precedingBaseType, precedingNodeF, precedingNodeB;

  precedingFormType = formType;
  skipPureTextItems(formType,nodeF,precedingFormType,
                    precedingNodeF);
  skipPureTextItems(baseType,nodeB,precedingBaseType,
                    precedingNodeB);
  for (;;) {
    if ((formType != 0)
        && (baseType == 0)) {
      PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                            formType->SourceCol);
      return;
    }
    else if ((baseType != 0)
             && (formType == 0)) {
      PCerrors.ContextError(MissingCont,precedingFormType->SourceLine,
                            precedingFormType->SourceCol);
      return;
    }
    if (formType == 0) return;

    if (nodeF->Atom != nodeB->Atom) {
      PCerrors.ContextError(IncompBaseType,nodeF->SourceLine,
                            nodeF->SourceCol);
      return;
    }
    cmpStringBuffer(nodeF,nodeB);
    if (PCerrors.AfterContextError) return;

    precedingFormType = formType;
    formType = formType->Right;
    baseType = baseType->Right;
    skipPureTextItems(formType,nodeF,precedingFormType,
                      precedingNodeF);
    skipPureTextItems(baseType,nodeB,precedingBaseType,
                      precedingNodeB);
  } exit: ;
} // END OF cmpEnumeration


void ContextCKCLASS::cmpRecordType (TreeNodePtr formType,
                                  TreeNodePtr baseType)

{
  TreeNodePtr precedingFormType;

  precedingFormType = formType;
  do {
    while ((formType != 0)
           && (formType->Down->Atomic
               || (formType->Down->Right == 0))) {
      precedingFormType = formType;
      formType = formType->Right;
    }
    while ((baseType != 0)
           && (baseType->Down->Atomic
               || (baseType->Down->Right == 0)))
      baseType = baseType->Right;
    if (existenceDifference(formType,baseType,precedingFormType)
        || (formType == 0))
      return;

    cmpFieldList(formType,baseType);
    if (PCerrors.AfterContextError) return;

    formType = formType->Right;
    baseType = baseType->Right;
  }
  while (!(formType == 0));
} // END OF cmpRecordType


void ContextCKCLASS::cmpFieldList (TreeNodePtr formType,
                                 TreeNodePtr baseType)

{
  if (formType->SynType != baseType->SynType)
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);

  switch (formType->SynType) {

  case STfieldList:
    cmpFieldIdentList(formType->Down->Down,baseType->Down->Down);

    CheckTypeCompatibility(formType->Down->Right,baseType->Down->Right);
    break;

  case STrecordCase:
    cmpRecordCase(formType->Down,baseType->Down);
    break;
  }
} // END OF cmpFieldList


void ContextCKCLASS::cmpFieldIdentList (TreeNodePtr formType,
                                      TreeNodePtr baseType)

{
  TreeNodePtr formFieldId, baseFieldId, precedingFormFieldId;

  do {
    formFieldId = formType->Down;
    precedingFormFieldId = formFieldId;
    while (!formFieldId->Atomic
           || (formFieldId->Atom != identifier)) {
      precedingFormFieldId = formFieldId;
      formFieldId = formFieldId->Right;
    }
    baseFieldId = baseType->Down;
    while (!baseFieldId->Atomic
           || (baseFieldId->Atom != identifier))
      baseFieldId = baseFieldId->Right;

    cmpStringBuffer(formFieldId,baseFieldId); // compare identifiers
    if (PCerrors.AfterContextError) return;

    if (succExistenceDifference(formType,baseType))
      return;
  }
  while (!(formType == 0));
} // END OF cmpFieldIdentList


void ContextCKCLASS::cmpRecordCase (TreeNodePtr formType,
                                  TreeNodePtr baseType)

{
  TreeNodePtr formFieldId, baseFieldId;

  formFieldId = formType->Down->Down;
  while (!formFieldId->Atomic
         || (formFieldId->Atom != identifier))
    formFieldId = formFieldId->Right;
  baseFieldId = baseType->Down->Down;
  while (!baseFieldId->Atomic
         || (baseFieldId->Atom != identifier))
    baseFieldId = baseFieldId->Right;
  
  //  cmpStringBuffer(formFieldId,baseFieldId); (* tag field identifiers *)
  //  IF AfterContextError THEN RETURN END;
 

  CheckTypeCompatibility(formType->Down->Right,baseType->Down->Right);
  // tag field types
  //  IF AfterContextError THEN RETURN END;

  formType = formType->Right; // first variant
  baseType = baseType->Right; // first variant
  do {
    if (formType->SynType != baseType->SynType) {
      PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                            formType->SourceCol);
      return;
    }
    if (formType->SynType == STvariant)
      cmpVariant(formType->Down,baseType->Down);
    else cmpRecordType(formType->Down,baseType->Down);
    if (PCerrors.AfterContextError) return;

    if (succExistenceDifference(formType,baseType))
      return;
  }
  while (!(formType == 0));
} // END OF cmpRecordCase


void ContextCKCLASS::cmpVariant (TreeNodePtr formType,
                               TreeNodePtr baseType)

{
  if (formType->Atomic != baseType->Atomic) {
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);
    return;
  }
  if (!formType->Atomic) {
    cmpCaseLabelList(formType->Down,baseType->Down);
    //    IF AfterContextError THEN RETURN END;
    cmpRecordType(formType->Right->Down,baseType->Right->Down);
  }
} // END OF cmpVariant


void ContextCKCLASS::cmpCaseLabelList (TreeNodePtr formType,
                                     TreeNodePtr baseType)

{
  do {
    cmpCaseLabel(formType->Down,baseType->Down);
    if (PCerrors.AfterContextError) return;
    if (succExistenceDifference(formType,baseType))
      return;
  }
  while (!(formType == 0));
} // END OF cmpCaseLabelList


void ContextCKCLASS::cmpCaseLabel (TreeNodePtr formType,
                                 TreeNodePtr baseType)

{
  cmpExpression(formType->Down,baseType->Down);
  //  IF AfterContextError THEN RETURN END;

  if (succExistenceDifference(formType,baseType)) return;

  if (formType != 0)
    cmpExpression(formType->Down,baseType->Down);
} // END OF cmpCaseLabel


void ContextCKCLASS::cmpSubrange (TreeNodePtr formType,
                                TreeNodePtr baseType)

{
  cmpExpression(formType->Down,baseType->Down);
  //  IF AfterContextError THEN RETURN END;

  formType = formType->Right;
  baseType = baseType->Right;

  cmpExpression(formType->Down,baseType->Down);
} // END OF cmpSubrange


void ContextCKCLASS::cmpExpression (TreeNodePtr formType,
                                  TreeNodePtr baseType)

{
  TreeNodePtr precedingFormType;

  cmpSimpleExpression(formType->Down,baseType->Down);
  //  IF AfterContextError THEN RETURN END;

  if (succExistenceDifference(formType,baseType)) return;

  if (formType != 0)
    cmpSimpleExpression(formType->Down,baseType->Down);
} // END OF cmpExpression


void ContextCKCLASS::cmpSimpleExpression (TreeNodePtr formType,
                                        TreeNodePtr baseType)

{
  // sign:
  if ((formType->Atomic
       && (formType->Atom == minus)
       && (!baseType->Atomic
           || (baseType->Atom != minus)))
      || (baseType->Atomic
          && (baseType->Atom == minus)
          && (!formType->Atomic
              || (formType->Atom != minus)))) {
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);
    return;
  }

  if (formType->Atomic)
    formType = formType->Right; // skip sign
  if (baseType->Atomic)
    baseType = baseType->Right; // skip sign

  // terms:
  for (;;) {
    cmpTerm(formType->Down,baseType->Down);
    if (PCerrors.AfterContextError) return;

    if (succExistenceDifference(formType,baseType))
      return;
    if (formType != 0) {
      if (formType->Atom != baseType->Atom) {
        PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                              formType->SourceCol);
        return;
      }
      formType = formType->Right;
      baseType = baseType->Right;
    }
    else goto exit;
  } exit: ;
} // END OF cmpSimpleExpression


void ContextCKCLASS::cmpTerm (TreeNodePtr formType,
                            TreeNodePtr baseType)

{
  for (;;) {
    cmpFactor(formType->Down,baseType->Down);
    if (PCerrors.AfterContextError) return;

    if (succExistenceDifference(formType,baseType))
      return;
    if (formType != 0) {
      if (formType->Atom != baseType->Atom) {
        PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                              formType->SourceCol);
        return;
      }
      formType = formType->Right;
      baseType = baseType->Right;
    }
    else goto exit;
  } exit: ;
} // END OF cmpTerm


void ContextCKCLASS::cmpFactor (TreeNodePtr formType,
                              TreeNodePtr baseType)

{
  if ((formType->Atomic != baseType->Atomic)
      || (formType->Atomic
          && (formType->Atom != baseType->Atom))
      || (!formType->Atomic
          && (formType->SynType != baseType->SynType))) {
    PCerrors.ContextError(IncompBaseType,formType->SourceLine,
                          formType->SourceCol);
    return;
  }

  if (formType->Atomic)
    switch (formType->Atom) {

    case octal:
    case octalChar:
    case decimal:
    case hexadecimal:
    case float:
    case StringConst:
      cmpStringBuffer(formType,baseType);
      break;

    case not:
      formType = formType->Right;
      baseType = baseType->Right;
      cmpFactor(formType,baseType);
      break;

    default:
      ;
    }
  else
    switch (formType->SynType) {

    case STexpression:
      cmpExpression(formType->Down,baseType->Down);
      break;

    case STqualident:
      cmpStringBuffer(formType->Down,baseType->Down);
      break;
    } // CASE
  // IF
} // END OF cmpFactor
*/



/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/

void ContextCKCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  IntCode.INIT();
  Atoms_INIT();
  PCerrors.INIT();
}
