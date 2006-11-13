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


#include "CdpCoder.h"
#include "TypCoder.h"
#include "ProCoder.h"
#include "DParser.h"
#include "ElemCode.h"
#include "Atoms.h"

#include "DString.h"
#include "DIO.h"
#include "Convert.h"

#pragma hdrstop


CdpCoderCLASS CdpCoder;
static bool __INITstarted=false;

/* ****************************************************************/
/* In the following "cdp" stands for "coding/decoding procedure". */
/* The cdp-procedures generate the calls to the elementary        */
/* ASN1pp CVT procedures:                             */
/* {CVT}{SEQUENCE|EOC|CARDINAL|INTEGER|REAL|CHAR|STRING}          */
/* ****************************************************************/

/************************************************************************/


void CdpCoderCLASS::CDProcGeneration ()

{
  unsigned savedIndent;
  TreeNodePtr node, nodeBase;

  if (IntCode.DclsAreCoded) {
    if (IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasCDP)
        //&& IntCode.CurrentTypeEntry->attributes.Contains(HasCDP)
        && !IntCode.CurrentTypeEntry->CDPfrom) {
      writeLn();
      switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
      case STclass:
      case STstruct:
        code("friend ");
        if (CmdParms.DLLexport.l) {
          code(CmdParms.DLLexport);
          code("_DLL ");
        }
        break;
      default:
        if (IntCode.GlobalProperties.Flags.Contains(IncludeFile)) {
          code("extern ");
          if (CmdParms.DLLexport.l) {
            code(CmdParms.DLLexport);
            code("_DLL ");
          }
        }
        else goto impl;
      }
      code("void CDP");
      codeVariable(IntCode.CurrentTypeEntry->id);
      code(" (");
      indentHere(savedIndent);
      codeNl("PutGetFlag pgf, ASN1* cid, address varAddr,");
      codeNl("bool baseCDP=false);");
      restoreIndent(savedIndent);

      switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
      case STclass:
      case STstruct:
        writeLn();
        code("virtual void CDP");
        code(" (");
        indentHere(savedIndent);
        codeNl("PutGetFlag pgf, ASN1* cid,");
        codeNl("bool baseCDP=false)");
        restoreIndent(savedIndent);
        code("{ CDP");
        codeVariable(IntCode.CurrentTypeEntry->id);
        codeNl("(pgf,cid,this,baseCDP); }");
        break;
      default:
        if (!IntCode.GlobalProperties.Flags.Contains(IncludeFile))
          goto impl;
      }

    }

    switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
    case STclass:
    case STstruct:
      declareCDPsOfVariants(IntCode.CurrentTypeEntry);
    }
    return;
  }

impl:

  switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
  case STstruct:
  case STclass:
    if (!IntCode.CurrentTypeEntry->baseType
    && IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasCDP)) {
      writeLn();
      writeLn();
      code("IMPLEMENT_DYNAMIC_CLASS(");
      codeVariable(IntCode.CurrentTypeEntry->id);
      code(",");
      node = IntCode.CurrentTypeEntry->typeSpecifier;
      node = node->Down->Right;
      if (node->SynType == STbaseVariant) node = node->Right;
      if (node->SynType == STbaseList) {
        node = node->Down;
        nodeBase = node->Down;
        if (nodeBase->Atomic)
          nodeBase = nodeBase->Right;
        if (nodeBase->Atomic)
          nodeBase = nodeBase->Right;
        TypCoder.CompleteClassName(nodeBase->Down);
        codeNl(")");
      }
      else codeNl("NULL)");
    }
    break;
  default: ;
  }

  if (IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasCDP)
      //&& IntCode.CurrentTypeEntry->attributes.Contains(HasCDP)
      && !IntCode.CurrentTypeEntry->CDPfrom) {
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
    code("void CDP");
    codeVariable(IntCode.CurrentTypeEntry->id);
    code(" (");
    indentHere(savedIndent);
    codeNl("PutGetFlag pgf, ASN1* cid, address varAddr,");
    switch (IntCode.CurrentTypeEntry->typeSpecifier->SynType) {
    case STstruct:
    case STclass:
      codeNl("bool baseCDP)");
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
    codeNl("if (cid->Skip()) return;");
    writeLn();

    CdpDataType(IntCode.CurrentTypeEntry->typeSpecifier,
                IntCode.CurrentTypeEntry->declarator);

    decIndent();
    code("} // END OF CDP");
    codeVariable(IntCode.CurrentTypeEntry->id);
    writeLn();
  }
}


void CdpCoderCLASS::declareCDPsOfVariants(TypeTablePtr currentTypeEntry)

{
  CHETypeTablePtr *variantType;
  unsigned savedIndent;

  for (variantType = (CHETypeTablePtr*)currentTypeEntry->variantTypes.first;
       variantType;
       variantType = (CHETypeTablePtr*)variantType->successor) {
    currentTypeEntry = variantType->data;

    if (currentTypeEntry->procGenAttributes.Contains(HasCDP)
        //&& currentTypeEntry->attributes.Contains(HasCDP)
        && !currentTypeEntry->CDPfrom) {
      writeLn();
      code("friend void CDP");
      codeVariable(currentTypeEntry->id);
      code(" (");
      indentHere(savedIndent);
      codeNl("PutGetFlag pgf, ASN1* cid, address varAddr,");
      codeNl("bool baseCDP=false);");
      restoreIndent(savedIndent);
    }
    declareCDPsOfVariants(currentTypeEntry);
  }
}


void CdpCoderCLASS::CdpDataType (TreeNodePtr typeSpecifier,
                                 TreeNodePtr declarator)

{
  //if (!typeSpecifier->Flags.Contains(HasCDP)) return;

  if (typeSpecifier->Flags.Contains(NoGET)) {
    codeNl("CDPpp.SKIP(PUT,cid);");
    return;
  }

  if (declarator
      && ( (declarator->Atomic && declarator->Atom != IdentifierA)
           || ( !declarator->Atomic
                && declarator->SynType != STfieldId
                && declarator->SynType != STpopup
              )
         )
     )
    return;

  altTag(typeSpecifier);

  if (declarator
      && ((declarator->Atomic && declarator->Atom == IdentifierA)
          || (!declarator->Atomic && declarator->SynType == STfieldId))
      && declarator->Right
      && !declarator->Right->Atomic
      && declarator->Right->SynType == STlengthSpec) {
    cdpArrayType(typeSpecifier,declarator->Right);
    return;
  }

  switch (typeSpecifier->SynType) {

  case STclass:
  case STstruct:
    cdpStructType(typeSpecifier->Down);
    break;

  case STenumeration:
    cdpEnumType(typeSpecifier->Down);
    break;

  case STsimpleType:
    cdpSimpleType(typeSpecifier->Down);
    break;

  case STchain:
  case STchainx:
  case STchain0:
  case STchainAny:
  case STchainAny0:
  case STnested:
  case STnested0:
  case STnestedAny:
  case STnestedAny0:
    cdpChainNestedType(typeSpecifier);
    break;

  case STdate:
    cdpDateType();
    break;

  case STtime:
    cdpTimeType();
    break;

  case STyear:
    cdpYearType();
    break;

  case STsignature:
    cdpSignatureType();
    break;

  default:
    code(designator);
    if (designator.l == 2) code("->");
    else code(".");
    code("CDP(");
    code(pgf);
    codeNl(",cid);");
  }
}


void CdpCoderCLASS::altTag (TreeNodePtr node)

{
  TreeNodePtr asn1Anno;

  asn1Anno = IntCode.FindAnnos(node,STasn1Annotation);
  if (asn1Anno == 0) return;

  code("cid->AltTag(");
  asn1Anno = asn1Anno->Down;
  if (asn1Anno->Atom == IntConst)
    code("Context,");
  else {
    switch (asn1Anno->Atom) {

    case UniversalClass:
      code("Universal,");
      break;

    case ApplicationClass:
      code("Application,");
      break;

    case PrivateClass:
      code("Private,");
      break;

    case ContextClass:
      code("Context,");
      break;
    }
    asn1Anno = asn1Anno->Right;
  }
  codeCard(DParser.Value(asn1Anno));
  asn1Anno = asn1Anno->Right;
  if ((asn1Anno == 0)
      || (asn1Anno->Atom == Implicit))
    codeNl(",true);");
  else
    codeNl(",false);");
}


void CdpCoderCLASS::cdpStructType (TreeNodePtr node)

{
  if (!(IntCode.ScopeNestingLevel
        || CdpCoder.SigCoderCalling))
    code("if (!baseCDP) ");
  code("CDPpp.CVTSEQUENCE(");
  code(pgf);
  codeNl(",cid);");
  incIndent();
  if (node->Atomic) {
    node = node->Right;
    if (!IntCode.ScopeNestingLevel) {
      if (node->SynType == STbaseVariant) node = node->Right;
      if (node->SynType == STbaseList) {
        cdpBaseList(node->Down);
        node = node->Right;
      }
    }
  }

  if (node) {
    IntCode.ScopeNestingLevel++;
    cdpMemberList(node->Down);
    IntCode.ScopeNestingLevel--;
  }
  decIndent();
  if (!(IntCode.ScopeNestingLevel
        || CdpCoder.SigCoderCalling))
    code("if (!baseCDP) ");
  code("CDPpp.CVTEOC(");
  code(pgf);
  codeNl(",cid);");
}


void CdpCoderCLASS::cdpBaseList (TreeNodePtr node)

{
  TreeNodePtr nodeBase, className;

  for (; node; node = node->Right) {
    nodeBase = node->Down;
    if (nodeBase->Atomic)
      nodeBase = nodeBase->Right;
    if (nodeBase->Atomic)
      nodeBase = nodeBase->Right;
    code("CDP");
    for (className = nodeBase->Down;
       className->Right;
       className = className->Right);
    codeAtom(className);
    code("(");
    code(pgf);
    codeNl(",cid,vp,true);");
//    TypCoder.CompleteClassName(nodeBase->Down);
//    codeNl("*)vp,true);");
  }
}


void CdpCoderCLASS::cdpMemberList (TreeNodePtr node)

{
  if (node->Atomic && (node->Atom == NoAtom)) return;

  for (; node; node = node->Right) {
    for (; node && node->Atomic; node = node->Right) ;
      // skip access specs
    cdpMemberDeclaration(node);
  }
}


void CdpCoderCLASS::cdpMemberDeclaration (TreeNodePtr node)

{
  if (!node->Flags.Contains(HasCDP)) return;

  switch (node->SynType) {
  case STrecordCase:
    cdpRecordCase(node->Down);
    break;

  default:
    node = node->Down;
    ProcCoder.DeclSpecifiers(node->Down);
    node = node->Right;
    for (; node; node = node->Right)
      cdpMemberDeclarator(node->Down->Down);
  }
}


void CdpCoderCLASS::cdpMemberDeclarator (TreeNodePtr node)

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
      cdpArrayType(IntCode.CurrentTypeSpecifier,node->Right);
    else
      CdpDataType(IntCode.CurrentTypeSpecifier,node1);
    restoreDesignator(savedDesLength);
    break;

  case STpopup:
    cdpMemberDeclarator(node->Down);
    break;
  }
}


void CdpCoderCLASS::cdpRecordCase (TreeNodePtr node)

{
  unsigned savedDesLength;
  TreeNodePtr tagFieldId;

  tagFieldId = node->Down->Right->Down;
  while (tagFieldId->Atom != IdentifierA)
    tagFieldId = tagFieldId->Right;
  savedDesLength = designator.l;
  appendIdToDesignator(tagFieldId->StringBufferPos);
  CdpDataType(node->Down,node->Down->Right);
  /* transmit tag field value */

  codeNl("if (cid->Skip()) return;");
  code("switch (");
  code(designator);
  restoreDesignator(savedDesLength);
  codeNl(") {");

  node = node->Right; /* first variant */
  for (; node && (node->SynType == STvariant); node = node->Right)
    cdpVariant(node);

  if (node) {
    if (node->Flags.Contains(HasCDP)) {
      codeNl("default:");
      incIndent();
      cdpMemberList(node->Down);
      decIndent();
    }
  }
  else
    codeNl("default: CDPpp.TagFieldError(cid);");
  codeNl("}");
}


void CdpCoderCLASS::cdpVariant (TreeNodePtr node)

{
  bool noBreak=node->Flags.Contains(NoBreak);

  node = node->Down;
  if (node->Atomic) return; /* empty variant */
  CaseLabelList(node->Down);
  incIndent();
  if (node->Right->Flags.Contains(HasCDP))
    cdpMemberList(node->Right->Down);
  if (!noBreak) codeNl("break;");
  decIndent();
}


void CdpCoderCLASS::CaseLabelList (TreeNodePtr node)

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


void CdpCoderCLASS::cdpEnumType (TreeNodePtr node)

{
  code("if (pgf == PUT) cid->PUTunsigned(");
  if (designator.l == 2) code("*");
  code(designator);
  codeNl(");");
  code("else ");
  if (designator.l == 2) code("*");
  code(designator);
  code(" = (");
  codeAtom(node);
  codeNl(")cid->FGETunsigned();");
}


void CdpCoderCLASS::cdpSimpleType (TreeNodePtr node)

{
  TypeTablePtr entryFound;

  if (node->Atom == IdentifierA) {
    code("CDP");
    IntCode.LookUpType(node->StringBufferPos,entryFound);
    if (entryFound && entryFound->CDPfrom)
      codeVariable(entryFound->CDPfrom->id);
    else
      codeAtom(node);
    code("(");
    code(pgf);
    code(",cid,");
    if (designator.l > 2) code("&");
  }
  else {
    code("CDPpp.CVT");
    codeAtom(node);
    code("(");
    code(pgf);
    code(",cid,");
    if (designator.l == 2) code("*");
  }
  code(designator);
  codeNl(");");
}


void CdpCoderCLASS::cdpArrayType (TreeNodePtr typeSpecifier,
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
      code("CDPpp.CVTstring(");
      code(pgf);
      code(",cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }
    else if ((typeEndNode->Down->Atom == ByteA)) {
      code("CDPpp.CVTbytes(");
      code(pgf);
      code(",cid,");
      if (designator.l == 2) code("*");
      code(designator);
      codeNl(");");
      return;
    }

  savedDesLength = designator.l;
  code("CDPpp.CVTSEQUENCE(");
  code(pgf);
  codeNl(",cid);");
  incIndent();
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
    cdpArrayType(typeSpecifier,subscripts->Right);
  else
    CdpDataType(typeSpecifier,0);
  decIndent();
  codeNl("}");
  restoreDesignator(savedDesLength);
  arrayIndexLevel--;
  decIndent();
  code("CDPpp.CVTEOC(");
  code(pgf);
  codeNl(",cid);");
}


void CdpCoderCLASS::cdpChainNestedType (TreeNodePtr node)

{
  code(designator);
  if (designator.l == 2) code("->");
  else code(".");
  code("CDP(");
  code(pgf);
  code(",cid");

  switch (node->SynType) {
  case STchainAny:
  case STchainAny0:
    code(",CDP");
    TypeName(node->Down,true);
    code(",NewCHE");
    TypeName(node->Down,true);
    break;
  case STnestedAny:
  case STnestedAny0:
    /*code(",CDP");
    TypeName(node->Down,true);
    code(",NewNSTA");
    TypeName(node->Down,true);*/
    break;
  case STchain:
  case STchain0:
    code(",CDP");
    TypeName(node->Down,true);
    break;
  case STnested:
  case STnested0:
    if (node->Down->SynType == STsimpleType) {
      code(",CDP");
      TypeName(node->Down,true);
    }
    break;
  default: ;
  }
  codeNl(");");
}


void CdpCoderCLASS::cdpSignatureType ()

{
  code("AFM.CDPSigCerts(");
  code(pgf);
  code(",cid,(address)&");
  code(designator);
  codeNl(");");
  if (!SigCoderCalling) {
    code("AFM.CertList((address)&");
    code(designator);
    codeNl(");");
  }
}


void CdpCoderCLASS::cdpDateType ()

{
  code("CDPDATE_(");
  code(pgf);
  code(",cid,(address)&");
  code(designator);
  codeNl(");");
}


void CdpCoderCLASS::cdpTimeType ()

{
  code("CDPTIME_(");
  code(pgf);
  code(",cid,(address)&");
  code(designator);
  codeNl(");");
}


void CdpCoderCLASS::cdpYearType ()

{
  code("CDPYEAR_(");
  code(pgf);
  code(",cid,(address)&");
  code(designator);
  codeNl(");");
}


void CdpCoderCLASS::TypeName (TreeNodePtr node, bool cdpName)

{
  TypeTablePtr entryFound;

  switch (node->SynType) {
  case STsimpleType:
    if (cdpName && (node->Down->Atom == IdentifierA)) {
      IntCode.LookUpType(node->Down->StringBufferPos,entryFound);
      if (entryFound && entryFound->CDPfrom)
        codeVariable(entryFound->CDPfrom->id);
      else
        codeAtom(node->Down);
    }
    else
      codeAtom(node->Down);
    break;

  default:
    switch (node->SynType) {
    case STarbitrary:
      code("ARBITRARY");
      break;

    case STbytes:
      code("BYTES");
      break;

    case STbitmap:
    case STstring:
    case STstring0:
      code("STRING");
      break;

    case STbitstring:
      code("BITSTRING");
      break;

    case STset:
      code("SET");
      break;

    case STsignature:
      code("SigCerts");
    }
  }
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void CdpCoderCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  IntCode.INIT();
  TypCoder.INIT();
  DParser.INIT();
  EC_INIT();
  Atoms_INIT();
  IO.INIT();

  SigCoderCalling = false;
  pgf = "pgf";
  IntCode.ScopeNestingLevel = 0;
  arrayIndexLevel = 0;
}
