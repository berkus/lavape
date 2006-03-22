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


#include "XParser.h"

#pragma hdrstop


XParserCLASS XParser;
static bool __INITstarted=false;




/* ************************************************************************/
/* executable MACROS: */


void XParserCLASS::CallMacro ()

{
  if (Atom == Call)
    IntCode.Push(STcall);
  else
    IntCode.Push(STcallSuspend);
  NextAtom();

  switch (Atom) {

  case Proc:
  case Sysproc:
    IntCode.PushAtom();
    NextAtom();
    break;

  default:
    PCerrors.Expected("PROC | SYSPROC");
    return;
  }
  Check(Assign);
  IntCode.Push(STreceiverList);
  receiver();
  IntCode.Pop();

  if (Atom == Comma) {
    NextAtom();
    inputOutputObjects();
  }

  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::CheckMacro ()

{
  IntCode.Push(STcheck);
  NextAtom();
  /* object id: */
  switch (Atom) {

  case Sysoid:
    IntCode.Push(STsysObId);
    break;

  case Oid:
    IntCode.Push(STobId);
    break;

  default:
    PCerrors.Expected("OID | SYSOID");
    return;
  }
  NextAtom();
  Check(Assign);
  constOrVar();
  IntCode.Pop();
  Check(Comma);

  /* BOOLEAN success indicator: */
  Check(Available);
  Check(Assign);
  DParser.Expression();
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::CheckpointMacro ()

{
  IntCode.Push(STcheckpoint);
  IntCode.PushEmpty();
  IntCode.Pop();
  NextAtom();
  CheckPush(Semicolon);
}


void XParserCLASS::DropMacro ()

{
  IntCode.Push(STdrop);
  do {
    NextAtom();
    Check(Lparenth);
    IntCode.RefTypeIdent();
    IntCode.CheckTypeDefined(true,true);
    Check(Rparenth);
    DParser.Expression();
  }
  while (Atom == Comma);
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::GetMacro ()

{
  if (Atom == Get)
    IntCode.Push(STget);
  else
    IntCode.Push(STsuspend);
  NextAtom();
  for (;;) {
    if (PCerrors.Recovery)
      return;

    /* object id: */
    switch (Atom) {

    case Sysoid:
      IntCode.Push(STsysObId);
      break;

    case Oid:
      IntCode.Push(STobId);
      break;

    default:
      PCerrors.Expected("OID | SYSOID");
      return;
    }
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    if (Atom == Comma) {
      NextAtom();
      if (Atom == Val) {
        IntCode.Push(STobVal);
        Check(Val);
        Check(Assign);
        variableSpec(false);
        IntCode.Pop();
        if (Atom == Comma)
          NextAtom();
        else
          goto exit;
      }
    }
    else
      goto exit;

    /* possibly the sender program name is requested: */
    if (Atom == From) {
      IntCode.Push(STorgDest);
      NextAtom();
      Check(Assign);
      if (Atom == Lparenth) {
        IntCode.Push(STsenderReceiver);
        NextAtom();
        if (Atom == Lparenth) {
          NextAtom();
          IntCode.RefTypeIdent();
          IntCode.CheckTypeDefined(true,false);
          Check(Rparenth);
        }
        else
          IntCode.GlobalProperties.Flags.INCL(DTypesUsed);
        DParser.Expression(); /* for the sender program name */
        Check(Comma);
        if (Atom == Lparenth) {
          NextAtom();
          IntCode.RefTypeIdent();
          IntCode.CheckTypeDefined(true,false);
          Check(Rparenth);
        }
        else
          IntCode.GlobalProperties.Flags.INCL(DTypesUsed);
        DParser.Expression(); /* for the sender orgunit */
        IntCode.Pop();
        Check(Rparenth);
      }
      else
        DParser.Expression();
      IntCode.Pop();
      if (Atom == Comma)
        NextAtom();
      else
        goto exit;
    }
  } exit: ;
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::GetPrivMacro ()

{
  IntCode.Push(STgetPrivate);
  NextAtom();
  for (;;) {
    if (PCerrors.Recovery)
      return;

    /* object id: */
    switch (Atom) {

    case Sysoid:
      IntCode.Push(STsysObId);
      break;

    case Oid:
      IntCode.Push(STobId);
      break;

    default:
      PCerrors.Expected("OID | SYSOID");
      return;
    }
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    Check(Comma);
    if (Atom == Val) {
      IntCode.Push(STobVal);
      NextAtom();
      Check(Assign);
      variableSpec(false);
      IntCode.Pop();
      Check(Comma);
    }

    /* BOOLEAN success indicator: */
    Check(Done);
    Check(Assign);
    DParser.Expression();
    if (Atom == Comma)
      NextAtom();
    else
      goto exit;
  } exit: ;
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::OrgunitMacro ()

{
  IntCode.Push(STorgunit);
  NextAtom();

  /* org-unit variable id: */
  IntCode.Push(STobId);
  Check(Orgvar);
  Check(Assign);
  constOrVar();
  IntCode.Pop();

  /* computed org-unit value: */
  IntCode.Push(STobVal);
  Check(Comma);
  Check(Orgval);
  Check(Assign);
  orgUnit();
  IntCode.Pop();

  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::ProcedureMacro ()

{
  bool noObj;

  IntCode.Push(STprocedure);
  NextAtom();

  noObj = true;
  for (;;) {
    if (PCerrors.Recovery)
      return;

    /* object id: */
    switch (Atom) {

    case Sysoid:
      IntCode.Push(STsysObId);
      break;

    case Oid:
      IntCode.Push(STobId);
      break;

    default:
      if (noObj)
        goto exit;
      else {
        PCerrors.Expected("OID | SYSOID");
        return;
      }
    }
    noObj = false;
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    if (Atom == Comma) {
      NextAtom();
      if (Atom == Val) {
        IntCode.Push(STobVal);
        NextAtom();
        Check(Assign);
        variableSpec(false);
        IntCode.Pop();
        if (Atom == Comma)
          NextAtom();
        else
          goto exit;
      }
    }
    else
      goto exit;
  } exit: ;

  IntCode.PushEmpty();
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::PutMacro ()

{
  NextAtom();
  putMacro(); /* called also in StartMacro */
  CheckPush(Semicolon);
}


void XParserCLASS::putMacro ()

{
  IntCode.Push(STput);
  for (;;) {
    if (PCerrors.Recovery)
      return;

    /* object id: */
    switch (Atom) {

    case Sysoid:
      IntCode.Push(STsysObId);
      break;

    case Oid:
      IntCode.Push(STobId);
      break;

    default:
      PCerrors.Expected("OID | SYSOID");
      return;
    }
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    if (Atom == Comma) {
      NextAtom();
      if ((Atom == Val) || (Atom == ValEnc)) {
        IntCode.Push(STobVal);
        if (Atom == ValEnc)
          IntCode.TopOfStack()->Flags.INCL(Encrypt);
        NextAtom();
        Check(Assign);
        constOrVar();
        IntCode.Pop();
        if (Atom == Comma)
          NextAtom();
        else
          goto exit;
      }
    }
    else
      goto exit;

    /* possibly a receiver or receiver list is specified: */
    if ((Atom == FOR)
        || (Atom == Forsys)) {
      if (Atom == FOR)
        IntCode.Push(STorgDest);
      else
        IntCode.Push(STsysOrgDest);
      NextAtom();
      Check(Assign);
      receivers();
      IntCode.Pop();
      if (Atom == Comma)
        NextAtom();
      else
        goto exit;
    }
  } exit: ;
  IntCode.Pop();
}


void XParserCLASS::PutPrivMacro ()

{
  IntCode.Push(STputPrivate);
  NextAtom();
  for (;;) {
    if (PCerrors.Recovery)
      return;

    /* object id: */
    switch (Atom) {

    case Sysoid:
      IntCode.Push(STsysObId);
      break;

    case Oid:
      IntCode.Push(STobId);
      break;

    default:
      PCerrors.Expected("OID | SYSOID");
      return;
    }
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    if (Atom == Comma) {
      NextAtom();
      if ((Atom == Val) || (Atom == ValEnc)) {
        IntCode.Push(STobVal);
        if (Atom == ValEnc)
          IntCode.TopOfStack()->Flags.INCL(Encrypt);
        NextAtom();
        Check(Assign);
        constOrVar();
        IntCode.Pop();
        if (Atom == Comma)
          NextAtom();
        else
          goto exit;
      }
    }
    else
      goto exit;
  } exit: ;
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::ReturnMacro ()

{
  bool noObj;

  IntCode.Push(STreturn);
  NextAtom();

  noObj = true;
  for (;;) {
    if (PCerrors.Recovery)
      return;
    switch (Atom) {

    case Sysoid:
      IntCode.Push(STsysObId);
      break;

    case Oid:
      IntCode.Push(STobId);
      break;

    default:
      if (noObj)
        goto exit;
      else {
        PCerrors.Expected("OID | SYSOID");
        return;
      }
    }
    noObj = false;
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    if (Atom == Comma) {
      NextAtom();
      if ((Atom == Val) || (Atom == ValEnc)) {
        IntCode.Push(STobVal);
        if (Atom == ValEnc)
          IntCode.TopOfStack()->Flags.INCL(Encrypt);
        NextAtom();
        Check(Assign);
        constOrVar();
        IntCode.Pop();
        if (Atom == Comma)
          NextAtom();
        else
          goto exit;
      }
    }
    else
      goto exit;
  } exit: ;

  IntCode.PushEmpty();
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::ShowMacro ()

{
  IntCode.Push(STshow);
  if (Atom == ShowEnc)
  IntCode.TopOfStack()->Flags.INCL(Encrypt);
  NextAtom();
  for (;;) {
    IntCode.Push(STshowForm);
      if (Atom == Form) {
  NextAtom();
  Check(Assign);
      }
      Check(Lparenth);
      IntCode.RefTypeIdent();
      IntCode.CheckTypeDefined(true,true);
      Check(Rparenth);
      if (Atom == StringConst) {
        IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
  IntCode.PushAtom();
  NextAtom();
      }
      DParser.Expression();
      if (Atom == StringConst) {
        IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
  IntCode.PushAtom();
  NextAtom();
      }
    IntCode.Pop();

    if (Atom == Comma) {
      NextAtom();
      if (Atom == ErrorText) {
        NextAtom();
        Check(Assign);
        IntCode.Push(STerror);
        switch (Atom) {

        case StringConst:
          IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
          IntCode.PushAtom();
          NextAtom();
          break;

        case IdentifierA:
          DParser.Expression();
          break;

        default:
          PCerrors.Expected("designator | string constant");
          return;
        }
        IntCode.Pop();
        if (Atom != Comma)
          goto exit;
        NextAtom();
      }
      if (Atom == Cursor) {
        NextAtom();
        Check(Assign);
        if (Atom == Lparenth) {
          NextAtom();
          DParser.Expression();
          Check(Comma);
          DParser.Expression();
          Check(Rparenth);
        }
        else
          DParser.Expression();
        if (Atom == Comma)
          NextAtom();
        else
          goto exit;
      }
    }
    else
      goto exit;
  } exit: ;
  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::StartMacro ()

{
  IntCode.Push(STstart);
  NextAtom();

  /* application id: */
  IntCode.Push(STapplId);
  Check(ApplicationClass);
  Check(Assign);
  constOrVar();
  IntCode.Pop();

  if (Atom != Comma) {
    IntCode.Pop();
    CheckPush(Semicolon);
    return;
  }
  else
    NextAtom();

  if (Atom == Version) {
    IntCode.Push(STversion);
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();
    if (Atom != Comma) {
      IntCode.Pop();
      CheckPush(Semicolon);
      return;
    }
    else
      NextAtom();
  }

  if (Atom == Owner) {
    IntCode.Push(STowner);
    NextAtom();
    Check(Assign);
    orgUnit();
    IntCode.Pop();
    if (Atom != Comma) {
      IntCode.Pop();
      CheckPush(Semicolon);
      return;
    }
    else
      NextAtom();
  }

  switch (Atom) {

  case Sysoid:
  case Oid:
    putMacro();
    break;

  default:
    ;
  }

  IntCode.Pop();
  CheckPush(Semicolon);
}


void XParserCLASS::StopMacro ()

{
  IntCode.Push(STstop);
  IntCode.PushEmpty();
  IntCode.Pop();
  NextAtom();
  CheckPush(Semicolon);
}


/* ************************************************************************/
/* auxiliary parsing procedures: */


void XParserCLASS::inputOutputObjects ()

{
  bool noInputObj, noOutputObj;

  noInputObj = true;
  noOutputObj = true;
  for (;;) {
    if (PCerrors.Recovery)
      return;
    switch (Atom) {

    case Inoid:
      if (noInputObj) {
        IntCode.Push(STinputObjects);
        noInputObj = false;
      }
      IntCode.Push(STobId);
      break;

    case Insysoid:
      if (noInputObj) {
        IntCode.Push(STinputObjects);
        noInputObj = false;
      }
      IntCode.Push(STsysObId);
      break;

    default:
      if (!noInputObj)
        IntCode.Pop();
      goto exit;
    }
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    if (Atom == Comma) {
      NextAtom();
      if ((Atom == Inval) || (Atom == InvalEnc)) {
        IntCode.Push(STobVal);
        if (Atom == InvalEnc)
          IntCode.TopOfStack()->Flags.INCL(Encrypt);
        NextAtom();
        Check(Assign);
        constOrVar();
        IntCode.Pop();
        if (Atom == Comma)
          NextAtom();
        else {
          IntCode.Pop();
          return;
        }
      }
    }
    else {
      IntCode.Pop();
      return;
    }
  } exit: ;

  for (;;) {
    if (PCerrors.Recovery)
      return;
    switch (Atom) {

    case Outoid:
      if (noOutputObj) {
        IntCode.Push(SToutputObjects);
        noOutputObj = false;
      }
      IntCode.Push(STobId);
      break;

    case Outsysoid:
      if (noOutputObj) {
        IntCode.Push(SToutputObjects);
        noOutputObj = false;
      }
      IntCode.Push(STsysObId);
      break;

    default:
      if (noOutputObj)
        PCerrors.Expected("INOID | INSYSOID | OUTOID | OUTSYSOID");
      else
        PCerrors.Expected("OUTOID | OUTSYSOID");
      return;
    }
    NextAtom();
    Check(Assign);
    constOrVar();
    IntCode.Pop();

    /* object value (optional): */
    if (Atom == Comma) {
      NextAtom();
    if (Atom == Outval) {
    IntCode.Push(STobVal);
        NextAtom();
        Check(Assign);
     variableSpec(false);
        IntCode.Pop();
        if (Atom == Comma)
          NextAtom();
        else {
          IntCode.Pop();
          break;
        }
      }
    }
    else {
      IntCode.Pop();
      break;
    }
  }
}


void XParserCLASS::orgUnit ()

{
  if (Atom == StringConst) {
    IntCode.PushAtom();
    NextAtom();
    IntCode.GlobalProperties.Flags.INCL(DTypesUsed);
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
  }
  else {
    if (Atom == Lparenth) {
      NextAtom();
      IntCode.RefTypeIdent();
      IntCode.CheckTypeDefined(true,false);
      Check(Rparenth);
    }
    else
      IntCode.GlobalProperties.Flags.INCL(DTypesUsed);
    DParser.Expression();
  }
}


void XParserCLASS::receiver ()

{
  switch (Atom) {

  case Lparenth:
    IntCode.Push(STsenderReceiver);
    NextAtom();
    switch (Atom) {

    case StringConst:
      IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
      IntCode.PushAtom();
      NextAtom();
      break;

    case IdentifierA:
      DParser.Expression();
      break;

    default:
      PCerrors.Expected("stringConst | identifier");
    }
    Check(Comma);
    orgUnit();
    Check(Rparenth);
    IntCode.Pop();
    break;

  case StringConst:
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    IntCode.PushAtom();
    NextAtom();
    break;

  case IdentifierA:
    DParser.Expression();
    break;

  default:
    PCerrors.Expected("identifier | string constant | \"(\"");
  }
}


void XParserCLASS::receivers ()

{
  switch (Atom) {

  case IdentifierA:
    DParser.Expression();
    break;

  case Lbrace:
    IntCode.Push(STreceiverList);
    NextAtom();
    for (;;) {
      receiver();
      if (Atom == Comma)
        NextAtom();
      else
        goto exit;
    } exit: ;
    Check(Rbrace);
    IntCode.Pop();
    break;

  default:
    PCerrors.Expected("identifier | \"{\"");
  }
}


void XParserCLASS::constOrVar ()

{
  switch (Atom) {

  case Plus:
  case Minus:
    IntCode.PushAtom();
    NextAtom();
    switch (Atom) {

    case OctalConst:
    case IntConst:
    case HexConst:
      IntCode.PushAtom();
      NextAtom();
      break;

    default:
      PCerrors.Expected("number");
      return;
    }
    break;

  case False:
  case True:
  case OctalConst:
  case IntConst:
  case HexConst:
    IntCode.PushAtom();
    NextAtom();
    break;

  case StringConst:
    IntCode.GlobalProperties.Flags.INCL(STRINGUsed);
    IntCode.PushAtom();
    NextAtom();
    break;

  case Lparenth:
    variableSpec(false);
    break;

  default:
    PCerrors.Expected("integer/string/bool constant | identifier");
    return;
  }
}


void XParserCLASS::variableSpec (bool shouldBeFormType)

{
  IntCode.Push(STvariableSpec);
  Check(Lparenth);
  switch (Atom) {

  case BooleanA:
  case ByteA:
  case Unsigned:
  case CharA:
  case Int:
    IntCode.PushAtom();
    NextAtom();
    break;

  case IdentifierA:
    IntCode.RefTypeIdent();
    IntCode.CheckTypeDefined(true,shouldBeFormType);
    break;
  }
  Check(Rparenth);
  
  DParser.Expression(); // designator
  IntCode.Pop();
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void XParserCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  ContextCK.INIT();
  DParser.INIT();
  Scanner_INIT();
  Atoms_INIT();
  IntCode.INIT();
  PCerrors.INIT();
}
