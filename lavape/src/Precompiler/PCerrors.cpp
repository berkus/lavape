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

               implementation of class PCerrorsCLASS

 **********************************************************************/

#include "PCerrors.h"


PCerrorsCLASS PCerrors;
static bool __INITstarted=false;

/************************************************************************/

void PCerrorsCLASS::setError (bool isContextError)
{
  if (ErrorCount == 0) {
    IO.Remove(CmdParms.OutputFile);
    if (IntCode.GlobalProperties.Flags.Contains(IncludeFile))
      IO.Remove(CmdParms.OutputFileImplMod);
  }
  if (ErrorCount >= maxErrors) {
    IO.WriteString("+++++ too many errors, pre-processing stopped");
    IO.WriteLn();
    exit(1);
  }
  ErrorCount++;
  if (!isContextError) {
    Recovery = true;
    IntCode.CurrentDefType = 0;
    IntCode.CurrentRefType = 0;
  }

//  IO.WriteString("++++ pp: ");
  if (CmdParms.CurrentIncludeFile.Length() == 0)
    IO.WriteString(CmdParms.SourceFile);
  else  
    IO.WriteString(CmdParms.CurrentIncludeFile);
  IO.WriteString(":");
  IO.WriteCard(AtomLine,1);
  IO.WriteString(":");
  IO.WriteCard(AtomCol,1);
  IO.WriteString(": ");
//  IO.WriteLn();
//  IO.WriteString("         ");
}


void PCerrorsCLASS::LexicalError (LexErrType lexErr)

{
  if ((lexErr == OpenComment)
      || (lexErr == StringExceedsLine)) {
    AtomLine = CommentLine;
    AtomCol = CommentCol;
  }
  setError(false);
//  IO.WriteString(" L ");
  switch (lexErr) {

  case EmptyString:
    IO.WriteString("empty strings are not allowed");
    break;

  case IllegalChar:
    IO.WriteString("illegal character");
    break;

  case MissingExponent:
    IO.WriteString("missing exponent of real number");
    break;

  case OpenComment:
    IO.WriteString("comment is not terminated by \"*)\"");
    IO.WriteLn();
    exit(1);
    break;

  case StringExceedsLine:
    IO.WriteString("string is not terminated on the same line");
    break;

  case WrongDecimal:
    IO.WriteString("decimal number contains hexadecimal digits");
    break;

  case WrongHexadecimal:
    IO.WriteString("\"H\" missing after hexadecimal number");
    break;

  case WrongOctal:
    IO.WriteString("octal number contains non-octal digits");
    break;

  case WrongExponent:
    IO.WriteString("incomplete exponent of floating point number");
    break;
  }
  IO.WriteLn();
}


void PCerrorsCLASS::Expected (const char * const s)

{
  if (Recovery) return;
  /* error reporting is suppressed until completion of recovery */
  setError(false);
//  IO.WriteString(" E ");
  IO.WriteString(s);
  IO.WriteString(" expected");
  IO.WriteLn();
}


void PCerrorsCLASS::NotYetImplemented ()

{
  if (Recovery) return;
  /* error reporting is suppressed until completion of recovery */
  setError(false);
//  IO.WriteString(" N ");
  IO.WriteLn();
}


void PCerrorsCLASS::InternalError (InternalErrType intErr)

{
  IO.CloseOutput();
  IO.Remove(CmdParms.OutputFile);
  if (IntCode.GlobalProperties.Flags.Contains(IncludeFile))
    IO.Remove(CmdParms.OutputFileImplMod);
  IO.WriteString("++++ internal error: ");
  switch (intErr) {

  case InputBufferOfl:
    IO.WriteString("Coder: input line buffer too short");
    break;

  case StackOverflow:
    IO.WriteString("IntCode: stack overflow");
    break;

  case StringBufferOverflow:
    IO.WriteString("Scanner: string buffer overflow");
    break;

  case StackUnderflow:
    IO.WriteString("IntCode: stack underflow");
    break;

  case IOppError:
    IO.WriteString("WriteSynDefFile: IOpp error");
    break;

  case SynIOError:
    IO.WriteString("WriteSynDefFile: SynIO error");
    break;
  }
  IO.WriteLn();
  PMDump.PostMortemDump();
}


void PCerrorsCLASS::ContextError (ContextErrType codeErr,
                                  unsigned line,
                                  unsigned col)

{
  AfterContextError = true;
  if ((line == lastErrLine)
      && (col == lastErrCol))
    return;
  lastErrLine = line;
  AtomLine = line;
  lastErrCol = col;
  AtomCol = col;
  setError(true);
//  IO.WriteString(" C ");
  switch (codeErr) {

  case DataLengthError:
    IO.WriteString("data length greater than field length");
    break;

  case FieldLength0:
    IO.WriteString("field length = 0 forbidden");
    break;

  case DataLength0:
    IO.WriteString("bound for data length = 0 or 0- forbidden");
    break;

  case DuplTypeDef:
    IO.WriteString("type has already been defined above");
    break;

  case DuplTypeDefSpec:
    IO.WriteString("duplicate struct/class/enum/typedef in type specifier");
    break;

  case DuplTypeSpec:
    IO.WriteString("a type has already been specified in this type specifier");
    break;

  case ForbiddenQualRef:
    IO.WriteString("qualified reference inadmissible for PC-defined types");
    break;

  case InFormsOnly:
    IO.WriteString("allowed in forms only");
    break;

  case NoFormType:
    IO.WriteString("type is used but not defined as a $FORM type");
    break;

  case BaseTypesOnly:
    IO.WriteString("don't use imported variant types in base type declarations");
    break;

  case NotAllowed:
    IO.WriteString("not allowed in this context");
    break;

  case NotAllowedInForm:
    IO.WriteString("not allowed in forms");
    break;

  case NotSupported:
    IO.WriteString("presently not supported");
    break;

  case OIDTooLong:
    IO.WriteString("object identifier too long (max. =");
    break;

  case OnTopLevelOnly:
    IO.WriteString("not allowed within a containing type descriptor");
    break;

  case NotOnTopLevel:
    IO.WriteString(
      "allowed only in member lists of classes");
    break;

  case CLincompatTF:
    IO.WriteString("case label incompatible with tag field type");
    break;

  case SingleCharExpected:
    IO.WriteString("a case label cannot be a string of length # 1");
    break;

  case TFNotItem:
    IO.WriteString("does not match any item of the tag field enumeration type");
    break;

  case UndefinedType:
    IO.WriteString("type not yet defined");
    break;

  case NoLocalType:
    IO.WriteString("imported type not allowed as array index type");
    break;

  case IncompBaseType:
    IO.WriteString("incompatible with the specified base type");
    break;

  case MissingCont:
    IO.WriteString("the specified base type has more such items hereafter");
    break;

  case VALTooLong:
    IO.WriteString("object value too long (max. =");
    break;
  }
  IO.WriteLn();
}


void PCerrorsCLASS::Missing (TStringBufferPos objId)

{
  setError(false);
//  IO.WriteString(" M ");
    IO.WriteString("type \"");
  do {
    IO.Write(StringBuffer[objId]);
    objId++;
  }
  while (!(StringBuffer[objId] == '\0'));
  IO.WriteString("\" undefined");
  IO.WriteLn();
}


/**********************************************************************/
/*           class constructor (the former "module body"):            */
/**********************************************************************/

void PCerrorsCLASS::INIT ()
{
  if (__INITstarted) return;
  __INITstarted = true;

  Atoms_INIT();
  IO.INIT();
  CmdParms.INIT();
  IntCode.INIT();

  ErrorCount = 0;
  Recovery = false; /* no error recovery pending */
  AfterContextError = false;
  lastErrLine = 0;
  lastErrCol = 0;

}
