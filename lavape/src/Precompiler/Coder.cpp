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


#include "ChString.h"
#include "DIO.h"
#include "MachDep.h"


#include "Coder.h"
#include "Atoms.h"
#include "CmdError.h"
#include "CmdParms.h"
#include "ElemCode.h"
#include "PCerrors.h"
#include "TypCoder.h"
#include "CdpCoder.h"
#include "FioCoder.h"
#include "SigCoder.h"
#include "ProCoder.h"
//#include "SynCoder.h"

#include <sys/types.h>
#ifndef WIN32
#include <time.h>
#endif
#include <sys/stat.h>

#pragma hdrstop


static ChainOfString included;

CoderCLASS Coder;
static bool __INITstarted=false;


/************************************************************************/

void CoderCLASS::Coder ()

{
  TreeNodePtr topLevelNode;
  FlagSet attributes;
  unsigned lineLength=0, loc;
  bool startOfLine=false;
  DString baseName, depFile;
  CHEString *iFile;
  time_t tval;

  IO.CloseInput();
  if (PCerrors.ErrorCount > 0) {
    IO.CloseOutput();
    return;
  }

  if (IntCode.GlobalProperties.Flags.Contains(IncludeFile))
    IntCode.ImplModIsCoded = false; /* first produce .h file */
  else
    IntCode.ImplModIsCoded = true; /* produce .C file only */
  IO.OpenInput(CmdParms.SourceFile);
  /* reposition to the beginning of the source file */
  
  for (topLevelNode = IntCode.TreeRoot;
       topLevelNode->Flags.Contains(Included); // NOT EOF-node
       topLevelNode = topLevelNode->Right);

  modGen = IntCode.GlobalProperties.Flags.Contains(MODgen);

  if (modGen) {
    IO.Remove(CmdParms.OutputFile);
    IO.OpenOutput(CmdParms.OutputFile);
    if (!IO.Done) CmdError.CommandError(OpenErrorOutput);
    IO.BigBuffer();
  }

  if (!IntCode.GlobalProperties.Flags.Contains(PPescEncountered))
  { /* copy source file without modification */
    for (; !topLevelNode->Atomic; // NOT EOF-node
         topLevelNode = topLevelNode->Right);
    /* find end-of-file node */

    if (modGen) {
      copyText(topLevelNode,false);
      IO.CloseOutput();
#ifndef WIN32
      chmod(CmdParms.OutputFile.c,S_IRUSR|S_IRGRP);
#endif
      goto makeDepFile;
    }
  }

  tval = time(0);
  //code("// generation time: ");
  //code(ctime(&tval));
  for (;;) {
    if (IntCode.GlobalProperties.Flags.Contains(IncludeFile)
        && IntCode.ImplModIsCoded) {
      if (!CmdParms.HasImplMod) {
        codeNl("#ifdef __GNUC__");
        code("#pragma implementation \"");
        code(CmdParms.ModuleName);
        codeNl(".h\"");
        codeNl("#endif");
        writeLn();
      }
      codeNl("//*******************************************************************");
      codeNl("//");
      code  ("//   public procedures of structs/classes defined in ");
      code(CmdParms.ModuleName);
      codeNl(".h");
      codeNl("//");
      codeNl("//*******************************************************************");
      writeLn();
      code("#include \"");
      code(CmdParms.ModuleName);
      codeNl(".h\"");
      writeLn();
    }
    else
      if (modGen) {
        copyText(topLevelNode,false);
        writeLn();
      }

    if (modGen) fixedImports(); /* ASN1, ASN1pp ... */

    imports(topLevelNode); /* user programmed imports */
    writeLn();
    codeNl("#pragma hdrstop");
    
    if (modGen) {
      if (!(IntCode.GlobalProperties.Flags.Contains(IncludeFile)
          && IntCode.ImplModIsCoded)) {
        copyText(topLevelNode,false);
        /* copy source up to and excluding the first $ */
        indentHere(savedIndent);
      }
      if (IntCode.GlobalProperties.Flags.Contains(IncludeFile)
          && (IntCode.ImplModIsCoded))
        IntCode.DclsAreCoded = false;
      else
        IntCode.DclsAreCoded = true;
      TypCoder.CodeCHENSTs();
    }

    if (topLevelNode->Atom == PPescape) topLevelNode = topLevelNode->Right;
    
    if (modGen) {
      /* code generation for the remaining $ macros (# $FROM) */
      while (!topLevelNode->Atomic /*NOT EOF-node*/) {
        IntCode.IsForm = false;
        switch (topLevelNode->SynType) {

        case STform:
          IntCode.IsForm = true;
        case STtype:
          if (!(IntCode.GlobalProperties.Flags.Contains(IncludeFile)
                && IntCode.ImplModIsCoded))
            TypCoder.TypeDcls(topLevelNode->Down);
          else {
            IntCode.DclsAreCoded = false;
            ProcCoder.CodeProcs(topLevelNode->Down);
          }
/*          if (IntCode.GlobalProperties.Flags.Contains(SYNgen)
              && IntCode.ImplModIsCoded
              && (topLevelNode->SynType == STform))
              SynCoder.SyntaxGeneration(topLevelNode->Down);*/
          break;

        case STcall:
        case STcallSuspend:
          codeNl("{");
          incIndent();
          callMacro(topLevelNode);
          decIndent();
          codeNl("}");
          break;

        case STcheck:
          codeNl("{");
          incIndent();
          checkMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STcheckpoint:
          checkpointMacro();
          break;

        case STdrop:
          dropMacro(topLevelNode->Down);
          break;

        case STget:
        case STsuspend:
          codeNl("{");
          incIndent();
          getMacro(topLevelNode);
          decIndent();
          codeNl("}");
          break;

        case STgetPrivate:
          codeNl("{");
          incIndent();
          getPrivMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STorgunit:
          codeNl("{");
          incIndent();
          orgunitMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STprocedure:
          codeNl("{");
          incIndent();
          procedureMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STput:
          codeNl("{");
          incIndent();
          putMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STputPrivate:
          codeNl("{");
          incIndent();
          putPrivMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STreturn:
          codeNl("{");
          incIndent();
          returnMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STshow:
          codeNl("{");
          incIndent();
          showMacro(topLevelNode);
          decIndent();
          codeNl("}");
          break;

        case STstart:
          codeNl("{");
          incIndent();
          startMacro(topLevelNode->Down);
          decIndent();
          codeNl("}");
          break;

        case STstop:
          stopMacro();
          break;

        }

  topLevelNode = topLevelNode->Right;
  /* skip the macro text up to the concluding ";" / "}" */
  skipText(topLevelNode);
        
        topLevelNode = topLevelNode->Right; // next $
        if (!(IntCode.GlobalProperties.Flags.Contains(IncludeFile)
              && IntCode.ImplModIsCoded)) {
          copyText(topLevelNode,false);
          /* copy source up to and excluding the next $ */
          indentHere(savedIndent);
        }
        if (topLevelNode->Atom == PPescape)
          topLevelNode = topLevelNode->Right; // next macro
      } /* while */
    }
    else {
      /* only syntax generation from the $FORM macros */

      while (!topLevelNode->Atomic) {
/*        if (topLevelNode->SynType == STform)
            SynCoder.SyntaxGeneration(topLevelNode->Down);*/
        topLevelNode = topLevelNode->Right; // ";" or "}"
        topLevelNode = topLevelNode->Right; // next $
        if (topLevelNode->Atom == PPescape)
          topLevelNode = topLevelNode->Right;
      }
    }
    
    if (IntCode.ImplModIsCoded) break;  // from entire code generation loop
    // (this was the second or only pass through the loop)
       
    else
      if (!modGen
          || (IntCode.GlobalProperties.Flags.Contains(IncludeFile)
              && !IntCode.FunctionsAreToBeGenerated))
        break;

    IntCode.ImplModIsCoded = true;
    IO.CloseInput();
    IO.OpenInput(CmdParms.SourceFile);
    /* second pass, generation of ...G.cpp for .ph */
    sourceLine = 1;
    sourceCol = 1;
    IO.CloseOutput();
#ifndef WIN32
    chmod(CmdParms.OutputFile.c,S_IRUSR|S_IRGRP);
#endif
    IO.Remove(CmdParms.OutputFileImplMod);
    IO.OpenOutput(CmdParms.OutputFileImplMod);
    if (!IO.Done) {
      IO.Remove(CmdParms.OutputFile);
      CmdError.CommandError(OpenErrorOutput);
    }
    IO.BigBuffer();
    topLevelNode = IntCode.TreeRoot;
  } /* for loop */
  
  if (modGen) {
    IO.CloseOutput();

#ifndef WIN32
    if (IntCode.ImplModIsCoded
    && IntCode.GlobalProperties.Flags.Contains(IncludeFile))
      chmod(CmdParms.OutputFileImplMod.c,S_IRUSR|S_IRGRP);
    else
      chmod(CmdParms.OutputFile.c,S_IRUSR|S_IRGRP);
#endif
  }


/*  if (IntCode.GlobalProperties.Flags.Contains(SYNgen))
      SynCoder.WriteSynDefFile();*/
  /* write form syntax definitions to *.syn file */


  makeDepFile:
  
  if ((CmdParms.SourceFile.Contains(".pC",0,loc)
       || CmdParms.SourceFile.Contains(".ph",0,loc))
      && (loc == (unsigned)(CmdParms.SourceFile.l-3)))
    baseName = CmdParms.SourceFile.Substr(0,loc);
  
  depFile = CmdParms.SourceFile + "d";
    
  IO.Remove(depFile);
  IO.OpenOutput(depFile);
  if (!IO.Done) {
    IO.Remove(depFile);
    CmdError.CommandError(OpenErrorOutput);
  }
  IO.BigBuffer();
  if (IntCode.GlobalProperties.Flags.Contains(IncludeFile))
    IO.WriteString(baseName + ".h");
  else
    IO.WriteString(baseName + ".cpp");
  lineLength += IO.Length;
  if (IntCode.IsForm) {
    IO.WriteString((" " + baseName) + ".syn");
    lineLength += IO.Length;
  }
  IO.WriteString(": $(PP) " + CmdParms.SourceFile);
  lineLength += IO.Length;
  
  for (iFile = (CHEString*)DParser.IncludedFiles.first;
       iFile;
       iFile = (CHEString*)iFile->successor) {
    if (lineLength >60) {
      IO.WriteString(" \\");
      IO.WriteLn();
      IO.WriteString("        ");
      lineLength = 8;
      startOfLine = true;
    }
    if (startOfLine)
      startOfLine = false;
    else {
      IO.Write(' ');
      lineLength++;
    }
    IO.WriteString(iFile->data);
    lineLength += IO.Length;
  }
  IO.WriteLn();
  IO.CloseOutput();

#ifdef SOLARIS2
  chmod(depFile.c,S_IRUSR|S_IRGRP);
#endif
}



/* ************************************************************************/
/* executable MACROS: */


void CoderCLASS::callMacro (TreeNodePtr node)

{
  TreeNodePtr callNode, nodeReceiver, nodeInObj, nodeOutObj;

  callNode = node;

  codeNl("OrcServ.Call();");

  /* set receiver auxiliary variables: */
  nodeReceiver = node->Down->Right;
  buildReceiverChain(nodeReceiver);

  /* put input objects of called procedure: */
  node = nodeReceiver->Right;
  if (!node || (node->SynType != STinputObjects)) {
    codeNl("OrcServ.PutCall(");
    incIndent();
    codeNl("false,");
    codeNl("0,0,");
    codeNl("0,0,");
    if (callNode->Down->Atom == Sysproc)
      code("true,");
    else
      code("false,");
    receivers();
    codeNl(");");
    decIndent();
  }
  else { /* node->SynType = STinputObjects */
    nodeInObj = node->Down;
    node = node->Right;
    do {
      setAuxVar("OID",nodeInObj->Down);
      if ((nodeInObj->Right)
          && (nodeInObj->Right->SynType == STobVal)) {
        setAuxVar("VAL",nodeInObj->Right->Down);
  if (nodeInObj->Right->Flags.Contains(Encrypt))
    codeNl("ASN1pp.IPCMem->PUTencrypted = 1;");
    }

      codeNl("OrcServ.PutCall(");
      incIndent();
      if (nodeInObj->SynType == STsysObId)
        codeNl("true,");
      else
        codeNl("false,");

      /* object id: */
      constOrVar("OID",nodeInObj->Down);
      codeNl(",");

      /* object value (optional): */
      nodeInObj = nodeInObj->Right;
      if ((nodeInObj)
          && (nodeInObj->SynType == STobVal)) {
        constOrVar("VAL",nodeInObj->Down);
        codeNl(",");
        nodeInObj = nodeInObj->Right;
      }
      else
        codeNl("0,0,");
      if (callNode->Down->Atom == Sysproc)
        code("true,");
      else
        code("false,");
      receivers();
      codeNl(");");
      decIndent();
    }
    while (nodeInObj); /* further input objects to be put */
  }
  if (nodeReceiver->SynType != STexpression)
    codeNl("((TRCVchain*)ASN1pp.IPCMem->RCVchain)->Destroy();");

  /* wait and get output objects from called procedure: */
  if (callNode->SynType == STcallSuspend)
    if (!node)
      codeNl("OrcServ.CallAndHalt(false,0,0,true);");
    else {
      nodeOutObj = node->Down; /* output objects */
      do {
        setAuxVar("OID",nodeOutObj->Down);
        codeNl("OrcServ.CallAndHalt(");
        if (nodeOutObj->SynType == STsysObId)
          code("true,");
        else
          code("false,");

        /* object id: */
        constOrVar("OID",nodeOutObj->Down);

        nodeOutObj = nodeOutObj->Right;
        if (nodeOutObj)
          codeNl(",false);");
        else
          codeNl(",true);");
      }
      while (nodeOutObj); /* further objects requested */
    }

  if (node) {
    nodeOutObj = node->Down; /* output objects */
    do {
      setAuxVar("OID",nodeOutObj->Down);
      if ((nodeOutObj->Right)
          && (nodeOutObj->Right->SynType == STobVal))
        setAuxVar("VAL",nodeOutObj->Right->Down);

      codeNl("OrcServ.GetReturn(");
      incIndent();
      if (nodeOutObj->SynType == STsysObId)
        codeNl("true, ");
      else
        codeNl("false, ");

      /* object id: */
      constOrVar("OID",nodeOutObj->Down);

      /* object value: */
      nodeOutObj = nodeOutObj->Right;
      codeNl(",");
      if ((nodeOutObj) && (nodeOutObj->SynType == STobVal)) {
        constOrVar("VAL",nodeOutObj->Down);
        codeNl(");");
        nodeOutObj = nodeOutObj->Right;
      }
      else
        codeNl("0,0);");
      decIndent();
    }
    while (nodeOutObj); /* further objects requested */
  }
}


void CoderCLASS::checkMacro (TreeNodePtr node)

{
  setAuxVar("OID",node->Down);

  codeNl("OrcServ.CheckAndReturn(");
  incIndent();
  if (node->SynType == STsysObId)
    codeNl("true,");
  else
    codeNl("false,");

  /* object id: */
  constOrVar("OID",node->Down);

  /* AVAILABLE parameter: */
  node = node->Right;
  codeNl(",");
  TypCoder.Expression(node->Down);
  codeNl(");");
  decIndent();
}


void CoderCLASS::checkpointMacro ()

{
  codeNl("OrcServ.Checkpoint();");
}


void CoderCLASS::dropMacro (TreeNodePtr node)

{
  TreeNodePtr formNode;
  TypeTablePtr entryFound;

  do {
    formNode = node->Right->Down;

    code("FIOpp.PutDropRequest((address)&");
    TypCoder.Expression(formNode); /* form variable name */
    code(",\"");

    IntCode.LookUpType(node->StringBufferPos,entryFound);
    if (entryFound->module.Defined())
      code(entryFound->module);
    else
      code(CmdParms.ModuleName); /* syntax/module name */
    code("\",\"");

    codeVariable(node->StringBufferPos); /* type name */
    codeNl("\");");

    node = node->Right->Right;
  }
  while (node);
}


void CoderCLASS::getMacro (TreeNodePtr node)

{
  TreeNodePtr savedNode, nodeSender, nodeS, nodeVar, nodeType;
  bool senderRequested;

  if (node->SynType == STsuspend) {
    node = node->Down;
    savedNode = node;
    do {
      nodeSender = node->Right;
      if (nodeSender && (nodeSender->SynType == STobVal))
        nodeSender = nodeSender->Right;
      if (nodeSender && (nodeSender->SynType == STorgDest))
        senderRequested = true;
      else
        senderRequested = false;

      setAuxVar("OID",node->Down);
      if (node->Right && (node->Right->SynType == STobVal))
        setAuxVar("VAL",node->Right->Down);

      code("OrcServ.CheckAndHalt(");
      if (node->SynType == STsysObId)
        code("true,");
      else
        code("false,");

      /* object id: */
      constOrVar("OID",node->Down);

      /* skip sender program list: */
      if (senderRequested)
        node = nodeSender->Right;
      else
        node = nodeSender;

      if (node)
        codeNl(",false);");
      else
        codeNl(",true);"); /* end of list of start objects */
    }
    while (node); /* further objects requested */ 
    node = savedNode;
  }
  else
    node = node->Down;
    
  do {
    nodeSender = node->Right;
    if (nodeSender && (nodeSender->SynType == STobVal))
      nodeSender = nodeSender->Right;
    if (nodeSender && (nodeSender->SynType == STorgDest))
      senderRequested = true;
    else
      senderRequested = false;

    setAuxVar("OID",node->Down);
    if (node->Right && (node->Right->SynType == STobVal))
      setAuxVar("VAL",node->Right->Down);

    if (senderRequested)
      codeNl("OrcServ.GetS(");
    else
      codeNl("OrcServ.Get(");
    incIndent();
    if (node->SynType == STsysObId)
      codeNl("true,");
    else
      codeNl("false,");

    /* object id: */
    constOrVar("OID",node->Down);

    /* object value: */
    node = node->Right;
    codeNl(",");
    if (node && (node->SynType == STobVal))
      constOrVar("VAL",node->Down);
    else
      code("0,0");

    /* possibly the sender program name is requested: */
    if (senderRequested) {
      nodeS = nodeSender->Down;
      codeNl(",");
      code("(address)&");
      if ((nodeS->SynType == STexpression)) {
        TypCoder.Expression(nodeS->Down);
        codeNl(".progID,CDPSTRING,");
        code("(address)&");
        TypCoder.Expression(nodeS->Down);
        codeNl(".orgUnit,CDPTOrgUnit);");
      }
      else {
        nodeS = nodeS->Down;
        if (nodeS->Atomic) {
          nodeVar = nodeS->Right->Down;
          nodeType = nodeS;
        }
        else nodeVar = nodeS->Down;
        TypCoder.Expression(nodeVar);
        if (nodeS->Atomic) {
          code(",CDP");
          codeAtom(nodeType);
          nodeS = nodeS->Right->Right;
        }
        else {
          code(",CDPSTRING");
          nodeS = nodeS->Right;
        }          
        code(",(address)&");
        if (nodeS->Atomic) {
          nodeVar = nodeS->Right->Down;
          nodeType = nodeS;
        }
        else nodeVar = nodeS->Down;
        TypCoder.Expression(nodeVar);
        if (nodeS->Atomic) {
          code(",CDP");
          codeAtom(nodeType);
        }
        else code(",CDPTSOrgunit");
        codeNl(");");
      }
      node = nodeSender->Right;
    }
    else {
      codeNl(");");
      node = nodeSender;
    }

    decIndent();
  }
  while (node); /* further objects requested */
}


void CoderCLASS::getPrivMacro (TreeNodePtr node)

{
  do {
    setAuxVar("OID",node->Down);
    if (node->Right && (node->Right->SynType == STobVal))
      setAuxVar("VAL",node->Right->Down);

    codeNl("OrcServ.GetPriv(");
    incIndent();
    if (node->SynType == STsysObId)
      codeNl("true,");
    else
      codeNl("false,");

    /* object id: */
    constOrVar("OID",node->Down);

    /* object value: */
    node = node->Right;
    codeNl(",");
    if (node && (node->SynType == STobVal)) {
      constOrVar("VAL",node->Down);
      node = node->Right;
    }
    else
      code("0,0");

    /* DONE parameter: */
    codeNl(",");
    TypCoder.Expression(node->Down);
    codeNl(");");
    decIndent();
    node = node->Right;
  }
  while (node); /* further private objects requested */
}


void CoderCLASS::orgunitMacro (TreeNodePtr node)

{
  TreeNodePtr orgUnitNode;

  setAuxVar("OID",node->Down);
  orgUnitNode = node->Right->Down;
  if (orgUnitNode->Atomic
      && orgUnitNode->Atom == StringConst)
    buildOrgUnitConst(orgUnitNode);

  codeNl("OrcServ.Orgunit(");
  incIndent();

  /* org-unit variable id: */
  constOrVar("OID",node->Down);
  codeNl(",/* ORGVAR*/");

  /* computed org-unit value: */
  if (orgUnitNode->Atomic)
    if (orgUnitNode->Atom == StringConst)
      codeNl("ASN1pp.IPCMem->RCVorgUnit.c, CDPTSOrgunit); /* ORGVAL */");
    else {
      code("(address)&");
      TypCoder.Expression(orgUnitNode->Right->Down);
      code(",CDP");
      codeAtom(orgUnitNode);
      codeNl("); /* ORGVAL */");
    }
  else {
    code("(address)&");
    TypCoder.Expression(orgUnitNode->Down);
    codeNl(",CDPTSOrgunit /* ORGVAL */);");
  }
  decIndent();
}


void CoderCLASS::procedureMacro (TreeNodePtr node)

{
  if (node->Atomic) {
    codeNl("OrcServ.CalledGet(");
    incIndent();
    codeNl("false,");
    codeNl("0,0,");
    codeNl("0,0);");
    decIndent();
    return;
  }
  do {
    setAuxVar("OID",node->Down);
    if (node->Right && (node->Right->SynType == STobVal))
      setAuxVar("VAL",node->Right->Down);

    codeNl("OrcServ.CalledGet(");
    incIndent();
    if (node->SynType == STsysObId)
      codeNl("true,");
    else
      codeNl("false,");

    /* object id: */
    constOrVar("OID",node->Down);

    /* object value: */
    node = node->Right;
    codeNl(",");
    if (node && (node->SynType == STobVal)) {
      constOrVar("VAL",node->Down);
      node = node->Right;
    }
    else
      code("0,0");

    codeNl(");");
    decIndent();
  }
  while (node); /* further input objects */
}


void CoderCLASS::putMacro (TreeNodePtr node)

{
  TreeNodePtr nodeReceiver;
  bool receiversSpecified;

  do {
    nodeReceiver = node->Right;
    if (nodeReceiver && (nodeReceiver->SynType == STobVal))
      nodeReceiver = nodeReceiver->Right;
    if (nodeReceiver
        && ((nodeReceiver->SynType == STorgDest)
            || (nodeReceiver->SynType == STsysOrgDest)))
      receiversSpecified = true;
    else
      receiversSpecified = false;

    setAuxVar("OID",node->Down);
    if (node->Right && (node->Right->SynType == STobVal)) {
      setAuxVar("VAL",node->Right->Down);
      if (node->Right->Flags.Contains(Encrypt))
    codeNl("ASN1pp.IPCMem->PUTencrypted = 1;");
   }

    if (receiversSpecified) {
      buildReceiverChain(nodeReceiver->Down);
      codeNl("OrcServ.PutR(");
    }
    else
      codeNl("OrcServ.Put(");
    incIndent();
    if (node->SynType == STsysObId)
      codeNl("true,");
    else
      codeNl("false,");

    /* object id: */
    constOrVar("OID",node->Down);

    /* object value (optional): */
    node = node->Right;
    codeNl(",");
    if (node && (node->SynType == STobVal))
      constOrVar("VAL",node->Down);
    else
      code("0,0");

    /* possibly a receiver or receiver list is specified: */
    if (receiversSpecified) {
      codeNl(",");
      if (nodeReceiver->SynType == STsysOrgDest)
        code("true,");
      else
        code("false,");
      receivers();
      codeNl(");");
      decIndent();
      if (nodeReceiver->Down->SynType != STexpression)
        codeNl("((TRCVchain*)ASN1pp.IPCMem->RCVchain)->Destroy();");
      node = nodeReceiver->Right;
    }
    else {
      codeNl(");");
      decIndent();
      node = nodeReceiver;
    }
  }
  while (node); /* further objects to be sent */
}


void CoderCLASS::putPrivMacro (TreeNodePtr node)

{
  do {
    setAuxVar("OID",node->Down);
    if (node->Right && (node->Right->SynType == STobVal)) {
      setAuxVar("VAL",node->Right->Down);
      if (node->Right->Flags.Contains(Encrypt))
    codeNl("ASN1pp.IPCMem->PUTencrypted = 1;");
  }

    codeNl("OrcServ.PutPriv(");
    incIndent();
    if (node->SynType == STsysObId)
      codeNl("true,");
    else
      codeNl("false,");

    /* object id: */
    constOrVar("OID",node->Down);

    /* object value (optional): */
    node = node->Right;
    codeNl(",");
    code(" ");
    if (node && (node->SynType == STobVal)) {
      constOrVar("VAL",node->Down);
      node = node->Right;
    }
    else
      code("0,0");
    codeNl(");");
    decIndent();
  }
  while (node); /* further private objects to be put */
}


void CoderCLASS::returnMacro (TreeNodePtr node)

{
  if (!node->Atomic)
    do {
      setAuxVar("OID",node->Down);
      if (node->Right && (node->Right->SynType == STobVal)) {
        setAuxVar("VAL",node->Right->Down);
    if (node->Right->Flags.Contains(Encrypt))
      codeNl("ASN1pp.IPCMem->PUTencrypted = 1;");
     }

      codeNl("OrcServ.PutReturn(");
      incIndent();
      if (node->SynType == STsysObId)
        codeNl("true,");
      else
        codeNl("false,");

      /* object id: */
      constOrVar("OID",node->Down);

      /* object value (optional): */
      node = node->Right;
      codeNl(",");
      code(" ");
      if (node && (node->SynType == STobVal)) {
        constOrVar("VAL",node->Down);
        node = node->Right;
      }
      else
        code("0,0");
      codeNl(");");
      decIndent();
    }
    while (node); /* further objects to be put */
  codeNl("OrcServ.Return(false,false);");
}


void CoderCLASS::showMacro (TreeNodePtr node)

{
  TreeNodePtr formNode, suffixNode, typeNode;
  TypeTablePtr entryFound;
  bool encrypt=false;

  if (node->Flags.Contains(Encrypt))
  encrypt = true;
  node = node->Down;
  
  do {
    typeNode = node->Down;
    formNode = node->Down->Right;

    if (formNode->Atomic) {
      code("FIOpp.CurrentRequest->data.PrefixText = \"");
      codeVariable(formNode->StringBufferPos);
      codeNl("\";");
      formNode = formNode->Right;
    }

    suffixNode = formNode->Right;
    if (suffixNode) {
      code("FIOpp.CurrentRequest->data.SuffixText = \"");
      codeVariable(suffixNode->StringBufferPos);
      codeNl("\";");
    }

    code("FIOpp.FShow(");
    indentHere(savedIndent);
    IntCode.LookUpType(typeNode->StringBufferPos,entryFound);
    code("\"");
    if (entryFound->module.Defined())
      code(entryFound->module);
    else
      code(CmdParms.ModuleName); /* syntax/module name */
    code("\",\"");

    codeVariable(typeNode->StringBufferPos); /* type name */
    code("\",");
    nlCode("(address)&");
    TypCoder.Expression(formNode->Down); /* form variable name */
    code(",FIO");
    codeVariable(typeNode->StringBufferPos); /* FIOtype name */
    code(",");
    if (entryFound->procGenAttributes.Contains(HasSIG)) {
      code("SIG");
      if (entryFound->SIGfrom)
        codeVariable(entryFound->SIGfrom->id); /* SIGtype name */
      else
        codeVariable(typeNode->StringBufferPos); /* SIGtype name */
      codeNl(",");
    }
    else codeNl("0,");

    node = node->Right;
    if (node && (node->SynType == STerror)) {
      if (node->Down->Atomic) {
        code("\"");
        codeVariable(node->Down->StringBufferPos);
        code("\"");
      }
      else
        TypCoder.Expression(node->Down->Down);
      node = node->Right;
    }
    else
      code("\"\"");
    code(",");

    if (node && (node->SynType == STexpression)) {
      code("(address)");
      TypCoder.Expression(node->Down); /* put cursor on this field */
      code(",");
      node = node->Right;
      if (node && (node->SynType == STexpression)) {
        TypCoder.Expression(node->Down); /* cursor pos in field */
        node = node->Right;
      }
      else
        code("-1");
    }
    else
      code("0,0");
    codeNl(");");
    restoreIndent(savedIndent);
  }
  while (node);

  if (encrypt)
  codeNl("ASN1pp.IPCMem->PUTencrypted = 1;");
  codeNl("FIOpp.Show();");
}


void CoderCLASS::startMacro (TreeNodePtr node)

{
  TreeNodePtr nodeVersion, nodeReceiver, orgUnitNode;
  bool receiversSpecified;

  setAuxVar("OID",node->Down);

  nodeVersion = 0;
  if (node->Right && (node->Right->SynType == STversion))
    nodeVersion = node->Right;
  if (nodeVersion)
    setAuxVar("VAL",nodeVersion->Down);

  orgUnitNode = 0;
  if (!nodeVersion) {
    if (node->Right && (node->Right->SynType == STowner))
      orgUnitNode = node->Right->Down;
  }
  else if (nodeVersion->Right && (nodeVersion->Right->SynType == STowner))
    orgUnitNode = nodeVersion->Right->Down;
  if (orgUnitNode && orgUnitNode->Atomic)
    buildOrgUnitConst(orgUnitNode);

  codeNl("OrcServ.RunApplication(");
  incIndent();
  constOrVar("OID",node->Down);
  codeNl(",/* APPLICATION */");
  node = node->Right;

  if (!node || (node->SynType != STversion))
    code("0,0");
  else {
    constOrVar("VAL",node->Down);
    node = node->Right;
  }
  codeNl(",/* VERSION */");

  if (!node || (node->SynType != STowner))
    code("0");
  else {
    if (orgUnitNode->Atomic)
      code("&ASN1pp.IPCMem->RCVorgUnit");
    else {
      code("(address)&");
      TypCoder.Expression(orgUnitNode->Down);
    }
    node = node->Right;
  }
  codeNl(",/* OWNER */");
  if (node)
    codeNl("true); /* MORE */");
  else
    codeNl("false); /* MORE */");
  decIndent();
  if (node)
    node = node->Down;
  else
    return;

  do {
    nodeReceiver = node->Right;
    if (nodeReceiver && (nodeReceiver->SynType == STobVal))
      nodeReceiver = nodeReceiver->Right;
    if (nodeReceiver
        && ((nodeReceiver->SynType == STorgDest)
            || (nodeReceiver->SynType == STsysOrgDest)))
      receiversSpecified = true;
    else
      receiversSpecified = false;

    setAuxVar("OID",node->Down);
    if (node->Right && (node->Right->SynType == STobVal))
      setAuxVar("VAL",node->Right->Down);

    if (receiversSpecified) {
      buildReceiverChain(nodeReceiver->Down);
      codeNl("OrcServ.PutStartObjR(");
    }
    else
      codeNl("OrcServ.PutStartObj(");
    incIndent();
    if (node->SynType == STsysObId)
      codeNl("true,/* SYSOID */");
    else
      codeNl("false,/* SYSOID */");

    /* object id: */
    constOrVar("OID",node->Down);

    /* object value (optional): */
    node = node->Right;
    codeNl(",");
    code(" ");
    if (node && (node->SynType == STobVal))
      constOrVar("VAL",node->Down);
    else
      code("0,0");

    /* possibly a receiver or receiver list is specified: */
    if (receiversSpecified) {
      codeNl(",");
      if (nodeReceiver->SynType == STsysOrgDest)
        codeNl("true,/* SYSORG */");
      else
        codeNl("false,/* SYSORG */");
      receivers();
      codeNl(",/* RECEIVERS */");
      if (nodeReceiver->Right)
        codeNl("true); /* MORE */");
      else
        codeNl("false); /* MORE */");
      decIndent();
      if (nodeReceiver->Down->SynType != STexpression)
        code("((TRCVchain*)ASN1pp.IPCMem->RCVchain)->Destroy();");
      node = nodeReceiver->Right;
    }
    else {
      codeNl(",");
      if (nodeReceiver)
        codeNl("true); /* MORE */");
      else
        codeNl("false); /* MORE */");
      decIndent();
      node = nodeReceiver;
    }
  }
  while (node); /* further objects to be sent */
}


void CoderCLASS::stopMacro ()

{
  codeNl("OrcServ.Stop();");
}


/************************************************************************/


bool CoderCLASS::notYetIncluded (const DString& module)

{
  if (included.AppendIfMissing(module)) return true;
  else return false;
}


void CoderCLASS::appendImportIf (TGlobalFlag used,
         const char *module)

{
  DString mod(module);

  if (IntCode.GlobalProperties.Flags.Contains(used)
      && notYetIncluded(mod)) {
    if (firstImport) {
      writeLn();
      firstImport = false;
    }
    code("#include \"");
    code(module);
    codeNl(".h\"");
  }
}


void CoderCLASS::fixedImports ()

{
  if (!(IntCode.GlobalProperties.Flags.Contains(IncludeFile)
        && IntCode.ImplModIsCoded)) {
    codeNl("#include \"SYSTEM.h\"");
    codeNl("#include \"AnyType.h\"");
  }

  if ((IntCode.GlobalProperties.Flags
       *SET(CDPgen,FIOgen,SIGNgen,
            DateTimeUsed,DTypesUsed,OrcServUsed,STRINGUsed,
      CHAINUsed,CHAINANYUsed,NESTEDANYUsed,-1))
      != SET()) {
    if (IntCode.ImplModIsCoded) {
//      IntCode.GlobalProperties.Flags.INCL(ASN1ppUsed); /* for OID... */
      if (IntCode.GlobalProperties.Flags.Contains(OrcServUsed))
  IntCode.GlobalProperties.Flags.INCL(CDPgen); /* for Cdp... */
    }
    firstImport = true;
    IntCode.GlobalProperties.Flags.INCL(ASN1ppUsed); /* for OID... */
    appendImportIf(ASN1ppUsed,"ASN1pp"); /* for ConversionProc */
    appendImportIf(CHAINUsed,"CHAIN");
    appendImportIf(CHAINANYUsed,"CHAINANY");
    appendImportIf(NESTEDANYUsed,"NESTANY");
    appendImportIf(DateTimeUsed,"DateTime"); /* for CDPDATE_, CDPTIME_, ... */
    appendImportIf(SETUsed,"SETpp");
    appendImportIf(STRINGUsed,"STR");
    appendImportIf(SIGNgen,"SIGtyp"); /* for SIGtypes.FIOSigCerts */
    if (IntCode.ImplModIsCoded) {
      appendImportIf(CDPgen,"CDPpp"); /* for Cvt... */
      appendImportIf(DTypesUsed,"DTypes");
      appendImportIf(FIOgen,"FIOpp");
      appendImportIf(OrcServUsed,"OrcServ");
      appendImportIf(SIGNgen,"AFM"); /* for AFM.CertList */
      appendImportIf(SIGNgen,"SIGpp"); /* for SIGpp.Entry, SIGpp.Exit, ... */
    }
  }
}


void CoderCLASS::imports (TreeNodePtr& node)

{
  DString str(100);
  
  for (; node;) {
    if (node->Atom == PPescape)
      if (node->Right->SynType == STinclude) {
        if (modGen
        && !(IntCode.GlobalProperties.Flags.Contains(IncludeFile)
       && IntCode.ImplModIsCoded)) {
          copyText(node,false);
          indentHere(savedIndent);
        }
        node = node->Right;

        DString mod(&StringBuffer[node->Down->StringBufferPos]);
        if (modGen
        && !(IntCode.GlobalProperties.Flags.Contains(IncludeFile)
       && IntCode.ImplModIsCoded)
  && notYetIncluded(mod)) {
    code("#include \"");
    str = DString(&StringBuffer[node->Down->StringBufferPos]);
    str.Delete(str.l-2,1);
    code(str);
    codeNl("\"");
  }
  skipIncludeMacro(node->Down);
      }
      else
        return;
    else
      return;
    do node = node->Right;
    while (node && node->Flags.Contains(Included));
  }
}


void CoderCLASS::constOrVar (DString oidVal,
                             TreeNodePtr node)

{
  code("(address)&");
  if (node->Atomic) {
    code("ASN1pp.IPCMem->");
    code(oidVal);
    switch (node->Atom) {

    case True:
    case False:
      code("bool,CDPboolean");
      break;

    case Minus:
      code("int,CDPint");
      break;

    case Plus:
    case IntConst:
    case OctalConst:
    case HexConst:
      code("unsigned,CDPunsigned");
      break;

    case StringConst:
      code("string,CDPSTRING");
      break;
    }
  }
  else {
    node = node->Down;
    TypCoder.Expression(node->Right->Down);
    code(",");
    code("CDP");
    codeAtom(node);
  }
}


void CoderCLASS::setAuxVar (DString oidVal,
                            TreeNodePtr node)

{
  if (!node->Atomic) return;
  
  code("ASN1pp.IPCMem->");
  code(oidVal);
  switch (node->Atom) {

  case False:
  case True:
    code("bool");
    break;

  case Minus:
    code("int");
    break;

  case Plus:
  case IntConst:
  case OctalConst:
  case HexConst:
    code("unsigned");
    break;

  case StringConst:
    code("string");
    break;
  }
  code(" = ");
  switch (node->Atom) {

  case False:
    code("false");
    break;

  case True:
    code("true");
    break;

  case Minus:
    code("-");
    node = node->Right;
    codeVariable(node->StringBufferPos);
    break;

  case Plus:
    node = node->Right;
    codeAtom(node);
    break;

  case IntConst:
  case OctalConst:
  case HexConst:
    codeAtom(node);
    break;

  case StringConst:
    code("\"");
    codeAtom(node);
    code("\"");
  }
  codeNl(";");
}


void CoderCLASS::buildReceiverChain (TreeNodePtr node)

{
  TreeNodePtr progNode, orgUnitNode, orgTypeNode;

  arbOrg = false;
  constOrg = false;
  if (node->SynType == STexpression) return;

  node = node->Down; /* first receiver */
  // codeNl("ASN1pp.IPCMem->RCVchain = 0;");
  do {
    if (node->Atomic) {
      code("OrcServ.AddReceiver(\"");
      codeVariable(node->StringBufferPos);
      codeNl("\",0,(TRCVchain*)ASN1pp.IPCMem->RCVchain);");
    }
    else
      switch (node->SynType) {

      case STsenderReceiver:
        progNode = node->Down;
        orgTypeNode = progNode->Right;
        orgUnitNode = orgTypeNode->Right;
        if (orgUnitNode->Atomic) {
          constOrg = true;
          buildOrgUnitConst(orgUnitNode);
        }
        if (progNode->Atomic)
          if (orgUnitNode->Atomic) {
            codeNl("OrcServ.AddReceiver(");
            incIndent();
            code("\"");
            codeVariable(progNode->StringBufferPos);
            codeNl("\",ASN1pp.IPCMem->RCVorgUnit,");
            codeNl("(TRCVchain*)ASN1pp.IPCMem->RCVchain);");
            decIndent();
          }
          else {
            code("OrcServ.AddReceiver");
            if (orgTypeNode) {
              arbOrg = true;
              code("ArbOrg");
            }
            codeNl("(");
            incIndent();
      code("\"");
            codeVariable(progNode->StringBufferPos);
            codeNl("\",");
      code("(address)&");
            TypCoder.Expression(orgUnitNode->Down);
      if (arbOrg) {
        code(",CDP");
        codeVariable(orgTypeNode->StringBufferPos);
        codeNl(",");
      }
      else
        codeNl(",");
            codeNl("(TRCVchain*)ASN1pp.IPCMem->RCVchain);");
            decIndent();
          }
        else
          if (orgUnitNode->Atomic) {
            codeNl("OrcServ.AddReceiver(");
            incIndent();
            TypCoder.Expression(progNode->Down);
            codeNl(",ASN1pp.IPCMem->RCVorgUnit,");
            codeNl("(TRCVchain*)ASN1pp.IPCMem->RCVchain);");
            decIndent();
          }
          else {
            code("OrcServ.AddReceiver");
            if (orgTypeNode) {
              arbOrg = true;
              code("ArbOrg");
            }
            codeNl("(");
            incIndent();
            TypCoder.Expression(progNode->Down);
            codeNl(",");
            TypCoder.Expression(orgUnitNode->Down);
            codeNl(",");
            codeNl("(TRCVchain*)ASN1pp.IPCMem->RCVchain);");
            decIndent();
          }
        break;

      case STexpression:
        code("OrcServ.AddReceiver(");
        TypCoder.Expression(node->Down);
        codeNl(",0,(TRCVchain*)ASN1pp.IPCMem->RCVchain);");
        break;
      }
    node = node->Right;
  }
  while (node);
}


void CoderCLASS::buildOrgUnitConst (TreeNodePtr orgUnitNode)

{
  code("ASN1pp.IPCMem->RCVorgUnit = \"");
  codeVariable(orgUnitNode->StringBufferPos);
  codeNl("\";");
}


void CoderCLASS::receivers ()

{
  code("(TRCVchain*)ASN1pp.IPCMem->RCVchain");
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void CoderCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  Atoms_INIT();
  CmdError.INIT();
  CmdParms.INIT();
  EC_INIT();
  IO.INIT();
  IntCode.INIT();
  PCerrors.INIT();
  TypCoder.INIT();
  CdpCoder.INIT();
  FioCoder.INIT();
  SigCoder.INIT();
//  SynCoder.INIT();
}

