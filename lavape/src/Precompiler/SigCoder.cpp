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

               implementation of class SigCoderCLASS

 **********************************************************************/

#include "SigCoder.h"  

#include"Atoms.h"
#include"Convert.h"
#include"ProCoder.h"
#include"CdpCoder.h"


SigCoderCLASS SigCoder;
static bool __INITstarted=false;


void SigCoderCLASS::SIGProcGeneration ()

{
  if (IntCode.DclsAreCoded) {
    if (IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasSIG)
        && IntCode.CurrentTypeEntry->attributes.Contains(HasSIG)
        && !IntCode.CurrentTypeEntry->SIGfrom) {
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
      code("void SIG");
      codeVariable(IntCode.CurrentTypeEntry->id);
      codeNl(" (ASN1* cid, address varAddr);");
    }
    
    switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
    case STclass:
    case STstruct:
      declareSIGsOfVariants(IntCode.CurrentTypeEntry);
    }
    return;
  }
  
impl:
  if (IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasSIG)
      && IntCode.CurrentTypeEntry->attributes.Contains(HasSIG)
      && !IntCode.CurrentTypeEntry->SIGfrom) {
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
    code("void SIG");
    codeVariable(IntCode.CurrentTypeEntry->id);
    codeNl(" (ASN1* cid, address varAddr)");
  
    designator = "vp";
   
    writeLn();
    codeNl("{");
    incIndent();
    codeVariable(IntCode.CurrentTypeEntry->id);
    code(" *vp = (");
    codeVariable(IntCode.CurrentTypeEntry->id);
    codeNl("*)varAddr;");
    codeNl("if (cid->Skip()) return;");
    writeLn();
  
    sigDataType(IntCode.CurrentTypeEntry->typeSpecifier,
                IntCode.CurrentTypeEntry->declarator);
  
    decIndent();
    code("} // END OF SIG");
    codeVariable(IntCode.CurrentTypeEntry->id);
    writeLn();
  }
} // END OF SIGProcGeneration


void SigCoderCLASS::declareSIGsOfVariants(TypeTablePtr currentTypeEntry)

{
  CHETypeTablePtr *variantType;
  
  for (variantType = (CHETypeTablePtr*)currentTypeEntry->variantTypes.first;
       variantType;
       variantType = (CHETypeTablePtr*)variantType->successor) {
    currentTypeEntry = variantType->data;
    
    if (currentTypeEntry->procGenAttributes.Contains(HasSIG)
        && currentTypeEntry->attributes.Contains(HasSIG)
        && !currentTypeEntry->SIGfrom) {
      writeLn();
      code("friend void SIG");
      codeVariable(currentTypeEntry->id);
      codeNl(" (ASN1* cid, address varAddr);");
    }
    declareSIGsOfVariants(currentTypeEntry);
  }
}


void SigCoderCLASS::sigDataType (TreeNodePtr typeSpecifier,
                                 TreeNodePtr declarator)

{
  TreeNodePtr sigAnnos;
  TypeTablePtr entryFound;

  sigAnnos = IntCode.FindAnnos(typeSpecifier,STsigIDlist);
  if (!(typeSpecifier->Flags.Contains(HasSIG) || sigAnnos))
    return;
  
  if (!declarator) goto checkType;
  if (declarator->Atomic)
    if (declarator->Atom == IdentifierA) goto checkType;
    else return;
  else
    if (declarator->SynType == STfieldId) goto checkType;
    else if (declarator->SynType != STpopup) return;
    else {
      sigDataType(typeSpecifier,declarator->Down);
      return;
    }
  
  checkType:
  if (typeSpecifier->SynType == STsignature) {
    code("SIGpp.Stop(cid,\"");
    codeVariable(typeSpecifier->Down->StringBufferPos);
    codeNl("\");");
  }
  else if (typeSpecifier->Flags.Contains(HasSIG))
  // generate signature data collection code:
    switch (typeSpecifier->SynType) {
    case STsimpleType:
      if (typeSpecifier->Down->Atom == IdentifierA) {
        IntCode.LookUpType(typeSpecifier->Down->StringBufferPos,
                           entryFound);
        if (entryFound->attributes.Contains(HasSIG))
          sigTypeRef(typeSpecifier->Down);
      }
      break;
    
    case STclass:
    case STstruct:
      sigStructType(typeSpecifier->Down);
      break;

    case STchain:
    case STchain0:
    case STchainAny:
    case STchainAny0:
      sigChainType(typeSpecifier);
      break;

    case STnested:
    case STnested0:
    case STnestedAny:
    case STnestedAny0:
      sigNestedType(typeSpecifier);
      break;

    default:
      if (declarator
          && ((declarator->Atomic && declarator->Atom == IdentifierA)
              || (!declarator->Atomic && declarator->SynType == STfieldId))
          && declarator->Right
          && !declarator->Right->Atomic
          && declarator->Right->SynType == STlengthSpec)
        sigArrayType(typeSpecifier,declarator->Right);
    }
    
  // generate signature data production code:
  if (sigAnnos)
    sigCdp(sigAnnos,typeSpecifier,declarator);
} // END OF sigDataType


void SigCoderCLASS::sigCdp (TreeNodePtr sigAnnos,
                            TreeNodePtr typeSpecifier,
                            TreeNodePtr declarator)
{
  TreeNodePtr sigIDnode;
  bool firstTime=true;
  
  sigIDnode = sigAnnos->Down;
  do {
    if (firstTime) {
      firstTime = false;
      code("if (");
    }
    else nlCode("|| ");
    code("SIGpp.Match(cid,\"");
    codeVariable(sigIDnode->Down->StringBufferPos);
    if (sigIDnode->Down->Right)
      code("\",false)");
    else
      code("\",true)");
    sigIDnode = sigIDnode->Right;
  } while (sigIDnode);
  codeNl(") {");
  incIndent();
  CdpCoder.SigCoderCalling = true;
  // to suppress "if (!baseCDP)"
  CdpCoder.pgf = "PUT";
  CdpCoder.CdpDataType(typeSpecifier,declarator);
  CdpCoder.pgf = "pgf";
  CdpCoder.SigCoderCalling = false;
  decIndent();
  codeNl("}");
} // END OF sigCdp


void SigCoderCLASS::sigArrayType (TreeNodePtr typeSpecifier,
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
    if ((typeEndNode->Down->Atom == CharA)) {
      code("CDPpp.CVTstring(PUT,cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }
    else if ((typeEndNode->Down->Atom == ByteA)) {
      code("CDPpp.CVTbytes(PUT,cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }

  savedDesLength = designator.l;
  arrayIndexLevel++;
  code("for (unsigned i");
  codeCard(arrayIndexLevel);
  code("=0; i");
  codeCard(arrayIndexLevel);
  code("<");
  if (IntCode.IsForm)
    codeAtom(subscripts->Down);
  else
    TypCoder.Expression(subscripts->Down);
  code("; i");
  codeCard(arrayIndexLevel);
  codeNl("++) {");
  incIndent();

  designator += "[i";
  Convert.CardToString(arrayIndexLevel,str);
  designator += str;
  designator += "]";

  if (subscripts->Right)
    sigArrayType(typeSpecifier,subscripts->Right);
  else {
    CdpCoder.pgf = "PUT";
    CdpCoder.CdpDataType(typeSpecifier,0);
    CdpCoder.pgf = "pgf";
  }
  decIndent();
  codeNl("}");
  restoreDesignator(savedDesLength);
  arrayIndexLevel--;
} // END OF sigArrayType


void SigCoderCLASS::sigChainType (TreeNodePtr typeSpecifier)

{
  TreeNodePtr sigAnnos;
  TypeTablePtr entryFound;
  unsigned savedIndent;
  
  if (typeSpecifier->Down->Down->Atom == IdentifierA) {
    IntCode.LookUpType(typeSpecifier->Down->Down->StringBufferPos,
                       entryFound);
    if (entryFound->attributes.Contains(HasSIG)) {
      code(designator);
      if (designator.l == 2) code("->");
      else code(".");
      code("SIG(cid,SIG");
      if (entryFound->SIGfrom)
        codeVariable(entryFound->SIGfrom->id);
      else
        codeAtom(typeSpecifier->Down->Down);
      codeNl(");");
    }
  }
  sigAnnos = IntCode.FindAnnos(typeSpecifier->Down,STsigIDlist);
  if (sigAnnos) {
    code("for (");
    indentHere(savedIndent);
    switch (typeSpecifier->SynType) {
    case STchainAny:
    case STchainAny0:
      code("ChainAnyElem *elem = ");
      break;
    case STchain:
    case STchain0:
      code("ChainElem<");
      codeAtom(typeSpecifier->Down->Down);
      code("> *elem = ");
    }
    code(designator);
    if (designator.l == 2) codeNl("->first;");
    else codeNl(".first;");
    codeNl("elem;");
    codeNl("elem = elem->successor)");
    restoreIndent(savedIndent);
    incIndent();
    sigCdp(sigAnnos,typeSpecifier,0);
    decIndent();
  }
} // END OF sigChainType


void SigCoderCLASS::sigNestedType (TreeNodePtr typeSpecifier)

{
  TypeTablePtr entryFound;

  IntCode.LookUpType(typeSpecifier->Down->Down->StringBufferPos,
                     entryFound);
  if (entryFound->attributes.Contains(HasSIG)) {
    code("if (");
    code(designator);
    if (designator.l == 2) codeNl("->ptr)");
    else codeNl(".ptr)");
    incIndent();
    code("SIG");
    if (entryFound && entryFound->SIGfrom)
      codeVariable(entryFound->SIGfrom->id);
    else
      codeVariable(typeSpecifier->Down->Down->StringBufferPos);
    code("(cid,(address)");
    code(designator);
    if (designator.l == 2) code("->ptr);");
    else codeNl(".ptr);");
    decIndent();
  }
} // END OF sigNestedType


void SigCoderCLASS::sigTypeRef (TreeNodePtr node)

{
  TypeTablePtr entryFound;

  code("SIG");
  IntCode.LookUpType(node->StringBufferPos,entryFound);
  if (entryFound->SIGfrom)
    codeVariable(entryFound->SIGfrom->id);
  else
    codeVariable(node->StringBufferPos);
  code("(cid,(address)");
  if (designator.l > 2) code("&");
  code(designator);
  codeNl(");");
} // END OF sigTypeRef


void SigCoderCLASS::sigStructType (TreeNodePtr node)

{
  
  if (IntCode.ScopeNestingLevel)
    incIndent();
  if (node->Atomic) {
    node = node->Right;
    if (!IntCode.ScopeNestingLevel) {  
      if (node->SynType == STbaseVariant) node = node->Right;
      if (node->SynType == STbaseList) {
        sigBaseList(node->Down);
        node = node->Right;
      }
    }
  }
  
  if (node) {
    IntCode.ScopeNestingLevel++;
    sigMemberList(node->Down);
    IntCode.ScopeNestingLevel--;
  }
  if (IntCode.ScopeNestingLevel)
    decIndent();
} // END OF sigStructType


void SigCoderCLASS::sigBaseList (TreeNodePtr node)

{
  TreeNodePtr nodeBase, className;
  
  for (; node; node = node->Right) {
    nodeBase = node->Down;
    if (nodeBase->Atomic)
      nodeBase = nodeBase->Right;
    if (nodeBase->Atomic)
      nodeBase = nodeBase->Right;
    code("SIG");
    for (className = nodeBase->Down;
       className->Right;
       className = className->Right);
    codeAtom(className);
    code("(cid,(address)(");
    TypCoder.CompleteClassName(nodeBase->Down);
    codeNl("*)vp);");
  }
}


void SigCoderCLASS::sigMemberList (TreeNodePtr node)

{
  if (node->Atomic && (node->Atom == NoAtom)) return;
  
  for (; node; node = node->Right) {
    for (; node && node->Atomic; node = node->Right) ;
      // skip access specs
    sigMemberDeclaration(node);
  }
}


void SigCoderCLASS::sigMemberDeclaration (TreeNodePtr node)

{
  if (!node->Flags.Contains(HasSIG)) return;
  
  switch (node->SynType) {
  case STrecordCase:
    sigRecordCase(node->Down);
    break;

  default:
    node = node->Down;
    ProcCoder.DeclSpecifiers(node->Down);
    node = node->Right;
    for (; node; node = node->Right)
      sigMemberDeclarator(node->Down->Down);
  }
}


void SigCoderCLASS::sigMemberDeclarator (TreeNodePtr node)

{
  TreeNodePtr node1;
  unsigned savedDesLength;
  
  if (node->Atomic) return;
  
  switch (node->SynType) {
  case STfieldId:
    node1 = node->Down;
    for (; node1 && (node1->Atom != IdentifierA); node1 = node1->Right);
    savedDesLength = designator.l;
    appendIdToDesignator(node1->StringBufferPos);
    if (node->Right) 
      sigArrayType(IntCode.CurrentTypeSpecifier,node->Right);
    else
      sigDataType(IntCode.CurrentTypeSpecifier,node1);
    restoreDesignator(savedDesLength);
    break;

  case STpopup:
    sigMemberDeclarator(node->Down);
    break;
  }
}


void SigCoderCLASS::sigRecordCase (TreeNodePtr node)

{
  unsigned savedDesLength;
  TreeNodePtr tagFieldId;
  
  tagFieldId = node->Down->Right->Down;
  while (tagFieldId->Atom != IdentifierA)
    tagFieldId = tagFieldId->Right;
  savedDesLength = designator.l;
  appendIdToDesignator(tagFieldId->StringBufferPos);
  sigDataType(node->Down,node->Down->Right);
  /* transmit tag field value */

  code("switch (");
  code(designator);
  restoreDesignator(savedDesLength);
  codeNl(") {");

  node = node->Right; /* first variant */
  for (; node && (node->SynType == STvariant); node = node->Right)
    sigVariant(node->Down);

  if (node) {
    if (node->Flags.Contains(HasSIG)) {
      codeNl("default:");
      incIndent();
      sigMemberList(node->Down);
      decIndent();
    }
  }
  else
    codeNl("default: Halt.HaltPMD();");
  codeNl("}");
}


void SigCoderCLASS::sigVariant (TreeNodePtr node)

{
  if (node->Atomic) return; /* empty variant */
  CaseLabelList(node->Down);
  incIndent();
  if (node->Right->Flags.Contains(HasSIG))
    sigMemberList(node->Right->Down);
  codeNl("break;");
  decIndent();
}


void SigCoderCLASS::CaseLabelList (TreeNodePtr node)

{
  for (; node; node = node->Right) {
    code("case ");
    if ((node->Atom == Plus)
    || (node->Atom == Minus)) {
      codeAtom(node);
      node = node->Right;
    }
    codeAtom(node);
    codeNl(":");
  }
}


/**********************************************************************/
/*         initialization proc. (the former "module body"):           */
/**********************************************************************/

void SigCoderCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  IntCode.INIT();
  TypCoder.INIT();
  CdpCoder.INIT();
  INIT();
  Atoms_INIT();
  IO.INIT();
}
