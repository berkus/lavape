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


#include "DParser.h"
#include "Parser.h"
#include "ContxtCK.h"
#include "ChString.h"
#include "UNIX.h"


DParserCLASS DParser;
static bool __INITstarted=false;

/**********************************************************************/

void DParserCLASS::ImportStatements ()

{
  for (;;)
    switch (Atom) {

    case PPescape:
      PCerrors.Recovery = false;
      IntCode.GlobalProperties.Flags.INCL(PPescEncountered);
      IntCode.PushAtom();
      NextAtom();
      if (Atom != Include) return;
      else {
        includeMacro();
        if (PCerrors.Recovery)
          SkipUntilPPescape();
      }
      break;

    case NumberSign:
      SkipUntilPPescape();
      break;

    default:
      goto exit;
    } exit:
    
  NonPPstatements();
}


void DParserCLASS::includeMacro ()

{
  IntCode.Push(STinclude);
  NextAtom();
  CheckPush(StringConst);
  if (PCerrors.Recovery) return;
  IntCode.Pop();
  
  IntCode.IncludeLevel++;
  include(&StringBuffer[IntCode.TopOfStack()->Down->StringBufferPos]);
  IntCode.IncludeLevel--;
}


void DParserCLASS::include (const DString& filename)

{
  InFile *curInFile;
  DString savedIncludeFile(CmdParms.CurrentIncludeFile);
  SourcePosition sourcePos;
  
  IO.CurrentInput(&curInFile);
  if (notYetIncluded(filename)) {
    SaveSourcePosition(sourcePos);
    Parser.Parser();
    RestoreSourcePosition(sourcePos);
    IO.CloseInput();
    IO.SwitchInput(curInFile);
    CmdParms.CurrentIncludeFile = savedIncludeFile;
  }
}


bool DParserCLASS::notYetIncluded (const DString& filename)

{
  CHEString *nextDir;
  DString fullName;
  int accessible;
    
  if (filename[0] == '/') {
    fullName = filename;
    accessible = access(fullName.c,R_OK);
  }
  else
    for (nextDir = (CHEString*)CmdParms.IncludeDirectories.first;
         nextDir;
         nextDir = (CHEString*)nextDir->successor) {
      fullName = nextDir->data + slash + filename;
      accessible = access(fullName.c,R_OK);
      if (accessible == 0) break;
    }

  if (accessible == 0) {
    if (!IncludedFiles.AppendIfMissing(fullName))
      return false;
    IO.OpenInput(fullName);
    if (IO.Done) {
      CmdParms.CurrentIncludeFile = fullName;
      IO.WriteString("including ");
      IO.WriteString(fullName);
      IO.WriteLn();
      return true;
    }
  }
  IO.WriteLn();
  IO.WriteString("++++ pp: can't find include file \"");
  IO.WriteString(filename.c);
  IO.WriteString("\"");
  IO.WriteLn();
  IO.WriteLn();
  exit(1);
  return false;
}


void DParserCLASS::TypeAndFormStatements ()

{
  while ((Atom == Type)
   || (Atom == Form)) {
    typeMacro();
    NonPPstatements();
  }
}


void DParserCLASS::NonPPstatements ()

{
  SkipUntilPPescape();
  if (Atom == PPescape)
    IntCode.GlobalProperties.Flags.INCL(PPescEncountered);
  IntCode.PushAtom();
  NextAtom();
}


void DParserCLASS::typeMacro ()

{
  TreeNodePtr tos;
  bool firstTime=true;

  if (Atom == Form) {
    IntCode.IsForm = true;
    IntCode.Push(STform);
  }
  else {
    IntCode.IsForm = false;
    IntCode.Push(STtype);
  }

  IntCode.procGenAttributes = SET();
  NextAtom();
  while (Atom == Plus) {
    if (PCerrors.Recovery) return;
    NextAtom();
    switch (Atom) {
    case Cdp:
      IntCode.procGenAttributes.INCL(HasCDP);
      break;

    case Fio:
      if (PCerrors.Recovery) return;
      tos = IntCode.TopOfStack();
      tos->SynType = STform;
      IntCode.IsForm = true;
      break;

    case Sig:
      IntCode.procGenAttributes.INCL(HasSIG);
      break;

    default:
      PCerrors.Expected("CDP | FIO | SIG");
      return;
    }
    NextAtom();
  }
  if (IntCode.IsForm)
    IntCode.procGenAttributes = IntCode.procGenAttributes+SET(HasFIO,-1);
//  else if (IntCode.procGenAttributes == SET())
//    IntCode.procGenAttributes = SET(HasCDP,-1);
   
  Check(Lbrace);
  
  for (;;) {
    if (PCerrors.Recovery) return;
    switch (Atom) {
    case Struct:
    case Class:
      typeOfType = isClass;
      break;
    case Enum:
      typeOfType = isEnum;
      break;
    case Typedef:
      typeOfType = isTypedef;
      break;
    default:
      if (firstTime || (Atom != Rbrace)) {
        PCerrors.Expected("struct | class | enum | typedef");
        return;
      }
      goto cont; // !firstTime && (Atom == Rbrace)
    }

    scopeNestingLevel = 0;
    IntCode.indexLevel = 0;
    IntCode.BaseTypeEntry = 0;
    IntCode.CurrentTypeSpecifier = 0;
    declaration();
    firstTime = false;
  }
  
  cont:
  if (Atom != Rbrace) PCerrors.Expected("\"}\"");
  IntCode.Pop();
  IntCode.PushAtom();
}


void DParserCLASS::declaration ()

{
  bool isFunctionDef;
  
  IntCode.attributes = SET();
  
  IntCode.Push(STdeclaration);
  
  declSpecifiers(IntCode.attributes,isFunctionDef);
  if (PCerrors.Recovery) return;
  
  if (!isFunctionDef) {
    switch (typeOfType) {
    case isTypedef:
      IntCode.CheckTypeDefined(
        (IntCode.RefModes * SET(ChainAnyRef,NestedAnyRef,-1) == SET())
        && (IntCode.typeRefAttributes*IntCode.procGenAttributes != SET()),
        IntCode.IsForm && IntCode.typeRefAttributes.Contains(HasSYN));
      declaratorList(IntCode.attributes);
      break;
    default:
      IntCode.CurrentDeclarator = 0;
      if (IntCode.IsForm
          && !IntCode.IncludeLevel
          && IntCode.attributes.Contains(HasSYN))
        IntCode.typeInd = ++typeIndex;
      else
        IntCode.typeInd = 0;
     IntCode.TypeDef();
    }
    
    Check(Semicolon);
  }
  
  IntCode.Pop();
  if (PCerrors.Recovery) return;
  IntCode.TopOfStack()->Flags = IntCode.attributes;
/*
  if (IntCode.BaseTypeEntry
      && !PCerrors.AfterContextError) {
    ContextCK.CheckTypeSpecCompatibility(
      IntCode.CurrentDeclSpecifiers,
      IntCode.BaseTypeEntry->typeSpecifier);
  }
*/  
}


void DParserCLASS::declSpecifiers (FlagSet& attributes,
                                   bool& isFunctionDef)

{
  bool isDefinition=false,
      isTypeDef=false,
      typeAlreadySpecified=false;
  
  IntCode.Push(STdeclSpecs);
  
  IntCode.RefModes = SET();
  IntCode.CurrentRefType = 0;
  isFunctionDef = false;
  
  for (;;) {
    if (PCerrors.Recovery) return;
    switch (Atom) {
    case Class:
    case Struct:
      if (isDefinition)
    PCerrors.ContextError(DuplTypeDefSpec,
        AtomLine,
        AtomCol);
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      classSpecifier(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      isDefinition = true;
      typeAlreadySpecified = true;
      break;

    case Typedef:
      if (scopeNestingLevel > 0)
        PCerrors.ContextError(OnTopLevelOnly,
            AtomLine,
            AtomCol);
      if (isDefinition)
        PCerrors.ContextError(DuplTypeDefSpec,
            AtomLine,
            AtomCol);
      IntCode.PushAtom();
      NextAtom();
      isTypeDef = true;
      isDefinition = true;
      break;
    
    case Enum:
      if (isDefinition)
        PCerrors.ContextError(DuplTypeDefSpec,
            AtomLine,
            AtomCol);
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      enumSpecifier(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      isDefinition = true;
      typeAlreadySpecified = true;
      break;
 
    case ChainA:
    case Chain0A:
    case ChainAnyA:
    case ChainAny0A:
    case ChainX:
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      chainType(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      typeAlreadySpecified = true;
      break;
 
    case NestedA:
    case Nested0A:
    case NestedAnyA:
    case NestedAny0A:
    case NestedX:
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      nestedType(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      typeAlreadySpecified = true;
      break;
   
    case Friend:    
    case Auto:
    case Register:
    case Static:
    case Extern:
    case ConstA:
    case Volatile:
    case Virtual:
    case Inline:
      if (typeAlreadySpecified) {
        PCerrors.Expected("declarator");
        return;
      }
      IntCode.PushAtom();
      NextAtom();
      break;

    case Address:
    case BooleanA:
    case ByteA:
    case CharA:
    case DoubleA:
    case FloatA:
    case Int:
    case Long:
    case Unsigned:
    case Void:
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      simpleType(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      typeAlreadySpecified = true;
      break;

    case IdentifierA:
      if (typeAlreadySpecified) goto annos;
      simpleType(attributes);
      if (PCerrors.Recovery) return;
      if (Atom != Lparenth) {
        IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
        typeAlreadySpecified = true;
        break;
      }
      isFunctionDef = true;
      if (!scopeNestingLevel)
        PCerrors.ContextError(NotOnTopLevel,
            AtomLine,
            AtomCol);
      IntCode.InterposeDontPop(STconstructor);
      IntCode.HasConstructor = true;
    constructor:
      balancedExp(STargumentList,Lparenth,Rparenth);
      cvQualifierList();
      if (Atom == Lbrace) {
        balancedExp(STfunctionBody,Lbrace,Rbrace);
        if (Atom == Semicolon) NextAtom();
      }
      else
        Check(Semicolon);
      IntCode.Pop();
      IntCode.Pop();
      IntCode.CurrentRefType = 0;
      return;

    case Not:
      isFunctionDef = true;
      if (!scopeNestingLevel)
        PCerrors.ContextError(NotOnTopLevel,
            AtomLine,
            AtomCol);
      IntCode.InterposeDontPop(STconstructor);
      IntCode.PushAtom();
      NextAtom();
      simpleType(attributes);
      goto constructor;
  
    case ArbitraryA:
    case BitmapA:
    case BytesA:
    case BitstringA:
    case DateA:
    case MoneyStringA:
    case STRINGA:
    case STRING0A:
    case TimeA:
    case YearA:
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      stringType(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      typeAlreadySpecified = true;
      break;
    
    case Set:
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      if (IntCode.IsForm)
        PCerrors.ContextError(NotAllowedInForm,AtomLine,AtomCol);
      setType(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      typeAlreadySpecified = true;
      break;
  
    case SignatureA:
      if (typeAlreadySpecified)
        PCerrors.ContextError(DuplTypeSpec,
            AtomLine,
            AtomCol);
      signatureType(attributes);
      if (PCerrors.Recovery) return;
      IntCode.CurrentTypeSpecifier = IntCode.TopOfStack();
      typeAlreadySpecified = true;
      break;

    default:
      if (typeAlreadySpecified) goto annos;
      if (scopeNestingLevel == 0) {
        if (isDefinition) {
          PCerrors.Expected("type specifier");
          return;
        }
        if (!IntCode.IsForm) {
          PCerrors.Expected("declaration specifier");
          return;
        }
      }
      IntCode.PushEmpty();
      IntCode.Pop();
      return;
    }
  }
  annos:
  if (PCerrors.Recovery) return;
  annotations(attributes);
  IntCode.typeRefAttributes = attributes;
    // add attributes to IntCode.CurrentTypeSpecifier->Flags
  IntCode.Pop();
  if (!PCerrors.Recovery)
    IntCode.TopOfStack()->Flags += attributes;
    // add attributes to declSpecifiers->Flags
  isChain = false;
  isNested = false;
}


void DParserCLASS::qualifiedName ()

{
  IntCode.Push(STqualName);
  if (Atom == ClassMember) {
    IntCode.PushAtom();
    NextAtom();
  }
  for (;;) {
    CheckPush(IdentifierA);
    if (Atom == ClassMember) NextAtom();
    else break;
  }
}


void DParserCLASS::classSpecifier (FlagSet& attributes)

{  
  switch (Atom) {
  case Class:
    IntCode.Push(STclass);
    break;
  case Struct:
    IntCode.Push(STstruct);
  }
  NextAtom();
  if (scopeNestingLevel == 0) {
    if (PCerrors.Recovery) return;
    IntCode.DefTypeIdent();
    if (Atom == Lparenth) baseVariant();
    if (Atom == Colon) baseList();
  }
  else if (Atom == IdentifierA) {
    IntCode.RefTypeIdent();
    IntCode.Pop();
    return;
  }
  
  Check(Lbrace);
  scopeNestingLevel++;
  memberList(attributes);
  scopeNestingLevel--;
  Check(Rbrace);
  
  IntCode.Pop();
}


void DParserCLASS::baseVariant ()

{
  TreeNodePtr tos;
  
  IntCode.Push(STbaseVariant);
  NextAtom();
  Check(Like);
  CheckPush(IdentifierA);
  if (!PCerrors.Recovery) {
    tos = IntCode.TopOfStack();
    IntCode.LookUpType(tos->StringBufferPos,IntCode.BaseTypeEntry);
    if (!IntCode.BaseTypeEntry)
      PCerrors.ContextError(UndefinedType,tos->SourceLine,tos->SourceCol);
  }

  if (Atom == Comma) {
    NextAtom();
    switch (Atom) {
    case Cdp:
      NextAtom();
      Check(From);
      CheckPush(IdentifierA);
      if (!PCerrors.Recovery) {
        tos = IntCode.TopOfStack();
        IntCode.LookUpType(tos->StringBufferPos,IntCode.CDPfromEntry);
        if (!IntCode.CDPfromEntry)
          PCerrors.ContextError(UndefinedType,tos->SourceLine,tos->SourceCol);
      }
      if (Atom == Comma) {
        NextAtom();
        Check(Sig);
        Check(From);
        CheckPush(IdentifierA);
        if (!PCerrors.Recovery) {
          tos = IntCode.TopOfStack();
          IntCode.LookUpType(tos->StringBufferPos,IntCode.SIGfromEntry);
          if (!IntCode.SIGfromEntry)
            PCerrors.ContextError(UndefinedType,tos->SourceLine,tos->SourceCol);
        }
      }
      break;
    
    case Sig:
      NextAtom();
      Check(From);
      CheckPush(IdentifierA);
      if (!PCerrors.Recovery) {
        tos = IntCode.TopOfStack();
        IntCode.LookUpType(tos->StringBufferPos,IntCode.SIGfromEntry);
        if (!IntCode.SIGfromEntry)
          PCerrors.ContextError(UndefinedType,tos->SourceLine,tos->SourceCol);
      }
      if (Atom == Comma) {
        NextAtom();
        Check(Cdp);
        Check(From);
        CheckPush(IdentifierA);
        if (!PCerrors.Recovery) {
          tos = IntCode.TopOfStack();
          IntCode.LookUpType(tos->StringBufferPos,IntCode.CDPfromEntry);
          if (!IntCode.CDPfromEntry)
            PCerrors.ContextError(UndefinedType,tos->SourceLine,tos->SourceCol);
        }
      }
      break;
      
    default:
      PCerrors.Expected("CDP | SIG");
      return;
    }
  }
  
  Check(Rparenth);
  IntCode.Pop();
}


void DParserCLASS::baseList ()

{
  IntCode.Push(STbaseList);
  NextAtom();
  for (;;) {
    IntCode.Push(STbaseSpec);
    switch (Atom) {
    case IdentifierA:
      completeClassName();
      break;

    case Virtual:
      IntCode.PushAtom();
      NextAtom();
      switch (Atom) {
      case Public:
      case PrivateA:
      case Protected:
        IntCode.PushAtom();
        NextAtom();
      }
      completeClassName();
      break;

    case Public:
    case PrivateA:
    case Protected:
      IntCode.PushAtom();
      NextAtom();
      if (Atom  == Virtual) {
        IntCode.PushAtom();
        NextAtom();
      }
      completeClassName();
    }
    IntCode.Pop();
    
    if (Atom == Comma) NextAtom();
    else break;
  }
  IntCode.Pop();
}


void DParserCLASS::memberList (FlagSet& attributes)

{
  unsigned fieldCount;
  bool noPrefix;

  IntCode.Push(STmemberList);
  memList(attributes,fieldCount,noPrefix);
  IntCode.Pop();
   
  if (noPrefix)
    attributes.INCL(NoPrefix);
  if (fieldCount > 1) {
    attributes.INCL(ManyFields);
    IntCode.indexLevel++;
  }
  if (PCerrors.Recovery) return;
  IntCode.TopOfStack()->Flags += attributes;
}


void DParserCLASS::memList (FlagSet& attributes,
                            unsigned& fieldCount,
                            bool& noPrefix)

{
  FlagSet attrib, accumAttrib;
  unsigned fieldCountPerList;
  bool annotFieldList=false, isFunctionDef, firstTime=true;
  
  fieldCount = 0;
  noPrefix = false;
  accumAttrib = AndedAttributes;
  
  for (;;) {
    switch (Atom) {
    case Rbrace:
    case Or:
    case LogOr:
    case ELSE:
    case End:
      if (firstTime) {
        IntCode.PushEmpty();
        return;
      }
      goto exit2;
    }
    firstTime = false;
    
    if (!isCase)
      for (;;)
        switch (Atom) {
        case Public:
        case PrivateA:
        case Protected:
          IntCode.PushAtom();
      NextAtom();
          Check(Colon);
          break;
        default: goto exit;
        } exit:
    if (Atom == Lbrace) {
      annotFieldList = true;
      NextAtom();
    }
  
    memberDeclaration(attrib,fieldCountPerList,noPrefix,isFunctionDef);
    
    fieldCount += fieldCountPerList;
    if (PCerrors.Recovery) return;

    if (annotFieldList) {
      annotFieldList=false;
      Check(Rbrace);
      annotations(attrib);
    }
    if (!isFunctionDef) Check(Semicolon);
    accumAttrib = accumAttrib*attrib;
    attributes = attributes+(attrib*OredAttributes);
  }
  
  exit2:
  attributes = attributes+accumAttrib;
}


void DParserCLASS::memberDeclaration (FlagSet& attributes,
                                      unsigned& fieldCount,
                                      bool& noPrefix,
                                      bool& isFunctionDef)

{
  bool hasFieldId;
  TreeNodePtr tos;
  AtomType savedAtom;
  
  attributes = SET();
  fieldCount = 0;
  isFunctionDef = false;
  
  switch (Atom) {
  case Case:
  case PopupCase:
    savedAtom = Atom;
    isCase = true;
    recordCase(attributes);
    tos = IntCode.TopOfStack();
    isCase = false;
    if ((savedAtom==PopupCase)
    && !PCerrors.Recovery)
      tos->Flags.INCL(PopUpIC);
    if (attributes.Contains(HasFIO))
      fieldCount = 1;
    noPrefix = true;
    break;
    
  default:
    IntCode.Push(STmemberDeclaration);
    declSpecifiers(attributes,isFunctionDef);
    if (!isFunctionDef)
      for (;;) {
        if (PCerrors.Recovery) return;
        memberDeclarator(hasFieldId,noPrefix,isFunctionDef);
        if (isFunctionDef) break;
        if (hasFieldId) fieldCount++;
        else attributes.INCL(HasSYN);
        if (Atom == Comma) NextAtom();
        else break;
      }
    if (PCerrors.Recovery) return;
    if (fieldCount)
      IntCode.CheckTypeDefined(
        (IntCode.RefModes * SET(ChainAnyRef,NestedAnyRef,-1) == SET())
        && (IntCode.typeRefAttributes*IntCode.procGenAttributes != SET()),
        IntCode.IsForm && IntCode.typeRefAttributes.Contains(HasSYN));
    IntCode.Pop();
    if (isFunctionDef) return;
    tos = IntCode.TopOfStack();
  }
  if (PCerrors.Recovery) return;
  tos->Flags += attributes;
}


void DParserCLASS::memberDeclarator (bool& hasFieldId,
                                     bool& noPrefix,
                                     bool& isFunctionDef)

{
  FlagSet attrib;
  
  IntCode.Push(STmemberDeclarator);
  declarator(hasFieldId,noPrefix,false,attrib,isFunctionDef);

  IntCode.Pop();
}


void DParserCLASS::enumSpecifier (FlagSet& attributes)

{
  TreeNodePtr tos;
  bool prefix, itemAnnot;
  unsigned aline, acol;
  FlagSet attrib;

  IntCode.Push(STenumeration);
  NextAtom();
  if (scopeNestingLevel == 0) {
    IntCode.DefTypeIdent();
    if (Atom == Lparenth) baseVariant();
  }
  else
    PCerrors.ContextError(OnTopLevelOnly,AtomLine,AtomCol);
      
  Check(Lbrace);
  IntCode.Push(STmenu);
  IntCode.PushEmpty();
  IntCode.Pop();
  attributes = DefaultAttributes;
  if (Atom == Slash) {
    formAnnotations(attributes);
    formAnnotations(attrib);
    IntCode.PopAnnotation();
  }

  for (;;) {
    IntCode.Push(STenumItem);
    if (Atom == Lbrace) {
      if (!IntCode.IsForm)
        PCerrors.ContextError(InFormsOnly,AtomLine,AtomCol);
      itemAnnot = true;
      NextAtom();
    }
    else
      itemAnnot = false;
    if (Atom == StringConst) {
      prefix = true;
      if (!IntCode.IsForm)
        PCerrors.ContextError(InFormsOnly,AtomLine,AtomCol);
      IntCode.PushAtom();
      NextAtom();
      formAnnotations(attrib);
      IntCode.PopAnnotation();
    }
    else
      prefix = false;
    if (Atom == IdentifierA) {
      IntCode.PushAtom();
      NextAtom();
      if (Atom == Minus) {
        NextAtom();
        aline = AtomLine;
        acol = AtomCol;
        Check(Fio);
        if (prefix || !IntCode.IsForm)
          PCerrors.ContextError(NotAllowed,aline,acol);
      }
      else {
        tos = IntCode.TopOfStack();
        if (!PCerrors.Recovery)
          tos->Flags.INCL(HasSYN);
      }
      if (PCerrors.Recovery)
        return;
      if (Atom == StringConst) {
        if (!IntCode.IsForm)
          PCerrors.ContextError(InFormsOnly,AtomLine,AtomCol);
        IntCode.PushAtom();
        NextAtom();
        formAnnotations(attrib);
        IntCode.PopAnnotation();
      }
    }
    else if (Atom == StringConst)
      do {
        IntCode.PushAtom();
        NextAtom();
        formAnnotations(attrib);
        IntCode.PopAnnotation();
      }
      while (Atom == StringConst);
    else if (!prefix) {
      PCerrors.Expected("identifier");
      return;
    }
    IntCode.Pop();
    if (itemAnnot) {
      Check(Rbrace);
      formAnnotations(attrib);
      IntCode.PopAnnotation();
    }
    if (Atom != Comma)
      break;
    NextAtom();
  }
  Check(Rbrace);
  IntCode.Pop();
}


void DParserCLASS::qualName ()

{
  for (;;) {
    CheckPush(IdentifierA);
    if (Atom == ClassMember) NextAtom();
    else return;
  }
}


void DParserCLASS::qualifiedTypeName ()

{
  IntCode.Push(STqualTypeName);
  qualName();
}


void DParserCLASS::completeClassName ()

{
  IntCode.Push(STcompClassName);
  if (Atom == ClassMember) {
    IntCode.PushAtom();
    NextAtom();
  }
  qualName();
  IntCode.Pop();
}


void DParserCLASS::annotations (FlagSet& attributes)

{
  TreeNodePtr tos;
  FlagSet exclAttrib;
  bool firstSIGanno, hasSIG;
  
  if (PCerrors.Recovery) return;
  exclAttrib = SET();
  tos = IntCode.TopOfStack();
  if (attributes.Contains(HasSIG))
    hasSIG = true;
  else
    hasSIG = false;
  firstSIGanno = false;
  
  for (;;) {
    if (PCerrors.Recovery) return;
    switch (Atom) {
      
    case MinusMinus:
      NextAtom();
      if (isTagField)
        PCerrors.ContextError(NotAllowed,AtomLine,AtomCol);
      else {
        exclAttrib.INCL(HasCDP);
        exclAttrib.INCL(HasFIO);
        exclAttrib.INCL(HasSYN);
      }
      break;
      
    case Minus:
      NextAtom();
      switch (Atom) {
  
      case Cdp:
        exclAttrib.INCL(HasCDP);
        break;
  
      case Get:
        attributes.INCL(NoGET);
        break;
  
      case Fio:
        if (!IntCode.IsForm)
          PCerrors.ContextError(InFormsOnly,AtomLine,AtomCol);
        else
    if (isTagField)
      PCerrors.ContextError(NotAllowed,AtomLine,AtomCol);
    else {
      exclAttrib.INCL(HasFIO);
      exclAttrib.INCL(HasSYN);
    }
        break;
  
      default:
        PCerrors.Expected("CDP | FIO | GET");
        return;
      }
      NextAtom();
      break;
      
    case Plus:
     if (!PCerrors.Recovery)
       if (tos->Atomic)
         PCerrors.ContextError(NotAllowed,AtomLine,AtomCol);
      NextAtom();
      Check(Sig);
      sigRefs();
      attributes.INCL(HasSIG);
      if (!hasSIG)
        firstSIGanno = true;
      break;
      
    case Slash:
      formAnnotations(attributes);
      goto exit;
      
    case Lbracket:
      asn1Tag();
      break;
      
    default:
      goto exit;
    } 
  } exit:
  IntCode.PopAnnotation();
  
  attributes = attributes-exclAttrib;
  
  if (!PCerrors.Recovery) {
    if (firstSIGanno)
      tos->Flags += (attributes-SET(HasSIG,-1));
    else
      tos->Flags += attributes;
  }
}


void DParserCLASS::simpleAnnotations (FlagSet& attributes)

{
  for (;;) {
    if (PCerrors.Recovery) return;
    switch (Atom) {

    case Menu:
    case Menufirst:
    case Highemphasis:
    case Lowemphasis:
    case Leftjustified:
    case EnterA:
    case CheckA:
    case PopDownA:
    case CenteredA:
    case Rightjustified:
    case Unaligned:
    case Leftaligned:
    case Centeraligned:
    case Rightaligned:
    case Horiz:
    case Horizontal:
    case Vert:
    case VerticalA:
    case ReverseA:
    case Overlay:
    case TextA:
    case Nl0:
    case Nf0:
    case Nl1:
    case Nf1:
    case Nl2:
    case Nf2:
    case Nl3:
    case Nf3:
    case Norules:
    case Hrules:
    case Vrules:
    case Fullbox:
      IntCode.PushAtom();
      NextAtom();
      break;

    case Newline:
    case Nl:
    case Newframe:
    case Nf:
      IntCode.Push(STnewLine);
      NextAtom();
      Check(Assign);
      CheckPush(IntConst);
      IntCode.Pop();
      break;

    case Buttonmenu:
      IntCode.Push(STbuttonmenu);
      NextAtom();
      if (Atom == Lparenth) {
        NextAtom();
        CheckPush(BitmapA);
        Check(Rparenth);
      }
      else
        IntCode.PushEmpty();
      IntCode.Pop();
      break;

    case Default:
      IntCode.Push(STinitValue);
      NextAtom();
      Check(Assign);
      switch (Atom) {

      case Plus:
      case Minus:
        IntCode.PushAtom();
        NextAtom();
        switch (Atom) {

        case IntConst:
          IntCode.PushAtom();
          NextAtom();
          break;

        default:
          PCerrors.Expected("decimal constant");
        }
        break;

      case IntConst:
      case StringConst:
      case IdentifierA:
      case True:
      case False:
        IntCode.PushAtom();
        NextAtom();
        break;

      default:
        PCerrors.Expected("decimal|bool|string constant");
      }
      IntCode.Pop();
      break;
      
    case Fixedlength:
      if (isChain) {
        attributes.INCL(FixedCountIC);
        attributes.INCL(NoEllipsisIC);
        NextAtom();
      }
      else PCerrors.ContextError(NotAllowed,AtomLine,AtomCol);
      IntCode.PushEmpty();
      break;

    case GreaterThan:
      attributes.INCL(OUTPUTIC);
      attributes.EXCL(DONTPUTIC);
      IntCode.PushEmpty();
      NextAtom();
      break;

    case Indentation:
      IntCode.Push(STindentation);
      NextAtom();
      Check(Assign);
      switch (Atom) {

      case Plus:
      case Minus:
        IntCode.PushAtom();
        NextAtom();
      }
      CheckPush(IntConst);
      IntCode.Pop();
      break;

    case InvisibleA:
//      if (isTagField)
        attributes.INCL(InvisibleIC);
//      else
//        PCerrors.ContextError(NotAllowed,AtomLine,AtomCol);
      NextAtom();
      IntCode.PushEmpty();
      break;

    case LessThan:
      attributes.INCL(INPUTIC);
      NextAtom();
      IntCode.PushEmpty();
      break;

    case PointerTo:
      attributes.INCL(DONTPUTIC);
      attributes.EXCL(OUTPUTIC);
      NextAtom();
      IntCode.PushEmpty();
      break;

    case Noecho:
      IntCode.PushAtom();
      NextAtom();
      break;

    case Name:
      IntCode.Push(STwidgetName);
      NextAtom();
      Check(Assign);
      CheckPush(StringConst);
      IntCode.Pop();
      break;

    case Noellipsis:
      if (isChain) {
        attributes.INCL(NoEllipsisIC);
        NextAtom();
      }
      else if (isNested) {
        attributes.INCL(FixedCountIC);
        NextAtom();
      }
      else PCerrors.ContextError(NotAllowed,AtomLine,AtomCol);
      IntCode.PushEmpty();
      break;

    case Realize:
      if (isNested) {
        attributes.INCL(RealizeIC);
        NextAtom();
      }
      else PCerrors.ContextError(NotAllowed,AtomLine,AtomCol);
      IntCode.PushEmpty();
      break;

    case Optionmenu:
      IntCode.Push(SToptionmenu);
      NextAtom();
      IntCode.PushEmpty();
      IntCode.Pop();
      break;

    case Popupmenu:
      IntCode.Push(STpopupmenu);
      NextAtom();
      if (Atom == Lparenth) {
        NextAtom();
        if (Atom == BitmapA) {
          IntCode.PushAtom();
          NextAtom();
          if (Atom == Comma) {
            NextAtom();
            CheckPush(StringConst);
          }
        }
        else {
          CheckPush(StringConst);
          if (Atom == Comma) {
            NextAtom();
            CheckPush(BitmapA);
          }
        }
        Check(Rparenth);
      }
      else
        IntCode.PushEmpty();
      IntCode.Pop();
      break;

    case Space:
      IntCode.Push(STspace);
      NextAtom();
      Check(Assign);
      CheckPush(IntConst);
      IntCode.Pop();
      break;

    case Tab:
      IntCode.Push(STtab);
      NextAtom();
      Check(Assign);
      CheckPush(IntConst);
      IntCode.Pop();
      break;

    case ToggleA:
      IntCode.Push(STtoggle);
      NextAtom();
      if (Atom == Lparenth) {
        NextAtom();
        CheckPush(BitmapA);
        Check(Rparenth);
      }
      else
        IntCode.PushEmpty();
      IntCode.Pop();
      break;

    default:
      PCerrors.Expected("annotation keyword | \"<\" | \">\" | \"->\"");
      return;
    }
    if (Atom == Slash) {
      NextAtom();
      break;
    }
    else Check(Comma);
  }
}


void DParserCLASS::formAnnotations (FlagSet& attributes)

{
  if (Atom == Slash) {
    if (!IntCode.IsForm)
      PCerrors.ContextError(InFormsOnly,AtomLine,AtomCol);
    NextAtom();
    IntCode.PushAnnotation(STformAnnotations);
    if (Atom != Slash)
      simpleAnnotations(attributes);
    else {
      IntCode.PushEmpty();
      NextAtom();
    }
    IntCode.Pop();
  }
}


void DParserCLASS::asn1Tag ()

{
  IntCode.PushAnnotation(STasn1Annotation);
  NextAtom();
  switch (Atom) {

  case UniversalClass:
  case ApplicationClass:
  case PrivateClass:
  case ContextClass:
    IntCode.PushAtom();
    NextAtom();
    break;

  default:
    ;
  }
  CheckPush(IntConst);
  switch (Atom) {

  case Explicit:
  case Implicit:
    IntCode.PushAtom();
    NextAtom();
    break;

  default:
    ;
  }
  Check(Rbracket);
  IntCode.Pop();
}


void DParserCLASS::declaratorList (FlagSet& attributes)

{
  FlagSet attrib, accumAttrib;
  bool hasFieldId, noPrefix, isFunctionDef;
  // needed only within structs/classes
  
  accumAttrib = AndedAttributes;
  IntCode.Push(STdeclaratorList);
  for (;;) {
    attrib = SET();
    declarator(hasFieldId,noPrefix,true,attrib,isFunctionDef);
    accumAttrib *= attrib;
    attributes += attrib*OredAttributes;
    if (!PCerrors.Recovery)
      IntCode.CurrentDeclarator = IntCode.TopOfStack()->Down;
        
    if (IntCode.IsForm
        && !IntCode.IncludeLevel
        && IntCode.attributes.Contains(HasSYN))
      IntCode.typeInd = ++typeIndex;
    else
      IntCode.typeInd = 0;
    IntCode.TypeDef();
    IntCode.RefModes = SET();
/*    
    switch (Atom) {
    case Assign:
    case Lparenth:
      initializer();
    }
*/    
    if (Atom == Comma)  NextAtom();
    else break;
  }
  IntCode.Pop();
  attributes += accumAttrib;
}


void DParserCLASS::declarator (bool& hasFieldId,
                               bool& noPrefix,
                               bool optional,
                               FlagSet& attributes,
                               bool& isFunctionDef)

{
  bool dummy;
  FlagSet attrib;
  
  if (PCerrors.Recovery) return;
  hasFieldId = false;
  attributes = SET();
  
  IntCode.Push(STdeclarator);
  declaratorLevel++;
  isFunctionDef = false;
  
  switch (Atom) {
  case Mult:
  case And:
    if (scopeNestingLevel == 0)
      PCerrors.ContextError(NotOnTopLevel,AtomLine,AtomCol);
    IntCode.PushAtom();
    NextAtom();
    cvQualifierList();
    declarator(dummy,dummy,false,attrib,isFunctionDef);
    break;

  case Not:
    if (scopeNestingLevel == 0)
      PCerrors.ContextError(NotOnTopLevel,AtomLine,AtomCol);
    IntCode.PushAtom();
    NextAtom();
    CheckPush(IdentifierA);
    break;

  case IdentifierA:
    if (declaratorLevel == 1) {
      if (typeOfType == isClass) { //  ==> within struct{}
        IntCode.PushAtom();
        NextAtom();
        if ((scopeNestingLevel == 1)
            && (Atom == Lparenth)) break;
            // formal parameters of member function following
        fieldIdentifier(false,true,hasFieldId,noPrefix);
      }
      else {
        IntCode.DefTypeIdent();
        if (Atom == Lparenth) baseVariant();
      }
    }
    else {
      IntCode.PushAtom();
      NextAtom();
    }
    break;

  case Operator:
    if (declaratorLevel == 1) {
      if (typeOfType == isClass) { //  ==> within struct{}
        IntCode.PushAtom();
        NextAtom();
        IntCode.PushAtom();
        NextAtom();
        if ((scopeNestingLevel == 1)
            && (Atom == Lparenth)) break;
            // formal parameters of member function following
      }
    }
    PCerrors.ContextError(NotSupported,AtomLine,AtomCol);
    break;
/*
      identOrQualName();
      if (Atom == ClassMemberPtr) {
        IntCode.Interpose(STclassMemberPtr);
        IntCode.Pop();
        cvQualifierList();
        declarator(dummy,dummy,false,attrib,isFunctionDef);
      }
*/

  case StringConst:
    if (declaratorLevel > 1) {
      PCerrors.Expected("* | & | ( | identifier");
      declaratorLevel--;
      return;
    }
    fieldIdentifier(true,false,hasFieldId,noPrefix);
    break;

  case Popup:
    if (declaratorLevel > 1) {
      PCerrors.Expected("* | & | ( | identifier");
      declaratorLevel--;
      return;
    }
    popupDeclarator(hasFieldId,noPrefix,attributes);
    IntCode.Pop();
    declaratorLevel--;
    return;
/*
  case Lbrace:
    if (declaratorLevel > 1) {
      PCerrors.Expected("* | & | ( | identifier");
      declaratorLevel--;
      return;
    }
    NextAtom();
    declarator(hasFieldId,noPrefix,false,attributes,isFunctionDef);
    Check(Rbrace);
    annotations(attributes);
    break;
*/
  case Lparenth:
    IntCode.Push(STnestedDeclarator);
    NextAtom();
    declarator(hasFieldId,noPrefix,false,attributes,isFunctionDef);
    Check(Rparenth);
    IntCode.Pop();
    break;
/*
  case ClassMember:
    identOrQualName();
    if (Atom == ClassMemberPtr) {
      IntCode.Interpose(STclassMemberPtr);
      IntCode.Pop();
      cvQualifierList();
      declarator(dummy,dummy,false,attrib,isFunctionDef);
    }
    break;
*/  
  default:
    if (optional) IntCode.PushEmpty();
    else {
      if (declaratorLevel > 1)
        PCerrors.Expected("* | & | ( | identifier");
      else
        PCerrors.Expected("* | & | ~ | ( | { | identifier | string | POPUP");
      declaratorLevel--;
      return;
    }
  }
  
  for (; (Atom == Lparenth) || (Atom == Lbracket);)
    switch (Atom) {
    case Lparenth:
      IntCode.InterposeDontPop(STfunction);
      isFunctionDef = true;
      hasFieldId = true;
      balancedExp(STargumentList,Lparenth,Rparenth);
        // should actually be "argumentDeclarationList";
      cvQualifierList();
      if (typeOfType == isClass
          /*&& declaratorLevel == 1*/) {
        if (Atom == Lbrace) {
          balancedExp(STfunctionBody,Lbrace,Rbrace);
          if (Atom == Semicolon) NextAtom();
        }
        else {
          if (Atom == Assign) {
            NextAtom();
            if (Atom != IntConst) {
              PCerrors.Expected("0");
              return;
            }
            IntCode.PushAtom();
            if (!PCerrors.Recovery)
              if (Value(IntCode.TopOfStack()) != 0) {
                PCerrors.Expected("0");
                return;
              }
            NextAtom();
          }
          Check(Semicolon);
        }
      }
      else {
        Check(Semicolon);
        IntCode.CurrentRefType = 0;
      }        
      IntCode.Pop();
      IntCode.Pop();
      declaratorLevel--;
      return;
    
    case Lbracket:
      IntCode.indexLevel++;
      lengthSpec();
    }
  
  IntCode.Pop();
  declaratorLevel--;
}


void DParserCLASS::fieldIdentifier(bool optional,
                                   bool afterId,
                                   bool& hasFieldId,
                                   bool& noPrefix)

{
  TreeNodePtr tos;
  unsigned explanStrCount;
  FlagSet attrib;
  
  if (afterId) {
    tos = IntCode.TopOfStack();
    IntCode.InterposeDontPop(STfieldId);
    hasFieldId = true;
    fieldId(tos,noPrefix,explanStrCount);
  }
  else {
    IntCode.Push(STfieldId);
    explanationStrings(explanStrCount);
    
    if (Atom == IdentifierA) {
      hasFieldId = true;
      IntCode.PushAtom();
      tos = IntCode.TopOfStack();
      NextAtom();
      fieldId(tos,noPrefix,explanStrCount);
    }
    else
      if (!optional) PCerrors.Expected("identifier");
  }
    
  IntCode.Pop();
}


void DParserCLASS::fieldId (TreeNodePtr tos,
                            bool& noPrefix,
                            unsigned& explanStrCount)

{
  FlagSet attrib;
  
  if (IntCode.IsForm)
    if (Atom == Minus) {
      NextAtom();
      Check(Fio);
      if (PCerrors.Recovery) return;
      noPrefix = true;
    }
    else if (explanStrCount == 0) {
      if (!PCerrors.Recovery)
        tos->Flags.INCL(HasSYN);
      formAnnotations(attrib);
      IntCode.PopAnnotation();
    }
    else
      if (!PCerrors.Recovery)
        tos->Flags.INCL(HasSYN);
  explanationStrings(explanStrCount);
}


void DParserCLASS::popupDeclarator (bool& hasFieldId,
                                    bool& noPrefix,
                                    FlagSet& attributes)

{
  FlagSet ignore;

  IntCode.Push(STpopup);
  hasFieldId = false;
  Check(Popup);
  annotations(attributes);

  if (typeOfType == isTypedef) {
    IntCode.DefTypeIdent();
    if (Atom == Lparenth) baseVariant();
    annotations(attributes);
  }
  else
    fieldIdentifier(false,false,hasFieldId,noPrefix);

  IntCode.Pop();
}


void DParserCLASS::initializer ()

{
  if (Atom == Assign) {
    NextAtom();
    if (Atom == Lbrace) balancedExp(STinitializerList,Lbrace,Rbrace);
    else Expression();
  }
  else balancedExp(STinitArgs,Lparenth,Rparenth);
}


void DParserCLASS::cvQualifierList ()

{
  for (;;) {
    switch (Atom) {
    case ConstA:
    case Volatile:
      IntCode.PushAtom();
      NextAtom();
      break;
    default: return;
    }
  }
}


void DParserCLASS::identOrQualName ()

{
  if (Atom == ClassMember)
    IntCode.Push(STabsQualName);
  else {
    CheckPush(IdentifierA);
    if (Atom != ClassMember) return;
    IntCode.Interpose(STqualName);
  }
  NextAtom();
  
  for (;;) {
    CheckPush(IdentifierA);
    if (Atom == ClassMember) NextAtom();
    else break;
  }
  IntCode.Pop();
}


void DParserCLASS::balancedExp (SyntacticType sT,
                                AtomType lParenth,
                                AtomType rParenth)

{
  unsigned parenthLevel=0, oldAtomLine, oldAtomCol;
  
  IntCode.Push(sT);
  for (;;) {
    IntCode.PushAtom();
    if (Atom == lParenth) parenthLevel++;
    else if (Atom == rParenth) {
      parenthLevel--;
      if (!parenthLevel) break;
    }
    
    oldAtomLine = AtomLine;
    oldAtomCol = AtomCol;
    NextAtom();
    if (Atom == Eof) {
      AtomLine = oldAtomLine;
      AtomCol = oldAtomCol;
      PCerrors.Expected(") | }");
      return;
    }
    else if (Atom == PPescape) {
      PCerrors.Expected(") | }");
      return;
    }
  }
  NextAtom();
  IntCode.Pop();
}


void DParserCLASS::constantExpression ()

{ // preliminary and very incomplete
  intConstOrName();
  for (;;) {
    switch (Atom) {
    case Plus:
    case Minus:
      IntCode.PushAtom();
      NextAtom();
      intConstOrName();
      continue;
    }
    break;
  }
}


void DParserCLASS::intConstOrName ()

{
  switch (Atom) {
  case IntConst:
    IntCode.PushAtom();
    NextAtom();
    break;
  case IdentifierA:
    qualifiedName();
    break;
  default:
    PCerrors.Expected("integer constant | identifier");
  }
}


void DParserCLASS::signatureType (FlagSet& attributes)

{
  IntCode.Push(STsignature);
  Check(SignatureA);
  Check(Lparenth);
  CheckPush(IdentifierA);
  if (Atom == Colon) {
    NextAtom();
    CheckPush(IntConst);
  }
  if (Atom == Comma) {
    NextAtom();
    CheckPush(Verify);
  }
  Check(Rparenth);
  IntCode.Pop();
  attributes = DefaultAttributes;
  attributes.INCL(HasSIG); /* for stopping the field collect process */
}


void DParserCLASS::setType (FlagSet& attributes)

{
  IntCode.GlobalProperties.Flags.INCL(SETUsed);
  IntCode.Push(STset);
  Check(Set);
  IntCode.PushEmpty();
  IntCode.Pop();
  attributes = DefaultAttributes;
  attributes.EXCL(HasFIO);
  attributes.EXCL(HasSYN);
}


void DParserCLASS::sigRefs ()

{
  IntCode.PushAnnotation(STsigIDlist);
  Check(Lparenth);
  for (;;) {
    IntCode.Push(STsigID);
    CheckPush(IdentifierA);
    if (Atom == Colon) {
      IntCode.PushAtom();
      NextAtom();
      Check(Local);
    }
    IntCode.Pop();

    if (Atom == Comma)
      NextAtom();
    else
      break;
  }
  Check(Rparenth);
  IntCode.Pop();
}


void DParserCLASS::stringType (FlagSet& attributes)

{
  switch (Atom) {

  case ArbitraryA:
    IntCode.Push(STarbitrary);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    break;

  case BitmapA:
    IntCode.Push(STbitmap);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    break;

  case BitstringA:
    IntCode.Push(STbitstring);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    break;

  case BytesA:
    IntCode.Push(STbytes);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    break;

  case DateA:
    IntCode.Push(STdate);
    IntCode.GlobalProperties.Flags.INCL(DateTimeUsed);
    break;

  case MoneyStringA:
    IntCode.Push(STmoneyString);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    isMoneyString = true;
    break;

  case STRING0A:
    IntCode.Push(STstring0);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    break;

  case STRINGA:
    IntCode.Push(STstring);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    break;

  case TimeA:
    IntCode.Push(STtime);
    IntCode.GlobalProperties.Flags.INCL(DateTimeUsed);
    break;

  case YearA:
    IntCode.Push(STyear);
    IntCode.GlobalProperties.Flags.INCL(DateTimeUsed);
    break;
  }
  NextAtom();
  
  if (Atom == Lbracket) lengthSpec();
  else IntCode.PushEmpty();
  isMoneyString = false;
  
  IntCode.Pop();
  attributes = DefaultAttributes;
  if (PCerrors.Recovery) return;
  if (IntCode.TopOfStack()->SynType == STarbitrary) {
    attributes.EXCL(HasFIO);
    attributes.EXCL(HasSYN);
  }
  
}


void DParserCLASS::chainType (FlagSet& attributes)

{
  bool isCHX = false;

  isChain = true;
  
  switch (Atom) {
  case ChainA:
    NextAtom();
    if (Atom == LessThan) {
      IntCode.Push(STchain);
      IntCode.GlobalProperties.Flags.INCL(CHAINUsed);
      break;
    }
    else goto chx;
  case Chain0A:
    IntCode.Push(STchain0);
    IntCode.GlobalProperties.Flags.INCL(CHAINUsed);
    NextAtom();
    break;
  case ChainAnyA:
    IntCode.Push(STchainAny);
    IntCode.RefModes.INCL(ChainAnyRef);
    IntCode.GlobalProperties.Flags.INCL(CHAINANYUsed);
    NextAtom();
    break;
  case ChainAny0A:
    IntCode.Push(STchainAny0);
    IntCode.RefModes.INCL(ChainAnyRef);
    IntCode.GlobalProperties.Flags.INCL(CHAINANYUsed);
    NextAtom();
    break;
  case ChainX:
    NextAtom();
chx:
    isCHX = true;
    IntCode.Push(STchainx);
    IntCode.PushEmpty();
    IntCode.GlobalProperties.Flags.INCL(CHAINANYUsed);
    attributes = DefaultAttributes;
    break;
  };
  
  if (!isCHX) {
    Check(LessThan);
    simpleType(attributes);
    annotations(attributes);
    Check(GreaterThan);
  }

  chainLength();

  IntCode.Pop();
}


void DParserCLASS::nestedType (FlagSet& attributes)

{
  bool isNANY = false;

  isNested = true;
  
  switch (Atom) {
  case NestedA:
    NextAtom();
    if (Atom == LessThan) {
      IntCode.Push(STnested);
      IntCode.RefModes.INCL(NestedRef);
      break;
    }
    else {
      isNANY = true;
      goto nany;
    }
  case Nested0A:
    NextAtom();
    IntCode.Push(STnested0);
    IntCode.RefModes.INCL(Nested0Ref);
    break;
  case NestedX:
    isNANY = true;
  case NestedAnyA:
    NextAtom();
nany:
    IntCode.Push(STnestedAny);
    if (isNANY) IntCode.PushEmpty();
    IntCode.RefModes.INCL(NestedAnyRef);
    IntCode.GlobalProperties.Flags.INCL(NESTEDANYUsed);
    break;
  case NestedAny0A:
    NextAtom();
    IntCode.Push(STnestedAny0);
    IntCode.RefModes.INCL(NestedAnyRef);
    IntCode.GlobalProperties.Flags.INCL(NESTEDANYUsed);
    break;
  };
  
  if (!isNANY) {
    Check(LessThan);
    simpleType(attributes);
    annotations(attributes);
    Check(GreaterThan);
  }

  IntCode.Pop();
}


void DParserCLASS::simpleType (FlagSet& attributes)

{
  switch (Atom) {

  case Address:
    IntCode.Push(STsimpleType);
    IntCode.PushAtom();
    NextAtom();
    IntCode.Pop();
    attributes = SET();
    break;

  case BooleanA:
  case ByteA:
  case CharA:
  case DoubleA:
  case FloatA:
  case Int:
  case Long:
  case Unsigned:
    IntCode.Push(STsimpleType);
    IntCode.PushAtom();
    NextAtom();
    lengthSpec();
    IntCode.Pop();
    attributes = DefaultAttributes;
    break;

  case Void:
    IntCode.Push(STsimpleType);
    IntCode.PushAtom();
    NextAtom();
    IntCode.Pop();
    break;

  case IdentifierA:
    IntCode.Push(STsimpleType);
    IntCode.RefTypeIdent();
    if (IntCode.IsForm) IntCode.RefModes.INCL(FormRef);
    IntCode.TypeRef(attributes);
    IntCode.Pop();
    break;

  case ArbitraryA:
  case BitmapA:
  case BytesA:
  case BitstringA:
  case DateA:
  case MoneyStringA:
  case STRINGA:
  case STRING0A:
  case TimeA:
  case YearA:
    stringType(attributes);
    break;    

  case SignatureA:
    signatureType(attributes);
    break;    

  case Set:
    setType(attributes);
    break;    

  default:
    msgText =
      "address | bool | byte | char | float | int | long | unsigned | identifier";
    PCerrors.Expected(msgText);
  }
}



void DParserCLASS::explanationStrings (unsigned& explanStrCount)

{
  FlagSet attrib;
  
  explanStrCount = 0;
  if ((Atom == StringConst)) {
    if (!IntCode.IsForm)
      PCerrors.ContextError(InFormsOnly,AtomLine,AtomCol);
    for (;;){
      if (PCerrors.Recovery) return;
      if (Atom == StringConst) {
        explanStrCount++;
        IntCode.PushAtom();
        NextAtom();
        formAnnotations(attrib);
        IntCode.PopAnnotation();
      }
      else
        break;
    }
  }
}


void DParserCLASS::recordCase (FlagSet& attributes)

{
  FlagSet attrib, tagFieldAttrib, accumAttrib;
  TreeNodePtr tagFieldNode, tos;
  bool invisible, annotFieldList=false, noBreak=false;

  accumAttrib = AndedAttributes;
  IntCode.Push(STrecordCase);
  NextAtom();
  if (Atom == Lbrace) {
    annotFieldList = true;
    NextAtom();
  }
  tagField(tagFieldAttrib,invisible);
  tagFieldNode = IntCode.TagFieldNode;
  if (PCerrors.Recovery) return;
  
  // if (invisible) tagFieldAttrib.EXCL(HasFIO);
  // to avoid unnecessary FIO-generation if variants have no FIO
  accumAttrib = accumAttrib*tagFieldAttrib;
  attributes = tagFieldAttrib*OredAttributes;
  
  if (annotFieldList) {
    annotFieldList=false;
    Check(Rbrace);
    annotations(attributes);
    accumAttrib = accumAttrib*attributes;
    attributes = attributes*OredAttributes;
  }
  
  IntCode.indexLevel++;
  Check(Of);

  do {
    if (Atom == Or || Atom == LogOr) {
      if (Atom == LogOr)
        noBreak = true;
      else
        noBreak = false;
      NextAtom();
    }
    else
      noBreak = false;
    variant(attrib,tagFieldNode);
    if (PCerrors.Recovery) return;
    tos = IntCode.TopOfStack();
    if (noBreak)
      tos->Flags.INCL(NoBreak);
    accumAttrib *= attrib;
    attributes += attrib*OredAttributes;
    attrib = SET();
  } while (Atom == Or || Atom == LogOr);

  if (Atom == ELSE) {
    NextAtom();
    elseVariant(attrib);
    if (PCerrors.Recovery) return;
    accumAttrib *= attrib;
    attributes += attrib*OredAttributes;
  }
  Check(End);
  attributes += accumAttrib;
  IntCode.Pop();
  annotations(attributes);
}


void DParserCLASS::variant (FlagSet& attributes,
                            TreeNodePtr tagFieldNode)

{
  TreeNodePtr tos;
  unsigned fieldCount;
  bool noPrefix, annotVariant;

  IntCode.Push(STvariant);
  switch (Atom) {

  case Or:
  case LogOr:
  case ELSE:
  case End:
    IntCode.PushEmpty();
    attributes = SET();
    break;

  default:
    if (Atom == Lbrace) {
      annotVariant = true;
      NextAtom();
    }
    else
      annotVariant = false;
    caseLabelList(tagFieldNode);
    Check(Colon);
    if (PCerrors.Recovery) return;
    IntCode.Push(STmemberList);
    memList(attributes,fieldCount,noPrefix);
    IntCode.Pop();
    tos = IntCode.TopOfStack();
    if (noPrefix)
      attributes.INCL(NoPrefix);
    if (fieldCount > 1) {
      attributes.INCL(ManyFields);
      IntCode.indexLevel++;
    }
    if (annotVariant)
      Check(Rbrace);
    formAnnotations(attributes);
    if (PCerrors.Recovery) return;
    tos->Flags += attributes;
    IntCode.PopAnnotation();
  }
  IntCode.Pop();
}


void DParserCLASS::elseVariant (FlagSet& attributes)

{
  TreeNodePtr tos;
  unsigned fieldCount;
  bool noPrefix, annotVariant;

  IntCode.Push(STelseVariant);
  if (Atom == Lbrace) {
    annotVariant = true;
    NextAtom();
  }
  else
    annotVariant = false;
  memList(attributes,fieldCount,noPrefix);
  IntCode.Pop();
  tos = IntCode.TopOfStack();
  if (noPrefix)
    attributes.INCL(NoPrefix);
  if (fieldCount > 1) {
    attributes.INCL(ManyFields);
    IntCode.indexLevel++;
  }
  if (annotVariant)
    Check(Rbrace);
  formAnnotations(attributes);
  if (PCerrors.Recovery) return;
  tos->Flags = attributes;
  IntCode.PopAnnotation();
}


void DParserCLASS::caseLabelList (TreeNodePtr tagFieldNode)

{
  IntCode.Push(STcaseLabelList);
  for (;;) {
    caseLabel(tagFieldNode);
    if (Atom != Comma)
      break;
    NextAtom();
  }
  IntCode.Pop();
}


void DParserCLASS::caseLabel (TreeNodePtr tagFieldNode)

{
  TreeNodePtr tos=0;
  
  switch (Atom) {
  case Plus:
  case Minus:
    IntCode.PushAtom();
    tos = IntCode.TopOfStack();
    NextAtom();
    CheckPush(IntConst);
    break;
    
  case IdentifierA:
  case CharConst:
  case IntConst:
  case OctalConst:
  case HexConst:
  case True:
  case False:
    IntCode.PushAtom();
    NextAtom();
    break;
  default:
    PCerrors.Expected("case label");
  }
  if (!tos) tos = IntCode.TopOfStack();
  ContextCK.CheckCaseLabelExpr(tos,tagFieldNode);
  
}


void DParserCLASS::tagField (FlagSet& attributes, bool& invisible)

{
  TreeNodePtr tos;
  unsigned explanStrCount;
  FlagSet attrib;

  IntCode.Push(STtagField);

  switch (Atom) {
  case BooleanA:
  case Unsigned:
  case CharA:
  case Int:
    IntCode.Push(STsimpleType);
    IntCode.PushAtom();
    NextAtom();
    lengthSpec();
    IntCode.Pop();
    attributes = DefaultAttributes;
    break;

  case IdentifierA:
    simpleType(attributes);
    IntCode.CheckTypeDefined(
      (IntCode.RefModes * SET(ChainAnyRef,NestedAnyRef,-1) == SET())
      && (IntCode.typeRefAttributes*IntCode.procGenAttributes != SET()),
      IntCode.IsForm && IntCode.typeRefAttributes.Contains(HasSYN));
    break;

  default:
    PCerrors.Expected("type identifier | bool | char | int | unsigned");
    return;
  }
  if (PCerrors.Recovery) return;
  IntCode.TagFieldNode = IntCode.TopOfStack();
  isTagField = true;
  annotations(attributes);
  isTagField = false;
  if (PCerrors.Recovery) return;
  if (IntCode.IsForm && attributes.Contains(InvisibleIC)) {
    invisible = true;
    if (!PCerrors.Recovery) {
      IntCode.TagFieldNode->Flags.INCL(OUTPUTIC);
      IntCode.TagFieldNode->Flags.EXCL(INPUTIC);
    }
  }
  else
    invisible = false;
  
  IntCode.Push(STfieldId);
  explanationStrings(explanStrCount);
  CheckPush(IdentifierA);
  if (PCerrors.Recovery) return;
  tos = IntCode.TopOfStack();
  if (IntCode.IsForm)
    if (Atom == Minus) {
      NextAtom();
      Check(Fio);
      if (PCerrors.Recovery) return;
    }
    else if (explanStrCount == 0) {
      if (!PCerrors.Recovery)
        tos->Flags.INCL(HasSYN);
      formAnnotations(attrib);
      IntCode.PopAnnotation();
    }
    else {
      if (!PCerrors.Recovery)
        tos->Flags.INCL(HasSYN);
    }
  explanationStrings(explanStrCount);
  IntCode.Pop();
  IntCode.Pop();
}


void DParserCLASS::Expression ()

{
  IntCode.Push(STexpression);
  
  do term();
  while (!PCerrors.Recovery && infixOp());
  
  IntCode.Pop();
}


void DParserCLASS::term ()

{
  IntCode.Push(STterm);
  switch (Atom) {
  case Plus:
  case PlusPlus:
  case Minus:
  case MinusMinus:
  case Mult:
  case And:
  case LogNot:
  case Not:
  case ClassMember:
    IntCode.PushAtom();
    NextAtom();
  }

  switch (Atom) {
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
    IntCode.PushAtom();
    NextAtom();
    break;
  case Lparenth:
    NextAtom();
    Expression();
    Check(Rparenth);
    switch (Atom) {
    case Plus:
    case PlusPlus:
    case Minus:
    case MinusMinus:
    case Mult:
    case And:
    case LogNot:
    case Not:
    case ClassMember:
    case Lparenth:
      term();
    }
    break;
  case IdentifierA:
    qualExpression();
    break;
  default:
    PCerrors.Expected("constant | identifier | type-name | prefix-operator | \"(\"");
    return;
  }
  suffixExpression();
  IntCode.Pop();
}


void DParserCLASS::qualExpression ()

{
  IntCode.Push(STqualExpression);
  for (;;) {
    CheckPush(IdentifierA);
    switch (Atom) {
    case ClassMember:
    case ClassMemberPtr:
    case Period:
      IntCode.PushAtom();
      NextAtom();
      break;
    default:
      IntCode.Pop();
      return;
    }
  }
}


bool DParserCLASS::infixOp()

{
  switch (Atom) {
  case Plus:
  case Minus:
  case Mult:
  case Slash:
  case EqualA:
  case NotEqual:
  case LessThan:
  case LessEqual:
  case GreaterThan:
  case GreaterEqual:
  case Modulus:
  case ExclOr:
  case And:
  case Assign:
  case Or:
  case MemberByOffset:
  case MemberByPointer:
  case PointerTo:
  case Lshift:
  case Rshift:
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
  case QuestionMark: // for conditional expressions
  case Colon:        // for conditional expressions
    IntCode.PushAtom();
    NextAtom();
    return true;
  default:
    return false;
  }
}


void DParserCLASS::suffixExpression ()

{
  switch (Atom) {
  case Lparenth:
    IntCode.Push(STsuffixExpression);
    actualParameters();
    Check(Rparenth);
    break;
  case Lbracket:
    IntCode.Push(STsubscriptExpression);
    NextAtom();  
    Expression();
    Check(Rbracket);
    break;
  case PlusPlus:
  case MinusMinus:
    IntCode.Push(STsuffixExpression);
    IntCode.PushAtom();
    NextAtom();  
    break;
  default: return;
  }
  IntCode.Pop();
}


void DParserCLASS::actualParameters ()

{
  IntCode.Push(STactualParameters);
  Check(Lparenth);
  if (Atom == Rparenth)
    IntCode.PushEmpty();
  else
    expList();
  Check(Rparenth);
  IntCode.Pop();
}


void DParserCLASS::expList ()

{
  for (;;) {
    Expression();
    if (PCerrors.Recovery) return;
    if (Atom != Comma) break;
    NextAtom();
  }
}



void DParserCLASS::lengthSpec ()

{
  TreeNodePtr tos, tos2;
  bool fixedLength=true;

  if (Atom == Lbracket) {
    IntCode.Push(STlengthSpec);
    NextAtom();
    if (IntCode.IsForm) {
      CheckPush(IntConst);
      tos = IntCode.TopOfStack();
  
      if (!PCerrors.Recovery)
        if (Value(tos) == 0)
          PCerrors.ContextError(FieldLength0,tos->SourceLine,tos->SourceCol);
  
      switch (Atom) {

      case Plus:
        IntCode.PushAtom();
        NextAtom();
        fixedLength = false;
        break;
/*
      case Minus:
        IntCode.PushAtom();
        fixedLength = false;
        NextAtom();
*/
      }

      if (Atom == Comma) {
        NextAtom();
        CheckPush(IntConst);
        if (PCerrors.Recovery) return;
        tos2 = IntCode.TopOfStack();
        switch (Atom) {

        case Plus:
          if (!PCerrors.Recovery)
            if ((Value(tos2) > Value(tos))
            && fixedLength) /* data length > field length */
              PCerrors.ContextError(DataLengthError,
                                    tos2->SourceLine,
                                    tos2->SourceCol);
          IntCode.PushAtom();
          NextAtom();
          break;

        case Minus:
          if (!PCerrors.Recovery)
            if (Value(tos2) == 0)
              PCerrors.ContextError(DataLength0,
                                    tos2->SourceLine,
                                    tos2->SourceCol);
          IntCode.PushAtom();
          NextAtom();
          break;

        default:
          if (!PCerrors.Recovery) {
            if (Value(tos2) == 0)
              PCerrors.ContextError(DataLength0,
                                    tos2->SourceLine,
                                    tos2->SourceCol);
            if ((Value(tos2) > Value(tos))
            && fixedLength) /* data length > field length */
              PCerrors.ContextError(DataLengthError,
                                    tos2->SourceLine,
                                    tos2->SourceCol);
          }
        }
        if (isMoneyString && (Atom == Comma)) {
            NextAtom();
            CheckPush(IntConst);
        }
      }
    }
    else Expression();

    IntCode.Pop();
    Check(Rbracket);
  }
}


void DParserCLASS::chainLength ()

{
  bool empty;

  if (Atom == Lbracket) {
    if (!IntCode.IsForm)
      PCerrors.ContextError(InFormsOnly,AtomLine,AtomCol);

    empty = true;
    IntCode.Push(STlengthSpec);
    NextAtom();
    if (Atom == IntConst) {
      empty = false;
      IntCode.PushAtom();
      NextAtom();
      switch (Atom) {

      case Plus:
        IntCode.PushAtom();
        NextAtom();
        break;

      case Minus:
        IntCode.PushAtom();
        NextAtom();
        break;
      }
      if (Atom == Comma) {
        NextAtom();
        ellipsisText();
      }
    }
    else if (Atom == StringConst) {
      ellipsisText();
      empty = false;
    }
    if (empty) {
      PCerrors.Expected("integer | string constant");
      return;
    }
    IntCode.Pop();
    Check(Rbracket);
  }
}


void DParserCLASS::ellipsisText ()

{
  CheckPush(StringConst);
  if (Atom == Comma) {
    NextAtom();
    CheckPush(StringConst);
  }
}


unsigned DParserCLASS::Value (TreeNodePtr node)

{
  if (!node) return 0;

  unsigned pos=node->StringBufferPos, val=0;

  do val = 10*val+StringBuffer[pos]-'0';
  while (StringBuffer[++pos] != '\0');
  return val;
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void DParserCLASS::INIT ()
{ /* DParser*/
  if (__INITstarted) return;
  __INITstarted = true;

  IntCode.INIT();
  CmdError.INIT();
  Scanner_INIT();
  Atoms_INIT();
  ContextCK.INIT();
  PCerrors.INIT();

  isCase = false;
  isChain = false;
  isNested = false;
  isTagField = false;
  isMoneyString = false;
  typeIndex = 0;
  declaratorLevel = 0;
}
