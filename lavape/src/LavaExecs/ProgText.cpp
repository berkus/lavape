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


#include "Constructs.h"
#include "ExecView.h"
#include "Convert.h"
#include "DIO.h"
#include "qstring.h"
#include "qcolor.h"
#include "qfont.h"
#include <locale.h>

#pragma hdrstop


static inline void code (const char *text)
/* put constant text code */

{
  IO.WriteString(text);
}


static inline void codeNl (const char *text)
/* put constant text code and then new line */

{
  IO.WriteString(text);
  IO.WriteLn();
}


static void PutLink(TIDTable *IDTable,TID refID,bool singleFile,bool error,char *refName,bool opt=false)
{
  unsigned loc=0;
  bool linkDefined=false, isFuncRef=false;
  TIDType idtype;
  DWORD dw;
  DString importedFile, nINCLstring, nIDstring, drefName(refName);
  LavaDECL *decl;
  SynObject *synObj;

  if (refID.nID >= 0) {
    dw = IDTable->GetVar(refID,idtype);
    if (dw) {
      linkDefined = true;
      if (idtype == globalID) {
        decl = *(LavaDECL**)dw;
        if (decl->DeclType == Function)
          isFuncRef = true;
      }
      else {
        synObj = (SynObject*)dw;
        if (!synObj->parentObject) { // SelfVar
          idtype = globalID;
          refID = ((SelfVar*)synObj)->typeID;
        }
      }
    }
  }

  if (/*!singleFile && */linkDefined) {
    code("<a href=\"");
    if (idtype == globalID) {
      if (refID.nINCL != 1) { // other Lava doc != std.lava
        importedFile = IDTable->GetRelSynFileName(refID);
        importedFile.Contains(".lava",0,loc);
        importedFile.Delete(loc,5);
        while (importedFile.Contains("\\",loc,loc))
          importedFile.Replace("/",loc);
        loc = 0;
        if (importedFile.Contains(":",0,loc))
          importedFile.Replace("|",loc);
        if (singleFile)
          importedFile = importedFile + "_1.htm";
        else
          importedFile = "../" + importedFile + "_m.htm";
      }
      else // std.lava
        if (singleFile)
          importedFile = "std_1.htm";
        else
          importedFile = "../std_m.htm";
      code(importedFile.c);
    }

    code("#T");
    IO.WriteCard(refID.nID,1);
    code("\"");
    if (idtype == globalID && !singleFile)
      code(" target=\"frmLeft\">");
    else
      code(">");
  }

  if (isFuncRef || error)
    code("<b>");
  if (error)
    code("<span class=\"red\">");
  else if (opt)
    code("<span class=\"pink\">");
  else
    code("<span class=\"black\">");

  if (refName[0] == '<') {
    drefName = drefName.Substr(1,0);
    code("&lt;");
  }
  code(drefName.c);

  code("</span>");
  if (isFuncRef || error)
    code("</b>");

  if (/*!singleFile && */linkDefined)
    code("</a>");
}

void CProgText::Insert (TToken token,bool isPrimToken,bool isOptl) {
  CHETokenNode *newChel;
  QString str;
  SynObject *currentObj, *ancestor;
  LavaDECL *formParmDecl;
  unsigned start=0, loc;

  newChel = new CHETokenNode;
  newChel->data.token = token;
  tokenChain.Insert(currentToken,newChel);
  currentToken = newChel;

  switch (token) {
  case VarName_T:
    if (currentSynObj->flags.Contains(isClosed))
      currentToken->data.str = QString("(") + ((VarName*)currentSynObj)->varName.c + QString(")");
    else
      currentToken->data.str = ((VarName*)currentSynObj)->varName.c;
    break;
  case TDOD_T:
    if (currentSynObj->parentObject->parentObject->primaryToken == FormParm_T
    && currentSynObj->parentObject->flags.Contains(isClosed))
      currentToken->data.str = QString("(") + ((TDOD*)currentSynObj)->name.c + QString(")");
    else
      currentToken->data.str = ((TDOD*)currentSynObj)->name.c;
    break;
  case Const_T:
    currentToken->data.str = ((Constant*)currentSynObj)->str.c;
    break;
  case FuncRef_T:
    if (currentSynObj->parentObject->primaryToken == initializer_T
    || currentSynObj->parentObject->primaryToken == dftInitializer_T
    || currentSynObj->parentObject->primaryToken == function_T)
      currentToken->data.str = ((Reference*)currentSynObj)->refDecl->FullName.c;
    else
      currentToken->data.str = ((Reference*)currentSynObj)->refName.c;
    break;
  case parameter_T:
    formParmDecl = document->IDTable.GetDECL(((Parameter*)currentSynObj)->formParmID,ckd.inINCL);
    currentToken->data.str = QString(formParmDecl->LocalName.c);
    break;
  case TypeRef_T:
  case CrtblRef_T:
    currentToken->data.str = ((Reference*)currentSynObj)->refName.c;
    break;
  case enumConst_T:
    currentToken->data.str = ((EnumConst*)currentSynObj)->Id.c;
    break;
  case Comment_T:
    currentToken->data.str = currentSynObj->comment.ptr->str.c;
    while ((loc=currentToken->data.str.indexOf("\r",start)) != -1) {
      currentToken->data.str.remove(loc,1);
      start = loc;
    }
    start = 0;
    break;
  default:
    currentToken->data.str = TOKENSTR[token];
  }

  currentToken->data.synObject = currentSynObj;

  if (insBlank) {
    currentToken->data.flags.INCL(insertBlank);
    insBlank = false;
  }

  if (!currentSynObj->startToken && token != NewLineSym_T) {
    for (currentObj = currentSynObj;
         currentObj && !currentObj->startToken;
         currentObj = currentObj->parentObject)
      currentObj->startToken = currentToken;
  }

  currentToken->data.newLines = newLines;
  if (newLines) {
    ancestor = currentSynObj;
    while (ancestor && ancestor->startToken == currentToken)
      ancestor = ancestor->parentObject;
    currentToken->data.indent = ancestor->currentIndent;
    newLines = 0;
  }

  if (isOptl)
    currentToken->data.flags.INCL(isOpt);
  if (ignored)
    currentToken->data.flags.INCL(ignoreSynObj);
  if (isPrimToken)
    currentToken->data.flags.INCL(primToken);
  if (token == FuncDisabled_T
  || token == ExpDisabled_T
  || token == CrtblDisabled_T
  || token == ObjDisabled_T)
    currentToken->data.flags.INCL(isDisabled);

  if (currentSynObj->IsPlaceHolder()
  || (isPrimToken && !currentSynObj->primaryTokenNode))
    currentSynObj->primaryTokenNode = currentToken;

  if (plhSearch
  && (currentSynObj->IsPlaceHolder()
      || currentSynObj->lastError)
  && !currentToken->data.flags.Contains(isDisabled)
  && currentToken->data.token != Comment_T
  && currentToken->data.token != Tilde_T
  && !ignored
  && !newSelection) {
    newSelection = currentToken;
    plhSearch = false;
  }

}

void CProgHTML::Insert (TToken token,bool isPrimToken,bool isOpt/*CHE *pred*/) {
  unsigned len, maxLine=0, lengthLastLine, start=0;
	int loc, ind=0;
  QString str, indent, padding;
	QChar blank(' ');
  bool bold=false, color=false, italic=false;

  switch (token) {
  case VarName_T:
    str = QString(((VarName*)currentSynObj)->varName.c);
    len = str.length();
    break;
  case TDOD_T:
    str = ((TDOD*)currentSynObj)->name.c;
    len = str.length();
    break;
  case Const_T:
    str = ((Constant*)currentSynObj)->str.c;
    len = str.length();
    break;
  case FuncRef_T:
  case TypeRef_T:
  case CrtblRef_T:
    str = ((Reference*)currentSynObj)->refName.c;
    len = str.length();
    break;
  case enumConst_T:
    str = ((EnumConst*)currentSynObj)->Id.c;
    len = str.length();
    break;
  case Comment_T:
    str = currentSynObj->comment.ptr->str.c;
		str.replace('\r',"");
		while (true) {
			loc = str.indexOf('\n',ind);
			if (loc != -1) {
			  maxLine = qMax((int)maxLine,loc-ind);
				ind = loc+1;
			}
			else {
				lengthLastLine = str.length()-ind;
				maxLine = qMax(maxLine,lengthLastLine);
				break;
			}
		}
		str.replace('<',"&lt;");
		str.replace('>',"&gt;");
		indent = indent.fill(' ',currentSynObj->startPosHTML);
		str.replace('\n',"\n"+indent);
		if (lengthLastLine < maxLine)
			str += padding.fill(' ',maxLine-lengthLastLine);
    len = maxLine;
    break;
  default:
    switch (token) {
    case Larrow_T:
      token = LarrowHTML;
      str = TOKENSTR[token];
      len = 3;
      break;
    case Rarrow_T:
      token = RarrowHTML;
      str = TOKENSTR[token];
      len = 3;
      break;
    case LessThan_T:
      token = LtHTML;
      str = TOKENSTR[token];
      len = 1;
      break;
    case LessEqual_T:
      token = LeHTML;
      str = TOKENSTR[token];
      len = 2;
      break;
    case Lshift_T:
      token = LshiftHTML;
      str = TOKENSTR[token];
      len = 2;
      break;
    default:
      str = TOKENSTR[token];
      len = str.length();
    }
    if (currentSynObj->IsPlaceHolder())
      str.replace('<',"&lt;");
      str.replace('>',"&gt;");
  }

  posInLine += len;

  if (token == LarrowHTML
  || token == RarrowHTML
	|| token == NewLineSym_T) {
    if (ignored) {
      code("<i>");
      italic = true;
      code("<span class=\"green2\">"); // dark green
      color = true;
    }
    else if (isPrimToken && currentSynObj->lastError) {
      code("<span class=\"red\">"); // red
      color = true;
    }
    else {
      code("<span class=\"blue\">"); // blue
      color = true;
    }
    code(str.toAscii());
  }
  else if (token == FuncRef_T
  || token == TypeRef_T
  || token == CrtblRef_T
  || token == enumConst_T
  ) {
    if (ignored) {
      code("<i>");
      italic = true;
      code("<span class=\"green2\">"); // dark green
      color = true;
      code(str.toAscii());
    }
    else if (TOKENSTR[token][0] == '(') { //currentSynObj->errorCode == &ERR_Broken_ref) {
      code("<span class=\"red\">"); // red
      color = true;
      code(str.toAscii());
    }
    else if (token == enumConst_T)
      PutLink(&document->IDTable,((EnumConst*)currentSynObj)->refID,
        singleFile,
        currentSynObj->lastError != 0,
        ((EnumConst*)currentSynObj)->Id.c);
    else
      PutLink(&document->IDTable,((Reference*)currentSynObj)->refID,
        singleFile,
        currentSynObj->lastError != 0,
        ((Reference*)currentSynObj)->refName.c);
  }
  else if (token == VarName_T) {
    if (ignored) {
      code("<i>");
      italic = true;
      code("<span class=\"green2\">"); // dark green
      color = true;
      code(((VarName*)currentSynObj)->varName.c);
    }
    else {
      code("<a id=\"");
      IO.WriteCard(((VarName*)currentSynObj)->varID.nID,1);
      code("\"</a>");
      if (currentSynObj->flags.Contains(isOptionalExpr)) {
        code("<span class=\"pink\">"); // pink
        color = true;
      }
      code(((VarName*)currentSynObj)->varName.c);
    }
  }
  else if (token == TDOD_T) {
    if (ignored) {
      code("<I>");
      italic = true;
      code("<span class=\"green2\">"); // dark green
      color = true;
      code(((TDOD*)currentSynObj)->name.c);
    }
    else {
      PutLink(&document->IDTable,((TDOD*)currentSynObj)->ID,
        singleFile,
        currentSynObj->lastError != 0,
        ((TDOD*)currentSynObj)->name.c,
        currentSynObj->flags.Contains(isOptionalExpr));
    }
  }
  else if (token == Comment_T) {
    code("<i>");
    italic = true;
    code("<span class=\"green2\">"); // dark green
    color = true;
    code(str.toAscii());
  }
  else if (TOKENSTR[token][0].isLetter()
  || TOKENSTR[token][0]=='@'
  || TOKENSTR[token][0]=='#'
  || TOKENSTR[token][0]==':'
  || TOKENSTR[token][0]=='Ø') {
    if (ignored) {
      code("<i><span class=\"green2\">"); // dark green
      italic = true;
      color = true;
    }
    else if (isPrimToken && currentSynObj->lastError) {
      code("<span class=\"red\">"); // red
      color = true;
    }
    else {
      if (isOpt)
        code("<span class=\"pink\">"); // pink
      else
        code("<span class=\"blue\">"); // blue
      color = true;
    }
    code(str.toAscii());
  }
  else if (currentSynObj->IsPlaceHolder()
    && token != FuncDisabled_T
    && token != ExpDisabled_T
    && token != CrtblDisabled_T
    && token != ObjDisabled_T) {
    if (ignored) {
      code("<i>");
      italic = true;
      code("<span class=\"green2\">"); // dark green
      color = true;
    }
    else {
      code("<span class=\"red\">"); // red
      color = true;
    }
    code(str.toAscii());
  }
  else {
    if (ignored) {
      code("<i>");
      italic = true;
      code("<span class=\"green2\">"); // dark green
      color = true;
    }
    else if (isPrimToken && currentSynObj->lastError) {
      code("<span class=\"red\">"); // red
      color = true;
    }
    code(str.toAscii());
  }

  if (color)
    code("</span>");
  if (italic)
    code("</i>");
  if (bold)
    code("</b>");
}

void CProgText::NewSel (const QPoint *point) {
  CHETokenNode *chep = (CHETokenNode*)tokenChain.first;
  int x, y, rt, rl, rb, rr;

  x = point->x();
  y = point->y();
  while (chep) {
    rl = chep->data.rect.left();
    rt = chep->data.rect.top();
    rb = chep->data.rect.bottom();
    rr = chep->data.rect.right();
    if (rt <= point->y()
    &&  rl <= point->x()
    &&  rb > point->y()
    &&  rr > point->x())
      break;
    chep = (CHETokenNode*)chep->successor;
  }
  if (chep)
    newSelection = chep;
}

void CProgText::Select (SynObject *selObj) {

  CHETokenNode *select;
  SynObject *optClause, *selAt=selectAt;

  if (newSelection)
    select = newSelection;
  else if (selectAt) {
    while (selAt->parentObject && !selAt->primaryTokenNode) selAt = selAt->parentObject;
    select = selAt->primaryTokenNode;
    if (!select)
      if (currentSelection)
        select = currentSelection;
      else
        select = (CHETokenNode*)tokenChain.first;
	}
  else if (currentSelection)
    select = currentSelection;
  else
    select = (CHETokenNode*)tokenChain.first;

  if (selObj)
    currentSynObj = selObj;
  else
    currentSynObj = select->data.synObject;

	if ((select->data.token == if_T || select->data.token == ifx_T)
	&& currentSynObj->primaryToken == elsif_T)
		currentSynObj = currentSynObj->parentObject;

  if (currentSynObj)
    currentSynObjID = currentSynObj->synObjectID;
  else
    currentSynObjID = 0;

  if (currentSynObj->primaryToken == ObjRef_T
  && select->data.token != Comment_T
  && !selObj) {
    currentSynObj = (TDOD*)((CHE*)((ObjReference*)currentSynObj)->refIDs.first)->data;
    if (select->data.token != Comment_T)
      select = ((TDOD*)currentSynObj)->primaryTokenNode;
  }

  currentSelection = select;
  currentToken = select;

  if (selObj) {
    ((CExecView*)ckd.execView)->selStartPos = selObj->startToken;
    if (selObj->type == implementation_T)
      ((CExecView*)ckd.execView)->selEndPos = selObj->startToken;
    else
      ((CExecView*)ckd.execView)->selEndPos = selObj->endToken;
  }
  else if (currentSelection->data.OptionalClauseToken(optClause)) {
    ((CExecView*)ckd.execView)->selStartPos = select;
    ((CExecView*)ckd.execView)->selEndPos = optClause->endToken;
  }
  else if (select->data.token == Comment_T
  || currentSynObj->IsPlaceHolder()
  || select->data.token == TDOD_T
  || select->data.synObject->type == implementation_T) {
    ((CExecView*)ckd.execView)->selStartPos = select;
    ((CExecView*)ckd.execView)->selEndPos = select;
  }
  else {
    ((CExecView*)ckd.execView)->selStartPos = currentSynObj->startToken;
    ((CExecView*)ckd.execView)->selEndPos = currentSynObj->endToken;
  }

  newSelection = 0;
  selectAt = 0;
  selectAfter = 0;
}

CHETokenNode *CProgText::FindPrecedingPlaceholder() {
  CHETokenNode *tNode;

  for ( tNode = (CHETokenNode*)currentSelection->predecessor;
        tNode;
        tNode = (CHETokenNode*)tNode->predecessor)
    if (tNode->data.synObject->IsPlaceHolder())
      return tNode;
  return currentSelection;
}

CHETokenNode *CProgText::FindNextPlaceholder() {
  CHETokenNode *tNode;

  for ( tNode = (CHETokenNode*)currentSelection->successor;
        tNode;
        tNode = (CHETokenNode*)tNode->successor)
    if (tNode->data.synObject->IsPlaceHolder())
      return tNode;
  return currentSelection;
}

void CProgText::SynObjNewLine () {
  Insert(NewLineSym_T);
	currentSynObj->startPos = lastIndent;
  NewLine();
}

void CProgHTML::SynObjNewLine () {
  Insert(NewLineSym_T);
	currentSynObj->startPosHTML = lastIndent;
  NewLine();
}

void CProgTextBase::NewLine () {
  newLines++;
}

void CProgHTML::NewLine () {
  IO.WriteLn();
  posInLine = currentSynObj->startPosHTML + currentSynObj->currentIndent*INDENT;
  for (int i=1; i<= posInLine; i++)
    code(" ");
	lastIndent = posInLine;
}

void CProgText::Blank () {
  insBlank = true;
}
void CProgHTML::Blank () {
  code(" ");
  posInLine++;
}

void CProgTextBase::INIT () {
  currentPos = 0;
  posInLine = 0;
  newLines = 0;
  currentToken = 0;
  newSelection = 0;
  currentSelection = 0;
  currentSynObj = 0;
  firstMoved = 0;
  insertedChars = 0;
  leftArrows = true;
  showComments = true;
  plhSearch = false;
  //selectAt = 0;
  //selectAfter = 0;
}
