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
#include "TypCoder.h"
#include "CdpCoder.h"
#include "FioCoder.h"
#include "SigCoder.h"


TypCoderCLASS TypCoder;
static bool __INITstarted=false;


/************************************************************************/


void TypCoderCLASS::CodeCHENSTs ()

{
  TypeTablePtr typeTableEntry;
  
  for (typeTableEntry = IntCode.TypeTable;
       typeTableEntry;
       typeTableEntry = typeTableEntry->successor) {
    if (typeTableEntry->refModes.Contains(ChainAnyRef)
        && !typeTableEntry->refModes.Contains(ChainAnyRefElseWhere)
        && typeTableEntry->module.m)
      CodeCHEdcl(typeTableEntry);
    if ((typeTableEntry->refModes.Contains(Nested0Ref)
         || typeTableEntry->refModes.Contains(NestedRef))
        && !typeTableEntry->refModes.Contains(Nested0RefElseWhere)
        && !typeTableEntry->refModes.Contains(NestedRefElseWhere)
        && typeTableEntry->module.m)
      CodeNST0dcl(typeTableEntry);
    if (typeTableEntry->refModes.Contains(NestedRef)
        && !typeTableEntry->refModes.Contains(NestedRefElseWhere)
        && typeTableEntry->module.m)
      CodeNSTdcl(typeTableEntry);
    /*if (typeTableEntry->refModes.Contains(NestedAnyRef)
          && !typeTableEntry->refModes.Contains(NestedAnyRefElseWhere)
          && typeTableEntry->module.m)
      CodeNSTAdcl(typeTableEntry);*/
  }
}


void TypCoderCLASS::CodeAssOperator ()

{
  writeLn();
  codeVariable(IntCode.CurrentTypeEntry->id);
  code("& ");
  codeVariable(IntCode.CurrentTypeEntry->id);
  code("::operator= (const ");
  codeVariable(IntCode.CurrentTypeEntry->id);
  codeNl("& rhs)");
  codeNl("{ return rhs; }");
}


void TypCoderCLASS::CodeCHENST ()

{
  if (IntCode.CurrentTypeEntry->refModes.Contains(ChainAnyRef))
    CodeCHEdcl(IntCode.CurrentTypeEntry);
  if (IntCode.CurrentTypeEntry->refModes.Contains(Nested0Ref)
      || IntCode.CurrentTypeEntry->refModes.Contains(NestedRef))
    CodeNST0dcl(IntCode.CurrentTypeEntry);
  if (IntCode.CurrentTypeEntry->refModes.Contains(NestedRef))
    CodeNSTdcl(IntCode.CurrentTypeEntry);
  /*if (IntCode.CurrentTypeEntry->refModes.Contains(NestedAnyRef))
    CodeNSTAdcl(IntCode.CurrentTypeEntry);*/
}


void TypCoderCLASS::CodeCHEdcl (TypeTablePtr typeEntry)

{
  if (IntCode.DclsAreCoded) {
    writeLn();
    code("struct ");
    if (CmdParms.DLLexport.l) {
      code(CmdParms.DLLexport);
      code("_DLL ");
    }
    code("CHE");
    codeVariable(typeEntry->id);
    codeNl(" : ChainAnyElem {");
    incIndent();
    codeVariable(typeEntry->id);
    codeNl(" data;");
    writeLn();
    codeNl("ChainAnyElem* Clone ()");
    code("{ return new CHE");
    codeVariable(typeEntry->id);
    codeNl("(*this); }");
    writeLn();
    codeNl("void CopyData (ChainAnyElem* from)");
    code("{ this->data = ((CHE");
    codeVariable(typeEntry->id);
    codeNl("*)from)->data; }");
    decIndent();
    codeNl("};");

    if (IntCode.GlobalProperties.Flags.Contains(IncludeFile)) {
      nlCode("extern ");
      if (CmdParms.DLLexport.l) {
        code(CmdParms.DLLexport);
        code("_DLL ");
      }
      code("ChainAnyElem* NewCHE");
      codeVariable(typeEntry->id);
      codeNl(" ();");
    }
  }
  
  if (!IntCode.ImplModIsCoded) return;
  
  writeLn();
  if (!IntCode.GlobalProperties.Flags.Contains(IncludeFile))
    code("static ");
  code("ChainAnyElem* NewCHE");
  codeVariable(typeEntry->id);
  codeNl(" ()");
  code("{ return (ChainAnyElem*)(new CHE");
  codeVariable(typeEntry->id);
  codeNl("); }");
}


void TypCoderCLASS::CodeNST0dcl (TypeTablePtr typeEntry)

{  
  if (IntCode.DclsAreCoded) {
    writeLn();
    code("struct ");
    if (CmdParms.DLLexport.l) {
      code(CmdParms.DLLexport);
      code("_DLL ");
    }
    code("NST0");
    codeVariable(typeEntry->id);
    codeNl(" {");
    codeNl("protected:");
    incIndent();
    
    writeLn();
    code("void init (const NST0");
    codeVariable(typeEntry->id);
    codeNl("&);");
    
    writeLn();
    code("void copy (const NST0");
    codeVariable(typeEntry->id);
    codeNl("&);");
    
    writeLn();
    decIndent();
    codeNl("public:");
    incIndent();
    
    writeLn();
//    IntCode.FindEndOfReferenceChain(
//        typeEntry->typeSpecifier,typeEndNode,
//        ignore1,ignore2);
//    if (typeEndNode->SynType == STstruct
//          || typeEndNode->SynType == STclass)
//      code("struct ");
    codeVariable(typeEntry->id);
    codeNl(" *ptr;");
    
    writeLn();
    code("NST0");
    codeVariable(typeEntry->id);
    codeNl(" () { ptr = 0; }");
    
    writeLn();
    code("NST0");
    codeVariable(typeEntry->id);
    code(" (const NST0");
    codeVariable(typeEntry->id);
    codeNl("& n)");
    codeNl("{ init(n); }");
    
    writeLn();
    codeNl("void Destroy () { delete ptr; ptr = 0; }");
    
    writeLn();
    code("NST0");
    codeVariable(typeEntry->id);
    code("& operator= (const NST0");
    codeVariable(typeEntry->id);
    codeNl("& n)");
    codeNl("{ copy(n); return *this; }");
    
    if (typeEntry->procGenAttributes.Contains(HasCDP)
    /*&& typeEntry->attributes.Contains(HasCDP)*/) {
      writeLn();
      codeNl("void CDP (PutGetFlag pgf, ASN1 *cid, ConversionProc cdp);");
    }
    if (typeEntry->procGenAttributes.Contains(HasFIO)
    /*&& typeEntry->attributes.Contains(HasFIO)*/) {
      writeLn();
      codeNl("void FIO (PutGetFlag pgf, ASN1 *cid, ConversionProc fio);");
    }
    decIndent();
    codeNl("};");
  }
  
  if (!IntCode.ImplModIsCoded) return;

  writeLn();
  code("NESTEDINITCOPY(");
  codeVariable(typeEntry->id);
  codeNl(")");
  
  if (typeEntry->procGenAttributes.Contains(HasCDP)
      /*&& typeEntry->attributes.Contains(HasCDP)*/) {
    writeLn();
    code("NESTEDCDP(");
    if (typeEntry->CDPfrom)
      codeVariable(typeEntry->CDPfrom->id);
    else
      codeVariable(typeEntry->id);
    codeNl(")");
  }
  
  if (typeEntry->procGenAttributes.Contains(HasFIO)
      /*&& typeEntry->attributes.Contains(HasFIO)*/) {
    writeLn();
    code("NESTEDFIO(");
    codeVariable(typeEntry->id);
    codeNl(")");
  }
}


void TypCoderCLASS::CodeNSTdcl (TypeTablePtr typeEntry)

{
  if (IntCode.GlobalProperties.Flags.Contains(IncludeFile)
      && IntCode.ImplModIsCoded) return;

  writeLn();
  code("struct ");
  if (CmdParms.DLLexport.l) {
    code(CmdParms.DLLexport);
    code("_DLL ");
  }
  code("NST");
  codeVariable(typeEntry->id);
  code(" : NST0");
  codeVariable(typeEntry->id);
  codeNl(" {");
  incIndent();

  code("NST");
  codeVariable(typeEntry->id);
  codeNl("() {}");  
    
  writeLn();
  code("NST");
  codeVariable(typeEntry->id);
  code(" (const NST");
  codeVariable(typeEntry->id);
  codeNl("& n)");
  codeNl("{ init(n); }");
    
  writeLn();
  code("NST");
  codeVariable(typeEntry->id);
  code("& operator= (const NST");
  codeVariable(typeEntry->id);
  codeNl("& n)");
  codeNl("{ copy(n); return *this; }");

  writeLn();
  code("virtual ~NST");
  codeVariable(typeEntry->id);
  codeNl(" () { Destroy(); }");
  decIndent();
  codeNl("};");
}


void TypCoderCLASS::CodeNSTAdcl (TypeTablePtr typeEntry)

{
  if (IntCode.DclsAreCoded) {
    writeLn();
    code("struct ");
    if (CmdParms.DLLexport.l) {
      code(CmdParms.DLLexport);
      code("_DLL ");
    }
    code("NSTA");
    codeVariable(typeEntry->id);
    code(" : public AnyType, ");
    codeVariable(typeEntry->id);
    codeNl(" {");
    incIndent();
    codeNl("AnyType* Clone ()");
    code("{ return (AnyType*)new NSTA");
    codeVariable(typeEntry->id);
    codeNl("(*this); }");
    writeLn();
    codeNl("void CopyData (AnyType* from)");
    codeNl("{");
    incIndent();
    code("*(");
    codeVariable(typeEntry->id);
    code("*)this = *(");
    codeVariable(typeEntry->id);
    code("*)(NSTA");
    codeVariable(typeEntry->id);
    codeNl("*)from;");
    decIndent();
    codeNl("}");
    decIndent();
    codeNl("};");

    if (IntCode.GlobalProperties.Flags.Contains(IncludeFile)) {
      nlCode("extern ");
      if (CmdParms.DLLexport.l) {
        code(CmdParms.DLLexport);
        code("_DLL ");
      }
      code("AnyType* NewNSTA");
      codeVariable(typeEntry->id);
      codeNl(" ();");
    }
  }

  if (!IntCode.ImplModIsCoded) return;
  
  writeLn();
  if (!IntCode.GlobalProperties.Flags.Contains(IncludeFile))
    code("static ");
  code("AnyType* NewNSTA");
  codeVariable(typeEntry->id);
  codeNl(" ()");
  code("{ return (AnyType*)(new NSTA");
  codeVariable(typeEntry->id);
  codeNl("); }");
}


void TypCoderCLASS::TypeDcls (TreeNodePtr node)

{
  for (; node; node = node->Right)
    declaration(node->Down);
}


void TypCoderCLASS::declaration (TreeNodePtr node)

{ TreeNodePtr dclSpec=node->Down;
  bool noDeclSpecifiers=false;
  
  writeLn();
  IntCode.DclsAreCoded = true;
  declSpecifiers(dclSpec,noDeclSpecifiers);
  
  if (node->Right) declaratorList(node->Right->Down);
  if (IntCode.CurrentTypeEntry->typeSpecifier->SynType != STenumeration)
    codeNl(";");
    
  if (node->Right) {
    ProcCoder.CodeDeferredProcs();
    // generate dcl and/or impl of procs for non-struct/class/enum
  }
  else
    if (IntCode.CurrentTypeEntry) {
      CodeCHENST();
      if (IntCode.ImplModIsCoded
          && (IntCode.CurrentTypeEntry->typeSpecifier->SynType
              != STenumeration)) {
        IntCode.DclsAreCoded = false;
        // generate impl. of procs for struct/class in *.pC
        CdpCoder.CDProcGeneration();
        FioCoder.FIOProcGeneration();
        SigCoder.SIGProcGeneration();
        IntCode.DclsAreCoded = true;
      }
    }
}


void TypCoderCLASS::declSpecifiers (TreeNodePtr node,
            bool& noDeclSpecifiers)

{
  bool firstDsp=true;
  
  for (; node; node = node->Right) {
    if (firstDsp) firstDsp=false;
    else code(" ");

    if (node->Atomic)
      if (node->Atom == NoAtom) {
        noDeclSpecifiers = true;
        return;
      }
      else codeAtom(node);
    else
      switch (node->SynType) {
      case STsimpleType:
        simpleType(node->Down);
        break;
    
      case STarbitrary:
        code("ARBITRARY");
        break;

      case STbitstring:
        code("BITSTRING");
        break;
    
      case STbytes:
        code("BYTES");
        break;
    
      case STbitmap:
      case STstring:
      case STmoneyString:
        code("STRING");
        break;
    
      case STdate:
        code("DATE_");
        break;
    
      case STstring0:
        code("STRING0");
        break;
    
      case STset:
        code("SETpp");
        break;

      case STsignature:
        code("SigCerts");
        break;
    
      case STtime:
        code("TIME_");
        break;
    
      case STyear:
        code("YEAR_");
        break;
    
      case STclass:
      case STstruct:
        classSpecifier(node);
        break;
    
      case STenumeration:
        enumSpecifier (node->Down);
        break;
    
      case STchain:
      case STchain0:
      case STchainAny:
      case STchainAny0:
        code("CHAIN");
        switch (node->SynType) {
        case STchain:
          code("<");
          break;
        case STchain0:
          code("0<");
          break;
        case STchainAny:
          code("ANY/*");
          break;
        case STchainAny0:
          code("ANY0/*");
          break;
        }
        switch (node->Down->SynType) {
        case STsimpleType:
          simpleType(node->Down->Down);
          break;
        case STarbitrary:
          code("ARBITRARY");
          break;
        case STbitstring:
          code("BITSTRING");
          break;
        case STsignature:
          code("SigCerts");
          break;
        case STbitmap:
        case STbytes:
        case STstring:
          code("STRING");
          break;
        case STstring0:
          code("STRING0");
        }
        switch (node->SynType) {
        case STchain:
        case STchain0:
          code(">");
          break;
        default:
          code("*/");
        }
        break;
    
      case STchainx:
        code("CHAINX");
        break;

      case STnestedAny:
      case STnestedAny0:
        code("NESTEDANY");
        switch (node->SynType) {
        case STnestedAny:
          if (!node->Down->Atomic)
            code("/*");
          break;
        case STnestedAny0:
          code("0/*");
          break;
        }
        goto typeNames;

      case STnested:
      case STnested0:
        code("NST");
        if (node->SynType == STnested0) code("0");
        typeNames:
        switch (node->Down->SynType) {
        case STsimpleType:
          simpleType(node->Down->Down);
          break;
        case STarbitrary:
          code("ARBITRARY");
          break;
        case STbitstring:
          code("BITSTRING");
          break;
        case STsignature:
          code("SigCerts");
          break;
        case STbitmap:
        case STbytes:
        case STstring:
          code("STRING");
          break;
        case STstring0:
          code("STRING0");
        }
        switch (node->SynType) {
        case STnestedAny:
        case STnestedAny0:
          if (!node->Down->Atomic)
            code("*/");
        }
        break;

      case STconstructor:
        node = node->Down;
        writeLn();
        if (node->Atomic) {
          if (node->Atom == Virtual) {
            codeAtom(node);
            code(" ");
            node = node->Right;
          }
          if (node->Atomic) {
            codeAtom(node); // "~" of destructor id
            node = node->Right;
          }
        }
        codeAtom(node->Down);
        node = node->Right;
        balancedExp(node->Down);
        node = node->Right;
        cvQualifierList(node);
        if (node) {
          isFunctionDef = true;
          balancedExp(node->Down);
        }
        return;
      }
  }
}


void TypCoderCLASS::simpleType (TreeNodePtr node)

{
  switch (node->Atom) {
  case ByteA:
    code("byte");
    break;

  default:
    codeAtom(node);
    if ((node->Atom == IdentifierA) && !IntCode.ScopeNestingLevel)
      IntCode.SetCurrentTypeEntry(node);
  }
}


void TypCoderCLASS::classSpecifier (TreeNodePtr node)

{ bool isPublic=true;

  if (node->Down->Atomic) {
    IntCode.SetCurrentTypeEntry(node->Down);
    if (IntCode.CurrentTypeEntry->baseType) {
      code("typedef ");
      node = node->Down;
      codeVariable(node->Right->Down->StringBufferPos);
      code(" ");
      codeVariable(node->StringBufferPos);
      return;
    }
  }
  
  if (node->SynType == STclass) {
    code("class ");
    isPublic = false;
  }
  else code("struct ");
  
  node = node->Down;
  if (node->Atomic) {
    if (CmdParms.DLLexport.l) {
      code(CmdParms.DLLexport);
      code("_DLL ");
    }
    codeVariable(node->StringBufferPos);
    
    node = node->Right;
    if (!IntCode.ScopeNestingLevel) {  
      code(" ");
  
      if (node->SynType == STbaseVariant) node = node->Right;
      code(": ");
      if (node->SynType == STbaseList) {
        baseList(node->Down);
        node = node->Right;
      }
      else
        if (IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasCDP))
          code("public DObject ");
        else
          code("public AnyType ");
    }
  }
  
  if (node) {
    IntCode.ScopeNestingLevel++;
    memberList(node->Down,isPublic);
    IntCode.ScopeNestingLevel--;
  }
}


void TypCoderCLASS::baseList (TreeNodePtr node)

{
  TreeNodePtr nodeBase;
  
  do {
    nodeBase = node->Down;
    if (nodeBase->Atomic) {
      codeAtom(nodeBase);
      code(" ");
      nodeBase = nodeBase->Right;
    }
    if (nodeBase->Atomic) {
      codeAtom(nodeBase);
      code(" ");
      nodeBase = nodeBase->Right;
    }
    CompleteClassName(nodeBase->Down);
    node = node->Right;
    if (node) code(", ");
  } while (node);
}


void TypCoderCLASS::CompleteClassName (TreeNodePtr node)

{
  if (node->Atom == ClassMember) {
    code("::");
    node = node->Right;
  }
  do {
    codeAtom(node);
    node = node->Right;
    if (node) code("::");
  } while (node);
}


void TypCoderCLASS::memberList (TreeNodePtr node,
                                bool isPublic)

{
  codeNl(" {");
  incIndent();
  if (IntCode.ScopeNestingLevel == 1
  && IntCode.CurrentTypeEntry->procGenAttributes.Contains(HasCDP)) {
    code("DECLARE_DYNAMIC_CLASS(");
    codeVariable(IntCode.CurrentTypeEntry->id);
    codeNl(")");
    writeLn();
  }

  memList(node,isPublic);
/*
  if (IntCode.CurrentTypeEntry->refModes.Contains(NestedAnyRef)) {
    writeLn();
    code("virtual ~");
    codeVariable(IntCode.CurrentTypeEntry->id);
    codeNl(" () {}");
  }
*/
  if (IntCode.ScopeNestingLevel == 1) {
    // generate dcl of procs for struct/class

    if (!IntCode.CurrentTypeEntry->hasConstructor) {
//      writeLn();
//      codeVariable(IntCode.CurrentTypeEntry->id);
//      code("& (");
//      codeVariable(IntCode.CurrentTypeEntry->id);
//      code("::*asopDummy)(const ");
//      codeVariable(IntCode.CurrentTypeEntry->id);
//      codeNl("&);");
      writeLn();
      codeVariable(IntCode.CurrentTypeEntry->id);
      codeNl(" () {}");
//      code("{ asopDummy = &");
//      codeVariable(IntCode.CurrentTypeEntry->id);
//      codeNl("::operator=; }");
    }

    writeLn();
    codeNl("virtual void CopyData (AnyType *from) {");
    incIndent();
    code("*this = *(");
    codeVariable(IntCode.CurrentTypeEntry->id);
    codeNl("*)from;");
    decIndent();
    codeNl("}");

    CdpCoder.CDProcGeneration();
    FioCoder.FIOProcGeneration();
    SigCoder.SIGProcGeneration();
  }
  decIndent();
  code("}");
}


void TypCoderCLASS::memList (TreeNodePtr node,
           bool& isPublic)

{
  if (node->Atomic && (node->Atom == NoAtom)) return;
  
  for (; node; node = node->Right) {
    for (;;)
      if (node->Atomic) {
  writeLn();
  codeAtom(node);
  codeNl(":");
  if (node->Atom == Public) isPublic = true;
  else isPublic = false;
  node = node->Right;
      }
      else break;
    memberDeclaration(node,isPublic);
  }
}


void TypCoderCLASS::memberDeclaration (TreeNodePtr node,
               bool& isPublic)

{
  bool firstDeclarator=true, noDeclSpecifiers=false;
  
  isFunctionDef = false;
  
  switch (node->SynType) {
  case STrecordCase:
    recordCase(node->Down,isPublic);
    break;

  default:
    node = node->Down;
    declSpecifiers(node->Down,noDeclSpecifiers);
    for (node = node->Right; node; node = node->Right) {
      if (firstDeclarator) {
        firstDeclarator=false;
        if (!noDeclSpecifiers) code(" ");
      }
      else code(", ");
      memberDeclarator(node->Down);
    }
    if (!noDeclSpecifiers && !isFunctionDef)
      codeNl(";");
  }
}


void TypCoderCLASS::recordCase (TreeNodePtr node,
        bool& isPublic)

{
  TreeNodePtr nodeFieldId;
  bool firstCase;

  code("/*  CASE */ ");
  
  if (node->Down->Atomic) codeVariable(node->Down->StringBufferPos);
  else codeAtom(node->Down->Down);
  
  code(" ");
  
  nodeFieldId = node->Down->Right->Down;
  while (nodeFieldId->Atom != IdentifierA)
    nodeFieldId = nodeFieldId->Right;
  codeVariable(nodeFieldId->StringBufferPos);

  codeNl("; /* OF */");

  node = node->Right; /* first variant */
  firstCase = true;
  while (node && (node->SynType == STvariant)) {
    writeLn();
    if (firstCase) {
      code("//    ");
      incIndent();
      incIndent();
      incIndent();
      incIndent();
      firstCase = false;
    }
    else {
      code("//  | ");
      incIndent();
      incIndent();
      incIndent();
      incIndent();
    }
    variant(node->Down,isPublic);
    decIndent();
    decIndent();
    decIndent();
    decIndent();
    node = node->Right;
  }

  if (node) {
    writeLn();
    code("//  ELSE");
    writeLn();
    incIndent();
    incIndent();
    memList(node->Down,isPublic);
    decIndent();
    decIndent();
  }
  nlCode("//  END");
  writeLn();
}


void TypCoderCLASS::variant (TreeNodePtr node,
           bool& isPublic)

{
  if (node->Atomic) return; /* empty variant */
  CaseLabelList(node->Down);
  codeNl(":");
  memList(node->Right->Down,isPublic);
}


void TypCoderCLASS::CaseLabelList (TreeNodePtr node)

{
  for (;;) {
    if ((node->Atom == Plus)
    || (node->Atom == Minus)) {
      codeAtom(node);
      node = node->Right;
    }
    codeAtom(node);
    node = node->Right;
    if (!node) break;
    code(",");
  }
}


void TypCoderCLASS::memberDeclarator (TreeNodePtr node)

{
  declarator(node->Down);
}


void TypCoderCLASS::enumSpecifier (TreeNodePtr node)

{ TreeNodePtr node1;

  IntCode.SetCurrentTypeEntry(node);
  node1 = node->Right;
  if (!IntCode.ScopeNestingLevel && (node1->SynType == STbaseVariant)) {
    code("typedef ");
    codeVariable(node1->Down->StringBufferPos);
    code(" ");
    codeVariable(node->StringBufferPos);
    codeNl(";");
  }
  else {
    code("enum ");
    if (!IntCode.ScopeNestingLevel)
      codeVariable(node->StringBufferPos);
  
    if (node1->SynType == STbaseVariant) node1 = node1->Right;
    node1 = node1->Right; // skip menu node
    enumeration(node1);
  }
  if (!IntCode.ScopeNestingLevel) {
    // generate dcl and/or impl for enum type
    CdpCoder.CDProcGeneration();
    FioCoder.FIOProcGeneration();
    SigCoder.SIGProcGeneration();
  }
}


void TypCoderCLASS::enumeration (TreeNodePtr node)

{
  TreeNodePtr node1;
  bool firstItem;

  firstItem = true;
  code(" {");
  incIndent();
  do {
    node1 = node->Down;
    if (node1->Atom == StringConst) {
      node1 = node1->Right;
      if (node1 && !node1->Atomic)
        node1 = node1->Right;
    }
    if (node1 && (node1->Atom == IdentifierA)) {
      if (firstItem) firstItem = false;
      else code(",");
      writeLn();
      codeVariable(node1->StringBufferPos);
    }
    node = node->Right;
  }
  while (node);
  codeNl("};");
  decIndent();
}


void TypCoderCLASS::declaratorList (TreeNodePtr node)

{
  TreeNodePtr decNode;
  bool firstDeclarator=true;
  
  for (; node; node = node->Right) {
    decNode=node->Down;
    if (decNode->Atomic) {
      if (decNode->Atom == IdentifierA) {
        IntCode.SetCurrentTypeEntry(decNode);
        IntCode.PrepareProcDcls();
      }
    }
    else
      if (decNode->SynType == STpopup) {
        IntCode.SetCurrentTypeEntry(decNode->Down);
        IntCode.PrepareProcDcls();
      }
    
    if (IntCode.GlobalProperties.Flags.Contains(IncludeFile)
    && IntCode.ImplModIsCoded) return;

    if (firstDeclarator) {
      firstDeclarator=false;
      code(" ");
    }
    else code(", ");
    
    declarator(decNode);
    
    if (node->Right
    && (node->Right->SynType == STinitializerList
        || node->Right->SynType == STinitArgs
        || node->Right->SynType == STexpression)) {
      node = node->Right;
      initializer(node);
    }
  }
}


void TypCoderCLASS::declarator (TreeNodePtr node)

{
  TreeNodePtr node1;
  
  if (node->Atomic)
    switch (node->Atom) {
    case Mult:
    case And:
      codeAtom(node);
      node = node->Right;
      cvQualifierList(node);
      declarator(node);
      break;

    case Not:
      codeAtom(node);
      codeAtom(node->Right);
      break;

    case IdentifierA:
      codeAtom(node);
      node = node->Right;
      if (node && (node->SynType == STbaseVariant)) node = node->Right;
      for (; node; node = node->Right) {
        code("[");
        if (IntCode.IsForm)
          codeAtom(node->Down);
        else
          Expression(node->Down);
        // should actually be "constantExpression"
        code("]");
      }
      break;

    default:
      printf("\n++++ not yet implemented type of declarator\n");
      exit(1);
    }
    
  else
    switch (node->SynType) {
    case STfieldId:
      node1 = node->Down;
      for (; node1; node1 = node1->Right)
        if (node1->Atom == IdentifierA) {
          codeAtom(node1);
          break;
        }
      node = node->Right;
      for (; node; node = node->Right) {
        code("[");
        if (IntCode.IsForm)
          codeAtom(node->Down);
        else
          Expression(node->Down);
        // should actually be "constantExpression"
        code("]");
      }
      break;

    case STpopup:
      node = node->Down;
      if (node->Atomic) codeAtom(node);
      else { // field ID
        node = node->Down;
        for (; node; node = node->Right)
          if (node->Atom == IdentifierA) {
            codeAtom(node);
            break;
          }
      }
      break;

    case STdeclarator:
      declarator(node->Down);
      break;

    case STnestedDeclarator:
      code("(");
      declarator(node->Down->Down);
      code(")");
      break;

    case STfunction:
      node = node->Down;
      codeAtom(node);
      if (node->Atom == Operator) {
        node = node->Right;
        code(" ");
        codeAtom(node);
        code(" ");
      }
      node = node->Right;
      balancedExp(node->Down);
      node = node->Right;
      cvQualifierList(node);
      if (node)
        if (node->Atomic)
          code(" =0");
        else {
          isFunctionDef = true;
          balancedExp(node->Down);
        }
      break;

    default:
      printf("\n++++ not yet implemented type of declarator\n");
      exit(1);
    }
}


void TypCoderCLASS::cvQualifierList (TreeNodePtr& node)

{
  for (; node && node->Atomic; node = node->Right)
    switch (node->Atom) {
    case ConstA:
    case Volatile:
      code(" ");
      codeAtom(node);
      break;
    default: return;
    }
}


void TypCoderCLASS::initializer (TreeNodePtr node)

{
  switch (node->SynType) {
  case STinitializerList:
    balancedExp(node->Down);
    break;
  case STinitArgs:
    balancedExp(node->Down);
    break;
  case STexpression:
    code(" = ");
    Expression(node->Down);
  }
}


void TypCoderCLASS::Expression (TreeNodePtr node)

{
  for (; node; node = node->Right) {
    term(node->Down);
    node = node->Right;
    if (!node) return;
    codeAtom(node);
  }
}


void TypCoderCLASS::term (TreeNodePtr node)

{
  if (node->Atomic)
    switch (node->Atom) {
    case Plus:
    case PlusPlus:
    case Minus:
    case MinusMinus:
    case Mult:
    case And:
    case LogNot:
    case Not:
    case Sizeof:
    case ClassMember:
  //  case New:
  //  case Delete:
      codeAtom(node);
      node = node->Right;
    }
  
  if (node->Atomic)
    switch (node->Atom) {
    case CharConst:
    case StringConst:
    case IntConst:
    case OctalConst:
    case HexConst:
    case FloatConst:
    case Address:
    case BooleanA:
    case ByteA:
    case CharA:
    case DoubleA:
    case FloatA:
    case Int:
    case Long:
    case Short:
    case Signed:
    case Unsigned:
      codeAtom(node);
    }
  else switch (node->SynType) {
    case STexpression:
      code("(");
      Expression(node->Down);
      code(")");
      if (node->Right) {
        node = node->Right;
  term(node->Down);
      }
      break;
    case STqualExpression:
      qualExpression(node->Down);
  }
  node = node->Right;
  if (node &&
      !node->Atomic
      && ((node->SynType == STsuffixExpression)
          || (node->SynType == STsubscriptExpression))) {
    suffixExpression(node);
    node = node->Right;
  }
}


void TypCoderCLASS::qualExpression (TreeNodePtr node)

{
  for (;node; node = node->Right) codeAtom(node);
}


void TypCoderCLASS::suffixExpression (TreeNodePtr node)

{
  if (node->SynType == STsuffixExpression) {
    node = node->Down;
    if (node->Atomic) codeAtom(node); // ++ or --
    else {
      code("(");
      switch (node->SynType) {
      case STexpression:
          Expression(node->Down);
          break;
      case STactualParameters:
          if (!node->Down->Atomic) 
            expList(node->Down);
      }
      code(")");
    }
  }
  else {
    code("[");
    Expression(node->Down->Down);
    code("]");
  }
}


void TypCoderCLASS::expList (TreeNodePtr node)

{
  for (; node;) {
    Expression(node->Down);
    node = node->Right;
    if (!node) return;
    code(",");
  }
}


void TypCoderCLASS::balancedExp (TreeNodePtr node)

{
  bool beginningOfLine = false, punctuationFollows;
   
  for (; node; node = node->Right) {
    switch (node->Atom) {
    case Lbrace:
      if (!beginningOfLine) {
        writeLn();
        beginningOfLine = true;
      }
      codeAtom(node);
      writeLn();
      incIndent();
      break;
    
    case Rbrace:
      if (!beginningOfLine) {
        writeLn();
        beginningOfLine = true;
      }
      decIndent();
      codeAtom(node);
      writeLn();
      if (node->Right) {
        beginningOfLine = true;
      }
      break;
    
    case Semicolon:
      codeAtom(node);
      writeLn();
      beginningOfLine = true;
      break;

    default:
      codeAtom(node);
      beginningOfLine = false;
      if (node->Atom <= IdentifierA) {
        switch (node->Right->Atom) {
        case Modulus:
        case ExclOr:
        case Lparenth:
        case Rparenth:
        case Minus:
        case Plus:
        case Assign:
        case Lbrace:
        case Rbrace:
        case Or:
        case Not:
        case Lbracket:
        case Rbracket:
        case Semicolon:
        case Colon:
        case LessThan:
        case GreaterThan:
        case QuestionMark:
        case Comma:
        case Period:
        case Slash:
        case PointerTo:
        case PlusPlus:
        case MinusMinus:
        case MemberByOffset:
        case MemberByPointer:
        case Lshift:
        case Rshift:
        case LessEqual:
        case GreaterEqual:
        case EqualA:
        case NotEqual:
        case LogAnd:
        case LogOr:
        case MultAssign:
        case DivAssign:
        case ModulusAssign:
        case PlusAssign:
        case MinusAssign:
        case LshiftAssign:
        case RshiftAssign:
        case AndAssign:
        case ExclOrAssign:
        case OrAssign:
        case ClassMember:
        case ClassMemberPtr:
          punctuationFollows = true;
          break;
        default:
          punctuationFollows = false;
        }
        if (!punctuationFollows) code(" ");
      }
    }
  }
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void TypCoderCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  Atoms_INIT();
  IntCode.INIT();
  
  IntCode.ScopeNestingLevel = 0;
}
