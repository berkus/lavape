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


#include "FioCoder.h"

#include "ProCoder.h"
#include "TypCoder.h"
#include "CdpCoder.h"
#include "ElemCode.h"
#include "Convert.h"

#pragma hdrstop


FioCoderCLASS FioCoder;
static bool __INITstarted=false;

/* ****************************************************************/
/* In the following "fio" stands for "forms input/output".        */
/* The fio-procedures generate calls to the elementary            */
/* ASN1 Put/Get procedures                                        */
/* ****************************************************************/

/************************************************************************/


void FioCoderCLASS::FIOProcGeneration ()

{
  unsigned savedIndent;
  
  if (IntCode.DclsAreCoded) {
    if (IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasFIO)
        && IntCode.CurrentTypeEntry->attributes.Contains(HasFIO)) {
      writeLn();
      switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
      case STclass:
      case STstruct:
  code("friend ");
  break;
      default:
  if (IntCode.GlobalProperties.Flags.Contains(IncludeFile))
    code("extern ");
  else goto impl;
      }
      code("void FIO");
      codeVariable(IntCode.CurrentTypeEntry->id);
      code(" (");
      indentHere(savedIndent);
      codeNl("PutGetFlag pgf, ASN1* cid, address varAddr,");
      codeNl("bool baseFIO=false);");
      restoreIndent(savedIndent);
    }

    switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
    case STclass:
    case STstruct:
      declareFIOsOfVariants(IntCode.CurrentTypeEntry);
    }
    return;
  }


impl:
  if (IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasFIO)
      && IntCode.CurrentTypeEntry->attributes.Contains(HasFIO)) {
    writeLn();
    writeLn();
    if (!IntCode.GlobalProperties.Flags.Contains(IncludeFile))
      switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
      case STstruct:
      case STclass:
        break;
      default:
        code("static ");
      }
    code("void FIO");
    codeVariable(IntCode.CurrentTypeEntry->id);
    code(" (");
    indentHere(savedIndent);
    codeNl("PutGetFlag pgf, ASN1* cid, address varAddr,");
    switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
    case STstruct:
    case STclass:
      codeNl("bool)");
      break;
    default:
      if (!IntCode.GlobalProperties.Flags.Contains(IncludeFile))
        codeNl("bool =false)");
      else
        codeNl("bool)");
    }
    restoreIndent(savedIndent);

    designator = "vp";
   
    writeLn();
    codeNl("{");
    incIndent();
    codeVariable(IntCode.CurrentTypeEntry->id);
    code(" *vp = (");
    codeVariable(IntCode.CurrentTypeEntry->id);
    codeNl("*)varAddr;");
    if (IntCode.CurrentTypeEntry->indexLevel) {
      code("unsigned ");
      for (unsigned k=1;
           k<=IntCode.CurrentTypeEntry->indexLevel;
           k++) {
        code("i");
        codeCard(k);
        if (k < IntCode.CurrentTypeEntry->indexLevel)
          code(", ");
        else
          codeNl(";");
      }
    }
    codeNl("if (cid->Skip()) return;");
    writeLn();
  
    indexLevel = 0;
//    recordIndexLevel = 0;
    
    codeNl("if (pgf == GET) {");
    incIndent();
    fipDataType(IntCode.CurrentTypeEntry->typeSpecifier,
      IntCode.CurrentTypeEntry->declarator);
    decIndent();
    codeNl("}");
  
    codeNl("else { // put direction");
    indexLevel = 0;
    incIndent();
    fopDataType(IntCode.CurrentTypeEntry->typeSpecifier,
                IntCode.CurrentTypeEntry->declarator);
    decIndent();
    codeNl("} // if (pgf == GET)");
    decIndent();
    code("} // END OF FIO");
    codeVariable(IntCode.CurrentTypeEntry->id);
    writeLn();
  }
}


void FioCoderCLASS::declareFIOsOfVariants(TypeTablePtr currentTypeEntry)

{
  CHETypeTablePtr *variantType;
  unsigned savedIndent;
  
  for (variantType = (CHETypeTablePtr*)currentTypeEntry->variantTypes.first;
       variantType;
       variantType = (CHETypeTablePtr*)variantType->successor) {
    currentTypeEntry = variantType->data;
    
    if (currentTypeEntry->procGenAttributes.Contains(HasFIO)
        && currentTypeEntry->attributes.Contains(HasFIO)) {
      writeLn();
      code("friend void FIO");
      codeVariable(currentTypeEntry->id);
      code(" (");
      indentHere(savedIndent);
      codeNl("PutGetFlag pgf, ASN1* cid, address varAddr,");
      codeNl("bool baseFIO=false);");
      restoreIndent(savedIndent);
    }
    declareFIOsOfVariants(currentTypeEntry);
  }
}


bool FioCoderCLASS::IsMenu (TreeNodePtr node,
                               bool& menuFirst,
                               AtomType& menuType)

{
  bool rc;
  TreeNodePtr enumId, prefix, simpleAnno;


  // check if MENU/MENULINE annotation specified (==> RETURN true)
  rc = false;
  menuFirst = false;
  menuType = NoAtom;
  if ((node->Annotation != 0)
      && (node->Annotation->Right != 0)) {
    simpleAnno = node->Annotation->Right->Down;
    // first menu annotation
    do {
      if (simpleAnno->Atomic)
        switch (simpleAnno->Atom) {

        case Menu:
          rc = true;
          break;

        case Menufirst:
          menuFirst = true;
          rc = true;
          break;
        }
      else if (simpleAnno->SynType == STbuttonmenu) {
        menuType = Buttonmenu;
        rc = true;
      }
      else if (simpleAnno->SynType == SToptionmenu) {
        menuType = Optionmenu;
        rc = true;
      }
      else if (simpleAnno->SynType == STpopupmenu) {
        menuType = Popupmenu;
        rc = true;
      }
      simpleAnno = simpleAnno->Right;
    }
    while (simpleAnno);
  }
  if (menuType != NoAtom)
    menuFirst = false;
  if (rc)
    return true;

  // check if explanation text present (==> RETURN true)
  enumId = node->Right;
  do {
    prefix = enumId->Down;
    if (prefix->Atom == StringConst) {
      prefix = prefix->Right;
      if (prefix
          && !prefix->Atomic)
        prefix = prefix->Right;
      if (!prefix)
        return true;
      else if (prefix->Atom == StringConst)
        return true;
    }
    prefix = prefix->Right; // skip enum. element id
    if (prefix)
      return true;

    enumId = enumId->Right;
  }
  while (enumId);

  return false;
}


void FioCoderCLASS::fipDataType (TreeNodePtr typeSpecifier,
                                 TreeNodePtr declarator)

{
  if (!typeSpecifier->Flags.Contains(HasFIO)) return;
  
  if (declarator
      && ( (declarator->Atomic && declarator->Atom != IdentifierA)
           || ( !declarator->Atomic
                && declarator->SynType != STfieldId
                && declarator->SynType != STpopup
              )
         )
     )
    return;
  
  if (declarator
      && ((declarator->Atomic && declarator->Atom == IdentifierA)
          || (!declarator->Atomic && declarator->SynType == STfieldId))
      && declarator->Right
      && !declarator->Right->Atomic
      && declarator->Right->SynType == STlengthSpec) {
    fipArrayType(typeSpecifier,declarator->Right);
    return;
  }
  
  switch (typeSpecifier->SynType) {

  case STclass:
  case STstruct:
    fipStructType(typeSpecifier->Down,typeSpecifier);
    break;

  case STenumeration:
    fipEnumType(typeSpecifier->Down);
    break;

  case STsimpleType:
    fipSimpleType(typeSpecifier->Down);
    break;

  case STchain:
  case STchain0:
  case STchainAny:
  case STchainAny0:
  case STnested:
  case STnested0:
  case STnestedAny:
  case STnestedAny0:
    fioChainNestedType(typeSpecifier);
    break;

  case STdate:
    fipDateType();
    break;

  case STtime:
    fipTimeType();
    break;

  case STyear:
    fipYearType();
    break;

  case STsignature:
    fioSignatureType(typeSpecifier->Down);
    break;
    
  default:
    flushAddressStack();
    code(designator);
    if (designator.l == 2) code("->");
    else code(".");
    codeNl("FIO(GET,cid);");
  }
}


void FioCoderCLASS::fopDataType (TreeNodePtr typeSpecifier,
                                 TreeNodePtr declarator)

{
  DString outFlag;
  
  if (!typeSpecifier->Flags.Contains(HasFIO)) return;
  
  if (declarator
      && ( (declarator->Atomic && declarator->Atom != IdentifierA)
           || ( !declarator->Atomic
                && declarator->SynType != STfieldId
                && declarator->SynType != STpopup
              )
         )
     )
    return;
  
  if (declarator
      && ((declarator->Atomic && declarator->Atom == IdentifierA)
          || (!declarator->Atomic && declarator->SynType == STfieldId))
      && declarator->Right
      && !declarator->Right->Atomic
      && declarator->Right->SynType == STlengthSpec) {
    fopArrayType(typeSpecifier,declarator->Right);
    return;
  }
  
  switch (typeSpecifier->SynType) {

  case STclass:
  case STstruct:
    fopStructType(typeSpecifier->Down,typeSpecifier);
    break;

  case STenumeration:
    fopEnumType(typeSpecifier);
    break;

  case STsimpleType:
    fopSimpleType(typeSpecifier);
    break;

  case STchain:
  case STchain0:
  case STchainAny:
  case STchainAny0:
  case STnested:
  case STnested0:
  case STnestedAny:
  case STnestedAny0:
    fioChainNestedType(typeSpecifier);
    break;

  case STdate:
    fopDateType(typeSpecifier);
    break;

  case STtime:
    fopTimeType(typeSpecifier);
    break;

  case STyear:
    fopYearType(typeSpecifier);
    break;

  case STsignature:
    fioSignatureType(typeSpecifier->Down);
    break;
    
  default:
    flushAddressStack();
    code(designator);
    if (designator.l == 2) code("->");
    else code(".");
    code("FIO(");
    if ((typeSpecifier->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
      if (typeSpecifier->Flags.Contains(OUTPUTIC))
        outFlag = "PUT";
      else
        outFlag = "DontPUT";
    else
      outFlag = "pgf";
    code(outFlag);
    codeNl(",cid);");
  }
}


void FioCoderCLASS::fioSignatureType (TreeNodePtr node)

{
  flushAddressStack();
  code("FIOpp.Signature(pgf,cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  code(",\"");
  codeAtom(node);
  codeNl("\");");
}


void FioCoderCLASS::fipDateType ()

{
  flushAddressStack();
  code("FIODATE_(GET,cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  codeNl(");");
}



void FioCoderCLASS::fipTimeType ()

{
  flushAddressStack();
  code("FIOTIME_(GET,cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  codeNl(");");
}

void FioCoderCLASS::fipYearType ()

{
  flushAddressStack();
  code("FIOYEAR_(GET,cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  codeNl(");");
}


void FioCoderCLASS::fipSimpleType (TreeNodePtr node)

{
  flushAddressStack();
  if (node->Atom == IdentifierA) {
    code("FIO");
    codeAtom(node);
    code("(GET,cid,(address)");
    if (designator.l > 2) code("&");
  }
  else {
    code("FIOpp.EOA");
    codeAtom(node);
    code("(GET,cid,");
    if (designator.l == 2) code("*");
  }
  code(designator);
  codeNl(");");
}


void FioCoderCLASS::fopDateType (TreeNodePtr node)

{
  DString outFlag;
    
  flushAddressStack();
  if ((node->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
    if (node->Flags.Contains(OUTPUTIC))
      outFlag = "PUT";
    else {
      pureINPUT = true;
      outFlag = "DontPUT";
    }
  else
    outFlag = "pgf";
    
  code("FIODATE_(");
  code(outFlag);
  code(",cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  codeNl(");");
}


void FioCoderCLASS::fopTimeType (TreeNodePtr node)

{
  DString outFlag;
    
  flushAddressStack();
  if ((node->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
    if (node->Flags.Contains(OUTPUTIC))
      outFlag = "PUT";
    else {
      pureINPUT = true;
      outFlag = "DontPUT";
    }
  else
    outFlag = "pgf";
    
  code("FIOTIME_(");
  code(outFlag);
  code(",cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  codeNl(");");
}


void FioCoderCLASS::fopYearType (TreeNodePtr node)

{
  DString outFlag;
    
  flushAddressStack();
  if ((node->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
    if (node->Flags.Contains(OUTPUTIC))
      outFlag = "PUT";
    else {
      pureINPUT = true;
      outFlag = "DontPUT";
    }
  else
    outFlag = "pgf";
    
  code("FIOYEAR_(");
  code(outFlag);
  code(",cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  codeNl(");");
}


void FioCoderCLASS::fopSimpleType (TreeNodePtr node)

{
  DString outFlag;
  TreeNodePtr nodeDown=node->Down;
    
  flushAddressStack();
  if ((node->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
    if (node->Flags.Contains(OUTPUTIC))
      outFlag = "PUT";
    else {
      pureINPUT = true;
      outFlag = "DontPUT";
    }
  else
    outFlag = "pgf";
    
  if (nodeDown->Atom == IdentifierA) {
    code("FIO");
    codeAtom(nodeDown);
    code("(");
    code(outFlag);
    code(",cid,(address)");
    if (designator.l > 2) code("&");
  }
  else {
    code("FIOpp.EOA");
    codeAtom(nodeDown);
    code("(");
    code(outFlag);
    code(",cid,");
    if (designator.l == 2) code("*");
  }
  code(designator);
  codeNl(");");
}


void FioCoderCLASS::fipEnumType (TreeNodePtr node)

{
  bool menuFirst;
  AtomType menuType;
  TreeNodePtr menuNode;

  down();
  menuNode = node->Right;
  if (menuNode->SynType == STbaseVariant)
    menuNode = menuNode->Right;
  if (IsMenu(menuNode,menuFirst,menuType)
      && menuFirst)
    right(1);
  flushAddressStack();
  codeNl("FIOpp.GetEOA(cid);");
  if (designator.l == 2) code("*");
  code(designator);
  code(" = (");
  codeAtom(node);
  codeNl(")cid->FGETunsigned();");
  up();
}


void FioCoderCLASS::fopEnumType (TreeNodePtr node)

{
  bool menuFirst;
  AtomType menuType;
  TreeNodePtr menuNode;

  down();
  menuNode = node->Down->Right;
  if (menuNode->SynType == STbaseVariant)
    menuNode = menuNode->Right;
  if (IsMenu(menuNode,menuFirst,menuType)
      && menuFirst)
    right(1);
  flushAddressStack();
  code("FIOpp.EOAenum(");
  if ((node->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
    if (node->Flags.Contains(OUTPUTIC))
      code("PUT,cid,(unsigned)");
    else {
      pureINPUT = true;
      code("DontPUT,cid,(unsigned)");
    }
  else
    code("pgf,cid,(unsigned)");
  if (designator.l == 2) code("*");
  code(designator);
  code(",");
  if (designator.l > 2) {
    code("(address)&");
    code(designator);
  }
  else
    code("varAddr");
  codeNl(");");
  up();
}


void FioCoderCLASS::fipArrayType (TreeNodePtr typeSpecifier,
                                  TreeNodePtr subscripts)

{
  TreeNodePtr typeEndNode, ignore1;
  bool ignore2;
  unsigned savedDesLength;
  DString str(6);

  IntCode.FindEndOfReferenceChain(typeSpecifier,typeEndNode,ignore1,
                                  ignore2);

  if ((subscripts->Right == 0)
      && (typeEndNode->SynType == STsimpleType))
    if (typeEndNode->Down->Atom == CharA) {
      flushAddressStack();
      code("FIOpp.EOAstring(pgf,cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }
    else if (typeEndNode->Down->Atom == ByteA) {
      flushAddressStack();
      code("FIOpp.EOAbytes(pgf,cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }

  down();
  flushAddressStack();
  savedDesLength = designator.l;
  indexLevel++;
//  arrayIndexLevel++;
  code("for (i");
  codeCard(indexLevel);
//  codeCard(arrayIndexLevel);
  code("=0; !FIOpp.IsUpStep(cid,i");
  codeCard(indexLevel);
//  codeCard(arrayIndexLevel);
  codeNl(");) {");
  incIndent();

  if (designator.l == 2) designator = "(*vp)";
  designator += "[i";
  Convert.CardToString(indexLevel,str);
//  Convert.CardToString(arrayIndexLevel,str);
  designator += str;
  designator += "]";

  if (subscripts->Right)
    fipArrayType(typeSpecifier,subscripts->Right);
  else
    fipDataType(typeSpecifier,0);
  decIndent();
  codeNl("} // for");
  restoreDesignator(savedDesLength);
//  arrayIndexLevel--;
}


void FioCoderCLASS::fopArrayType (TreeNodePtr typeSpecifier,
                                  TreeNodePtr subscripts)

{
  TreeNodePtr typeEndNode, ignore1;
  bool ignore2;
  unsigned savedDesLength;
  DString str(6), outFlag(7);

  IntCode.FindEndOfReferenceChain(typeSpecifier,typeEndNode,ignore1,
                                  ignore2);
  if ((subscripts->Right == 0)
      && (typeEndNode->SynType == STsimpleType)) {
    if (typeSpecifier->Flags * SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1) != SET())
      if (typeSpecifier->Flags.Contains(OUTPUTIC))
        outFlag = "PUT";
      else
        outFlag = "DontPUT";
    else
      outFlag = "pgf";
 
    if (typeEndNode->Down->Atom == CharA) {
      flushAddressStack();
      code("FIOpp.EOAstring(");
      code(outFlag);
      code(",cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }
    else if (typeEndNode->Down->Atom == ByteA) {
      flushAddressStack();
      code("FIOpp.EOAbytes(");
      code(outFlag);
      code("cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }
  }

  down();
  savedDesLength = designator.l;
  indexLevel++;
//  arrayIndexLevel++;
  flushAddressStack();
  code("for (i");
  codeCard(indexLevel);
//  codeCard(arrayIndexLevel);
  code("=0; i");
  codeCard(indexLevel);
//  codeCard(arrayIndexLevel);
  code("<");
  codeAtom(subscripts->Down);
  code("; i");
  codeCard(indexLevel);
//  codeCard(arrayIndexLevel);
  codeNl("++) {");
  incIndent();
  
  if (designator.l == 2) designator = "(*vp)";
  designator += "[i";
  Convert.CardToString(indexLevel,str);
//  Convert.CardToString(arrayIndexLevel,str);
  designator += str;
  designator += "]";

  if (subscripts->Right)
    fopArrayType(typeSpecifier,subscripts->Right);
  else
    fopDataType(typeSpecifier,0);
  code("if (i");
  codeCard(indexLevel);
//  codeCard(arrayIndexLevel);
  code("+1 < ");
  codeAtom(subscripts->Down);
  codeNl(") FIOpp.RIGHT(1);");
  decIndent();
  codeNl("} // for");
  restoreDesignator(savedDesLength);
//  arrayIndexLevel--;
  up();
}


void FioCoderCLASS::fioChainNestedType (TreeNodePtr node)

{
  flushAddressStack();
  code(designator);
  if (designator.l == 2) code("->");
  else code(".");
  code("FIO(pgf,cid");
  
  switch (node->SynType) {
  case STchainAny:
  case STchainAny0:
    code(",FIO");
    CdpCoder.TypeName(node->Down);
    code(",NewCHE");
    CdpCoder.TypeName(node->Down);
    break;
  case STnestedAny:
  case STnestedAny0:
    code(",FIO");
    CdpCoder.TypeName(node->Down);
    code(",NewNST");
    CdpCoder.TypeName(node->Down);
    break;
  case STchain:
  case STchain0:
    code(",FIO");
    CdpCoder.TypeName(node->Down);
    break;
  case STnested:
  case STnested0:
    if (node->Down->SynType == STsimpleType) {
      code(",FIO");
      CdpCoder.TypeName(node->Down);
    }
    break;
  default: ;
  }
  codeNl(");");
}


void FioCoderCLASS::fipStructType (TreeNodePtr node,
           TreeNodePtr structNode)

{
  down();
    
  if (node->Atomic) {
    node = node->Right;
    if (!IntCode.ScopeNestingLevel) {  
      if (node->SynType == STbaseVariant) node = node->Right;
      if (node->SynType == STbaseList) {
  fioBaseList(node->Down);
  node = node->Right;
      }
    }
  }
  
  if (node) {
    IntCode.ScopeNestingLevel++;
    fipMemberList(node->Down,structNode);
    IntCode.ScopeNestingLevel--;
  }
}


void FioCoderCLASS::fioBaseList (TreeNodePtr node)

{
  TreeNodePtr nodeBase, className;
  
  flushAddressStack();
  for (; node; node = node->Right) {
    nodeBase = node->Down;
    if (nodeBase->Atomic)
      nodeBase = nodeBase->Right;
    if (nodeBase->Atomic)
      nodeBase = nodeBase->Right;
    code("FIO");
    for (className = nodeBase->Down;
       className->Right;
       className = className->Right);
    codeAtom(className);
    code("(pgf,cid,(address)(");
    TypCoder.CompleteClassName(nodeBase->Down);
    codeNl("*)vp,true);");
  }
}


void FioCoderCLASS::fipMemberList (TreeNodePtr node,
           TreeNodePtr structNode)

{
  unsigned iField, savedRIL;

  if (node->Atomic && (node->Atom == NoAtom)) return;
  
  iField = 0;
  if (structNode->Flags.Contains(ManyFields)) {
    indexLevel++;
//    recordIndexLevel++;
    flushAddressStack();
    code("i");
    codeCard(indexLevel);
//    codeCard(recordIndexLevel);
    codeNl(" = 1;");
    code("while (!FIOpp.IsUpStep(cid,i");
    codeCard(indexLevel);
//    codeCard(recordIndexLevel);
    codeNl(")) { // field list");
    down();
    incIndent();
    if (!structNode->Flags.Contains(NoPrefix)) right(1);

    flushAddressStack();
    code("switch (i");
    codeCard(indexLevel);
//    codeCard(recordIndexLevel);
    savedRIL = indexLevel;
//    savedRIL = recordIndexLevel;
    codeNl(") {");

    for (; node; node = node->Right) {
      for (; node && node->Atomic; node = node->Right) ;
  // skip access specs
      if (node->Flags.Contains(HasSYN))
        fipMemberDeclaration(node,structNode,iField);
    }

    flushAddressStack();
    code("} // switch (i");
    codeCard(savedRIL);
    codeNl(")");
    if (!structNode->Flags.Contains(NoPrefix)) up();
    decIndent();
    code("} // while (... i");
    codeCard(savedRIL);
    codeNl(")");
  }
  else { // only one field
    for (; node; node = node->Right) {
      for (; node && node->Atomic; node = node->Right) ;
  // skip access specs
      if (node->Flags.Contains(HasFIO)
    && ((node->SynType == STrecordCase)
        || !node->Down->Down->Atomic)) {
  fipMemberDeclaration(node,structNode,iField);
  break;
      }
      else iField++;
    }
    up();
  }
}


void FioCoderCLASS::fipMemberDeclaration (TreeNodePtr node,
              TreeNodePtr structNode,
              unsigned& iField)

{
  if (!node->Flags.Contains(HasSYN)) return;
  
  switch (node->SynType) {
  case STrecordCase:
    fipRecordCase(node->Down,structNode,iField);
    break;

  default:
    node = node->Down;
    ProcCoder.DeclSpecifiers(node->Down);
    if (IntCode.CurrentTypeSpecifier) {
      node = node->Right;
      for (; node; node = node->Right) {
  fipMemberDeclarator(node->Down->Down,structNode,iField);
  if (structNode->Flags.Contains(ManyFields))
    decIndent();
      }
    }
    else iField++;
  }
}


void FioCoderCLASS::fipMemberDeclarator (TreeNodePtr declarator,
                                         TreeNodePtr structNode,
                                         unsigned& iField)

{
  TreeNodePtr prefixOrIdNode;
  unsigned savedDesLength, prefixCount;

  if (declarator->Atomic) return;

  switch (declarator->SynType) {
  case STfieldId:
    iField++;
    prefixOrIdNode = declarator->Down;
    skipPrefixStrings(prefixOrIdNode,prefixCount);
    
    if (structNode->Flags.Contains(ManyFields)) {
      code("case ");
      codeCard(iField);
      codeNl(":");
      incIndent();
    }
    else if (iField > 1) {
      right(iField-1);
      down();
    }
    else
      down();

    if (structNode->Flags.Contains(ManyFields)
  && !structNode->Flags.Contains(NoPrefix)) {
      if (prefixCount > 1) right(prefixCount-1);
    }
    else if (prefixCount > 0)
      right(prefixCount);

    savedDesLength = designator.l;
    appendIdToDesignator(prefixOrIdNode->StringBufferPos);
    if (IntCode.PopupNode) down();
    if (declarator->Right) 
      fipArrayType(IntCode.CurrentTypeSpecifier,declarator->Right);
    else
      fipDataType(IntCode.CurrentTypeSpecifier,declarator);
    if (IntCode.PopupNode) up();
    restoreDesignator(savedDesLength);
    
    if (structNode->Flags.Contains(ManyFields)) {
      if (structNode->Flags.Contains(NoPrefix)) up();
      flushAddressStack();
      codeNl("break;");
    }
    else
      up();
    break;

  case STpopup:
    IntCode.PopupNode = declarator;
    fipMemberDeclarator(declarator->Down,structNode,iField);
    IntCode.PopupNode = 0;
    break;
  }
}


void FioCoderCLASS::skipPrefixStrings (TreeNodePtr& nodeFieldId,
                                       unsigned& prefixCount)

{
  prefixCount = 0;
  do {
    if (nodeFieldId->Atomic)
      if (nodeFieldId->Atom == StringConst)
        prefixCount++;
      else if (nodeFieldId->Flags.Contains(HasSYN) // identifier
        ) {
        if (prefixCount == 0)
          prefixCount = 1;
        return;
      }
      else
        return;
    nodeFieldId = nodeFieldId->Right;
  }
  while (nodeFieldId);
}


void FioCoderCLASS::fipRecordCase (TreeNodePtr node,
                                   TreeNodePtr structNode,
                                   unsigned& iField)

{
  unsigned savedDesLength, savedRIL, prefixCount;
  TreeNodePtr nodeFieldId, tagFieldType;
  bool noTagField;
  DString savedDes;

  iField++;

  if (structNode->Flags.Contains(ManyFields)) {
    flushAddressStack();
    code("case ");
    codeCard(iField);
    codeNl(":");
    incIndent();
  }
  else if (iField > 1) {
    right(iField-1);
    down();
  }
  else
    down();

  indexLevel++;
//  recordIndexLevel++;
  flushAddressStack();
  code("for (i");
  codeCard(indexLevel);
//  codeCard(recordIndexLevel);
  code("=1; !FIOpp.IsUpStep(cid,i");
  codeCard(indexLevel);
//  codeCard(recordIndexLevel);
  codeNl(");) {");
  incIndent();
  code("switch (i");
  codeCard(indexLevel);
//  codeCard(recordIndexLevel);
  savedRIL = indexLevel;
//  savedRIL = recordIndexLevel;
  codeNl(") {");
  codeNl("case 1: // tag field");
  incIndent();
  down(); // --> first tag field prefix
  tagFieldType = node->Down;
  nodeFieldId = tagFieldType->Right->Down;
  if (tagFieldType->Flags.Contains(InvisibleIC))
    noTagField = true;
  else
    noTagField = false;
  skipPrefixStrings(nodeFieldId,prefixCount);
  if (noTagField)
    prefixCount = 0;
  if (prefixCount > 0)
    right(prefixCount);
  savedDesLength = designator.l;
  appendIdToDesignator(nodeFieldId->StringBufferPos);
  fipDataType(tagFieldType,nodeFieldId);
  restoreDesignator(savedDesLength);
  up(); // --> tagfield super node
  flushAddressStack();
  codeNl("break;");
  decIndent();

  codeNl("case 2: // current variant");
  incIndent();
  down();
  flushAddressStack();
  code("switch (");
  appendIdToDesignator(nodeFieldId->StringBufferPos);
  code(designator);
  savedDes = designator;
  restoreDesignator(savedDesLength);
  codeNl(") {");

  node = node->Right;
  while (node && (node->SynType == STvariant)) {
    fipVariant(node->Down);
    node = node->Right;
  }

  flushAddressStack();
  if (node) {
    if (node->Flags.Contains(HasFIO)) {
      codeNl("default:");
      incIndent();
      down();
      fipMemberList(node->Down,node);
      flushAddressStack();
      decIndent();
    }
  }
  else
    codeNl("default: CDPpp.TagFieldError(cid);");
  code("} // switch (");
  code(savedDes);
  codeNl(")");
  up();

  flushAddressStack();
  decIndent();
  code("} // switch (i");
  codeCard(savedRIL);
  codeNl(")");
  decIndent();
  code("} // for (i");
  codeCard(savedRIL);
  codeNl(")");

  if (structNode->Flags.Contains(ManyFields)) {
    codeNl("break;");
    decIndent();
  }
}


void FioCoderCLASS::fipVariant (TreeNodePtr node)

{
  if (node->Atomic) return; // empty variant
  
  CdpCoder.CaseLabelList(node->Down);
  incIndent();
  if (node->Right->Flags.Contains(HasFIO)) {
    down();
    fipMemberList(node->Right->Down,node->Right);
    flushAddressStack();
  }
  codeNl("break;");
  decIndent();
}


void FioCoderCLASS::fopStructType (TreeNodePtr node,
           TreeNodePtr structNode)

{
  down();
  
  if (node->Atomic) {
    node = node->Right;
    if (!IntCode.ScopeNestingLevel) {  
      if (node->SynType == STbaseVariant) node = node->Right;
      if (node->SynType == STbaseList) {
  fioBaseList(node->Down);
  node = node->Right;
      }
    }
  }
  
  if (node) {
    IntCode.ScopeNestingLevel++;
    fopMemberList(node->Down,structNode);
    IntCode.ScopeNestingLevel--;
  }

  up();
}


void FioCoderCLASS::fopMemberList (TreeNodePtr node,
           TreeNodePtr structNode)

{
  if (node->Atomic && (node->Atom == NoAtom)) return;
  
  if ((structNode->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
    if (structNode->Flags.Contains(OUTPUTIC)) {
      flushAddressStack();
      codeNl("FIOpp.PushFlag(pgf,PUT);");
    }
    else {
      flushAddressStack();
      code("FIOpp.PushFlag(pgf,DontPUT);");
    }
    
  do {
    for (; node && node->Atomic; node = node->Right) ;
      // skip access specs
    if (node->Flags.Contains(HasSYN)) {
      fopMemberDeclaration(node);
      node = node->Right;
      if (node) right(1);
    }
    else
      node = node->Right;
  } while (node);
  
  if ((structNode->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET()) {
    flushAddressStack();
    codeNl("FIOpp.PopFlag(pgf);");
  }
}


void FioCoderCLASS::fopMemberDeclaration (TreeNodePtr node)

{
  if (!node->Flags.Contains(HasSYN)) return;
  
  switch (node->SynType) {
  case STrecordCase:
    fopRecordCase(node);
    break;

  default:
    node = node->Down;
    ProcCoder.DeclSpecifiers(node->Down);
    if (IntCode.CurrentTypeSpecifier) {
      node = node->Right;
      for (;;) {
  fopMemberDeclarator(node->Down->Down);
  node = node->Right;
  if (node) right(1);
  else break;
      }
    }
  }
}


void FioCoderCLASS::fopMemberDeclarator (TreeNodePtr declarator)

{
  TreeNodePtr prefixOrIdNode;
  unsigned savedDesLength, prefixCount;

  if (declarator->Atomic) return;

  switch (declarator->SynType) {
  case STfieldId:
    prefixOrIdNode = declarator->Down;
    skipPrefixStrings(prefixOrIdNode,prefixCount);
    
    if (prefixOrIdNode != 0) {
      down();
      if (prefixCount > 0)
        right(prefixCount);
      if (IntCode.PopupNode) down();
      savedDesLength = designator.l;
      appendIdToDesignator(prefixOrIdNode->StringBufferPos);
      if (declarator->Right) 
  fopArrayType(IntCode.CurrentTypeSpecifier,declarator->Right);
      else
  fopDataType(IntCode.CurrentTypeSpecifier,declarator);
      if (IntCode.PopupNode) up();
      up();
      restoreDesignator(savedDesLength);
    }
    break;

  case STpopup:
    IntCode.PopupNode = declarator;
    fopMemberDeclarator(declarator->Down);
    IntCode.PopupNode = 0;
    break;
  }
}


void FioCoderCLASS::fopRecordCase (TreeNodePtr caseNode)

{
  unsigned savedDesLength, prefixCount;
  TreeNodePtr node, nodeFieldId, tagFieldType;
  bool noTagField;

  node = caseNode->Down;
  if ((caseNode->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET())
    if (caseNode->Flags.Contains(OUTPUTIC)) {
      flushAddressStack();
      codeNl("FIOpp.PushFlag(pgf,PUT);");
    }
    else {
      flushAddressStack();
      codeNl("FIOpp.PushFlag(pgf,DontPUT)");
    }

  down(); // --> tagfield super node
  down(); // --> first tagfield prefix
  tagFieldType = node->Down;
  nodeFieldId = tagFieldType->Right->Down;
  if (tagFieldType->Flags.Contains(InvisibleIC))
    noTagField = true;
  else
    noTagField = false;
  skipPrefixStrings(nodeFieldId,prefixCount);
  if (noTagField)
    prefixCount = 0;
  if (prefixCount > 0)
    right(prefixCount);
  savedDesLength = designator.l;
  appendIdToDesignator(nodeFieldId->StringBufferPos);
  pureINPUT = false;
  fopDataType(tagFieldType,nodeFieldId); // tag field
  up(); // --> tagfield super node
  if (!pureINPUT) {
    flushAddressStack();
    codeNl("if (FIOpp.LastIO == PUT) {");
    incIndent();
    right(1); down(); // --> current variant

    flushAddressStack();
    code("switch (");
    code(designator);
    restoreDesignator(savedDesLength);
    codeNl(") {");

    node = node->Right;
    while (node && (node->SynType == STvariant)) {
      fopVariant(node->Down);
      node = node->Right;
    }

    flushAddressStack();
    if (node) {
      if (node->Flags.Contains(HasFIO)) {
  codeNl("default:");
  incIndent();
  down();
  fopMemberList(node->Down,node);
    up();
  decIndent();
      }
    }
    else
      codeNl("default: CDPpp.TagFieldError(cid);");
    flushAddressStack();
    codeNl("} // switch (tagfield)");
    
    up();
    decIndent();
    codeNl("} // if (FIOpp.LastIO ...");
  }
  else
    restoreDesignator(savedDesLength);
  up();
  if ((caseNode->Flags*SET(INPUTIC,OUTPUTIC,DONTPUTIC,-1)) != SET()) {
    flushAddressStack();
    codeNl("FIOpp.PopFlag(pgf);");
  }
}


void FioCoderCLASS::fopVariant (TreeNodePtr node)

{
  if (node->Atomic) return; // empty variant
  
  CdpCoder.CaseLabelList(node->Down);
  incIndent();
  if (node->Right->Flags.Contains(HasFIO)) {
    down();
    fopMemberList(node->Right->Down,node->Right);
    up();
    flushAddressStack();
  }
  codeNl("break;");
  decIndent();
}

/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void FioCoderCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  IntCode.INIT();
  TypCoder.INIT();
  EC_INIT();
  Atoms_INIT();
}
