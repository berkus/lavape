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


#include "qmessagebox.h"
#include "qfileinfo.h"
#include "LavaPE.h"
#include "LavaPEStringInit.h"
#include "Constructs.h"
#include "LavaPEView.h"
#include "LavaPEFrames.h"
//#include "PropView.h"
#include "ExecTree.h"
#include "qstring.h"
#include "Convert.h"
#include "DIO.h"
#include "SynIO.h"
#include "SYSTEM.h"
//#include "LavaPEWizard.h"
#include "Boxes.h"
//#include "Resource.h"
#include <list>
//#include "qvaluelist.h"
#include <qdir.h>
//#include <direct.h>
//#include <errno.h>
//#include "stdafx.h"

#pragma hdrstop


static inline void code (const char *text);
static inline void codeNl (const char *text);
static inline void codeDIV ();
static inline void codeEndDIV ();
static void GenFramesHTML();
static void GenIndex(CLavaPEView *dartView,bool withINCL);
static void PutLink(TIDTable *IDTable,TID &refID,bool singleFile);
static void PutExecLink(DString fileName);
static void PutFileLink(QString fileName);

static LavaDECL *currentNode;
//static int i, oldIndent, indent = 0;
static bool singleFile=false, firstImport;
static QString baseFile, baseName, execFile, dclFile, shortFile;
static OutFile *dclFileRef;



void CLavaPEView::OnGenHtml() 
{
  singleFile = false;
  OnGenHtmlI();
}

void CLavaPEView::OnGenHtmlS() 
{
  singleFile = true;
  OnGenHtmlI();
}

void CLavaPEView::OnGenHtmlI() 
{
  QString pathName = GetDocument()->GetFilename();
  TDeclType localCategory=NoDef;
  SyntaxDefinition &synDef = GetDocument()->mySynDef->SynDefTree;
  CHESimpleSyntax *chpSimSyn;
  QString iconPathTo, copyCommand, mdCommand;
  bool firstChild=true;

  QFileInfo fi(pathName);
  iconPathTo = fi.path();
  baseName = fi.baseName();
  pathName = pathName.left(pathName.length()-5);
  baseFile = pathName;
  copyCommand = ExeDir + "/cp_icons_std" + " " + ExeDir + " " + iconPathTo;
  if (!singleFile)
    copyCommand += " " + baseName;
#ifdef WIN32
  copyCommand.replace('/',"\\");
#endif
  system(copyCommand.toAscii());

  if (singleFile)
    dclFile = baseFile + "_1.htm";
  else {
    dclFile = baseFile + "_m.htm";
    GenFramesHTML();
  }


  IO.FOpenOutput(&dclFileRef,dclFile.toAscii());

	QFileInfo sf(dclFile);
  shortFile = sf.baseName();

  codeNl("<HTML>");
  codeNl("<HEAD>");
  codeNl("<META http-equiv=\"Content-Type\" CONTENT=\"text/html; charset=iso-8859-1\">");
  codeNl("<META NAME=\"GENERATOR\" CONTENT=\"Lava HTML Generator\">");
  code("<TITLE>");
  code(shortFile.toAscii());
  codeNl("</TITLE>");
  codeNl("<LINK REL=\"STYLESHEET\" TYPE=\"text/css\" HREF=\"LavaIcons/LavaStyles.css\">");
  codeNl("<SCRIPT LANGUAGE=\"JavaScript\" SRC=\"LavaIcons/toggleDisplay.js\" TYPE=\"text/javascript\">");
  codeNl("</SCRIPT>");
  codeNl("<SCRIPT LANGUAGE=\"JavaScript\" SRC=\"LavaIcons/gotoTarget.js\" TYPE=\"text/javascript\">");
  codeNl("</SCRIPT>");
  codeNl("</HEAD>");
  codeNl("<BODY>");
  code("<CENTER><H1>");
  if (baseName == "std")
    code(shortFile.toAscii());
  else
    code(dclFile.toAscii());
  codeNl("</H1>");
  if (!singleFile) {
    code("<A HREF=\"#index\">");
    codeNl("<B>Go To Symbol Index</B></A><BR>");
    chpSimSyn=(CHESimpleSyntax*)synDef.first;
    chpSimSyn=(CHESimpleSyntax*)chpSimSyn->successor;
    if (chpSimSyn) {
      code("<A HREF=\"#imported\">");
      codeNl("<B>Go To Imported Files</B></A>");
    }
  }
  codeNl("</CENTER><BR><BR><DIV CLASS=Element>");

  GenHTML(myDECL,localCategory,firstChild);
  GenIndex(this,false);

  codeNl("</DIV></BODY></HTML>");
  IO.CloseOutput();
}

static inline void code (const char *text)
/* put constant text code */
{
  IO.WriteString(text);
}

static inline void codeNl (const char *text)
/* put constant text code */
{
  IO.WriteString(text);
  IO.WriteLn();
}

static inline void codeCard (unsigned c)
{
  IO.WriteCard(c,1);
}

static void codeDIV ()
{
  IO.WriteString("\n<DIV>");
}

static void codeElementDIV ()
{
  IO.WriteString("\n<DIV class=\"Element\">");
}

static void codeDIVid (int id,const char idType='S')
{
  IO.WriteString("\n<DIV ID=\"");
	IO.Write(idType);
  codeCard(id);
  code("\">");
}

static void codeEndDIV ()
{
  IO.WriteString("</DIV>");
}

static void printCommentLines (DString comment) {
  DString comm;
  QString str(comment.c);

  code("<pre><I><FONT color=#00ff00>");
  str = str.replace('<',"&lt;");
  str = str.replace('\r',"");
  comm = qPrintable(str);
  code(comm.c);
  code(":</FONT></I></pre>");
}


using namespace std ;

struct SymEntry {
  DString fullName;
  TID refID;
};

bool operator< (const SymEntry &lhs,const SymEntry &rhs) {
  if (lhs.fullName < rhs.fullName) return true;
  else return false;
}

bool operator> (const SymEntry &lhs,const SymEntry &rhs) {
  if (lhs.fullName > rhs.fullName) return true;
  else return false;
}

bool operator== (const SymEntry &lhs,const SymEntry &rhs) {
  if (lhs.fullName == rhs.fullName) return true;
  else return false;
}

bool operator!= (const SymEntry &lhs,const SymEntry &rhs) {
  if (lhs.fullName != rhs.fullName) return true;
  else return false;
}

typedef /*QValueList*/list<SymEntry> SYMLIST;


static void GenFramesHTML() {
  QString file = baseFile + ".htm";

  IO.OpenOutput(file.toAscii());
  codeNl("<HTML>");
  codeNl("<HEAD>");
  code("<META http-equiv=\"Content-Type\" ");
  codeNl("CONTENT=\"text/html; charset=iso-8859-1\">");
  codeNl("<META NAME=\"GENERATOR\" CONTENT=\"Lava HTML Generator\">");
  code("<TITLE>");
  code(file.toAscii());
  codeNl("</TITLE>");
  codeNl("<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"LavaIcons/LavaStyles.css\">");
  codeNl("</head>");

  codeNl("<FRAMESET COLS=\"50%,*\">");
  code("<FRAME SRC=\"");
  file = baseName + "_m.htm";
  code(file.toAscii());
  codeNl("\" NAME=\"frmLeft\">");
  codeNl("<FRAME SRC=\"LavaIcons/EmptyExec.htm\" NAME=\"frmRight\">");
  codeNl("<NOFRAMES>");
  codeNl("<P>This web page uses frames, but your browser doesn't support them.</P>");
  codeNl("</NOFRAMES>");
  codeNl("</FRAMESET>");
  codeNl("</HTML>");
  IO.CloseOutput();
}

#define COLNO 2

static void GenIndex(CLavaPEView *dartView,bool withINCL)
{
  TIDTable *IDTable = (TIDTable*)dartView->GetDocument()->mySynDef->IDTable;
  SyntaxDefinition &synDef = dartView->GetDocument()->mySynDef->SynDefTree;
  LavaDECL *decl, *parentDecl, *pDECL;
  int incl, id;
  SYMLIST symList;
  SYMLIST::iterator i;
  SymEntry *sep;
  CHESimpleSyntax *chpSimSyn;
  int index=0;
  bool inFormDef=false;

  for (incl = 0; incl < IDTable->freeINCL; incl++) {
    if (IDTable->IDTab[incl]->isValid) {
      for (id = 0; id < IDTable->IDTab[incl]->freeID; id++) {
        if (IDTable->IDTab[incl]->SimpleIDTab[id]->idType == globalID) {
          decl = *IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL;
          if (decl->FullName.l) {
            for ( parentDecl = decl;
                  parentDecl;
                  parentDecl = parentDecl->ParentDECL)
              if (parentDecl->DeclType == FormDef) {
                inFormDef = true;
                break;
              }
            if (!inFormDef) {
              sep = new SymEntry;
              sep->refID = TID(id, incl);
              pDECL = *IDTable->IDTab[incl]->SimpleIDTab[id]->pDECL;
              sep->fullName = pDECL->FullName;
              if (pDECL->DeclType == Impl
              || (pDECL->DeclType == Function
                  && pDECL->ParentDECL->DeclType == Impl)
              || ((pDECL->DeclType == IAttr || pDECL->DeclType == OAttr)
                   && pDECL->ParentDECL->ParentDECL->DeclType == Impl))
                sep->fullName += "::impl";
              symList.insert(symList.end(),*sep);
            }
            else
              inFormDef = false;
          }//if
        }//if
      }//for
    }//if
    if (!withINCL)
      break;
  }//for
  symList.sort();

  codeNl("\n\n<BR><BR><A NAME=\"index\"></A><H2>Index of symbols defined in this file</H2>");
  codeNl("<P>(Order: by lines from left to right,<BR>");
  codeNl("capital letters before small letters)</P>");
  codeNl("<FONT SIZE=1>");
  code("<TABLE BORDER=\"0\" BGCOLOR=\"#FFFFA0\" WIDTH=\"100%\" COLS=\"");
  codeCard(COLNO);
  codeNl("\" STYLE=\"font-size:10pt\">");
  for (i =  symList.begin(); i != symList.end(); ++i) {
    if (index % COLNO == 0)
      code("<TR>");
    code("<TD>");
    PutLink(&dartView->GetDocument()->IDTable,(*i).refID,singleFile);
    codeNl("</TD>");
    if (++index % COLNO == 0)
      codeNl("</TR>");
  }
  if (index % COLNO != 0)
    codeNl("</TR>");
  codeNl("</TABLE></FONT>");

  chpSimSyn=(CHESimpleSyntax*)synDef.first;
  chpSimSyn=(CHESimpleSyntax*)chpSimSyn->successor;
  if (chpSimSyn) {
    codeNl("<BR><A NAME=\"imported\"></A><H2>Imported files</H2>");
    firstImport = true;
    for (; chpSimSyn; chpSimSyn=(CHESimpleSyntax*)chpSimSyn->successor) {
      PutFileLink(chpSimSyn->data.SyntaxName.c);
      codeNl("<BR>");
      firstImport = false;
    }
  }
}

static void PutComment(LavaDECL *pnode,bool isTrailing,bool isInLine)
{
  unsigned loc = 0;
  DString commentOut;

  if (!pnode->DECLComment.ptr)
    return;

  if ((isTrailing ^ pnode->DECLComment.ptr->PrintFlags.Contains(trailingComment))
  || (isInLine ^ pnode->DECLComment.ptr->PrintFlags.Contains(inLineComment)))
    return;

  commentOut = pnode->DECLComment.ptr->Comment;
/*
  if (isInLine)
    if (isTrailing) {
      code(" ");
      printCommentLines(pnode->DECLComment.ptr->Comment);
    }
    else {
      printCommentLines(pnode->DECLComment.ptr->Comment);
      code(" ");
    }
  else*/
    printCommentLines(pnode->DECLComment.ptr->Comment);
	code("\n");
}


static void PutLink(TIDTable *IDTable,TID &refID,bool singleFile)
{
  unsigned loc=0;
  LavaDECL *pnode=0;
  QString importedFile, symName, link, nINCLstring, nIDstring;

  if (refID.nID >= 0)
    pnode = IDTable->GetDECL(refID);

  if (pnode) {
    if (refID.nINCL > 1) {// external Lava doc != std.lava
      importedFile = IDTable->GetRelSynFileName(refID).c;
			QFileInfo fi(importedFile);
			importedFile = fi.filePath();
      importedFile.replace('\\',"/");
      importedFile.replace(':',"|");
			if (singleFile)
				importedFile = "file://" + importedFile + "_1.htm";
			else
				importedFile = "file://" + importedFile + "_m.htm";
    }
    else if (refID.nINCL == 1) {// std.lava
			if (singleFile)
				importedFile = "std_1.htm";
			else
				importedFile = "std_m.htm";
    }

  }

	link = importedFile + "#" + QString("%1").arg(refID.nID);

	if (pnode) {
		code("<A HREF=\"");
		if (link[0] == '#') {
			code("javascript:gotoTarget('");
			code(link.toAscii());
			code("','");
			code(QString("S%1").arg(refID.nID).toAscii());
			code("')\">");
		}
		else {
			code(link.toAscii());
			code("\">");
		}
		code("<FONT color=#000000>");
		if (pnode->DeclType == VirtualType) {
			link = QString("&lt;") + QString(pnode->FullName.c) + QString(">");
			code((QString("&lt;") + QString(pnode->FullName.c) + QString(">")).toAscii());
		}
		else {
				symName = pnode->FullName.c;
        if (pnode->DeclType == Impl
        || (pnode->DeclType == Function
            && pnode->ParentDECL->DeclType == Impl)
        || ((pnode->DeclType == IAttr || pnode->DeclType == OAttr)
            && pnode->ParentDECL->ParentDECL->DeclType == Impl))
          symName += "::impl";
				symName.replace('<',"&lt;");
				code(symName.toAscii());
		}
		code("</FONT></A>");
	}
	else {
		code("<FONT color=#FF0000>");
		code(QString("(%1,%2)").arg(nINCLstring).arg(nIDstring).toAscii());
		code("</FONT>");
	}
}

static void PutExecLink(DString fileName)
{
  code("<A HREF=\"");
  code(fileName.c);
  code("\" TARGET=\"frmRight\"><B><I>");
  switch (currentNode->DeclType) {
  case Require:
    code("Require");
    break;
  case Ensure:
    code("Ensure");
    break;
  default:
    switch (currentNode->ParentDECL->DeclType) {
    case Function:
    case Initiator:
      code("Exec");
      break;
    default:
      code("Invariant");
    }
  }
  code("</I></B></A>");
}

static void PutFileLink(QString fileName)
{
  unsigned loc=0;
  QString linkText;

  if (firstImport) {
    if (singleFile)
      fileName = "std_1.htm";
    else
      fileName = "std_m.htm";
    linkText = fileName;
  }
  else {
		QFileInfo fi(fileName);
		fileName = fi.filePath();
    if (singleFile)
      fileName += "_1.htm";
    else
      fileName += "_m.htm";
    linkText = fileName;
    fileName.replace(':',"|");
    fileName.replace('\\',"/");
  }

  code("<A HREF=\"");
  code(fileName.toAscii());
  code("\">");
  code(linkText.toAscii());
  code("</A>");
}


static void PutCategory(TDeclType myCategory,TDeclType &parentCategory, bool &firstChild)
{
  if (myCategory == parentCategory) {
		codeElementDIV();
    return;
	}

  parentCategory = myCategory;

  if (!currentNode->ParentDECL)
    return;

	if (!firstChild)
		codeEndDIV(); // terminate preceding category DIV
	else
		firstChild = false;

  switch (myCategory) {
  case VirtualType:
    code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('V");
    codeCard(currentNode->OwnID);
    code("')\">Virtual types:</DIV>");
		codeDIVid(currentNode->OwnID,'V');
    break;
  case IsDef:
    code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('D");
    codeCard(currentNode->OwnID);
    code("')\">Declarations:</DIV>");
		codeDIVid(currentNode->OwnID,'D');
    break;
  case Attr:
    switch (currentNode->ParentDECL->DeclType) {
    case Interface:
    case Impl:
    case CompObjSpec:
    case CompObj:
    case Initiator:
			code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('F");
			codeCard(currentNode->OwnID);
			code("')\">Features:</DIV>");
 			codeDIVid(currentNode->OwnID,'F');
     break;
    default:
			code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('A");
			codeCard(currentNode->OwnID);
			code("')\">Attributes:</DIV>");
 			codeDIVid(currentNode->OwnID,'A');
    }
    break;
  case IAttr:
		code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('I");
		codeCard(currentNode->OwnID);
		code("')\">Inputs:</DIV>");
 		codeDIVid(currentNode->OwnID,'I');
    break;
  case OAttr:
		code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('O");
		codeCard(currentNode->OwnID);
		code("')\">Outputs:</DIV>");
 		codeDIVid(currentNode->OwnID,'O');
    break;
  case ExecDef:
  case Require:
  case Ensure:
    switch (currentNode->ParentDECL->DeclType) {
    case Function:
    case Initiator:
			code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('X");
			codeCard(currentNode->ParentDECL->OwnID);
      switch (currentNode->DeclType) {
      case ExecDef:
			  code("')\">Exec:</DIV>");
        break;
      case Require:
        if (currentNode->ParentDECL->ParentDECL->DeclType == Impl)
			    code("_ReqI')\">Require:</DIV>");
        else
			    code("_Req')\">Require:</DIV>");
        break;
      case Ensure:
        if (currentNode->ParentDECL->ParentDECL->DeclType == Impl)
			      code("_EnsI')\">Ensure:</DIV>");
        else
			    code("_Ens')\">Ensure:</DIV>");
        break;
      }
      break;
    default:
			code("\n<DIV CLASS=\"Category\" TITLE=\"click to hide/show details\" ONCLICK=\"toggleDisplay('X");
			codeCard(currentNode->ParentDECL->OwnID);
			code("')\">Invariant:</DIV>");
    }
    break;
  }
	if (myCategory != ExecDef
  && myCategory != Require
  && myCategory != Ensure)
		codeElementDIV();
}


void CLavaPEView::GenHTML(LavaDECL *pnode,TDeclType &parentCategory, bool &fstChild) 
{  
  CHE *chp=0;
  TDeclType localCategory=NoDef, myCategory;
  bool isFirst=true, firstChild=true, isEnum=false;
  LavaDECL *pnode0=pnode, *pnodeType;
  CHEEnumSelId* enumsel=0;
  CHETID *chpTID;
  DString fullName, ownID, rmDir;
  unsigned loc;
  CLavaPEDoc *myDoc=GetDocument();
  TID objectTID=TID(myDoc->IDTable.BasicTypesID[B_Object],myDoc->isStd?0:1);

  currentNode = pnode;

  switch (pnode->DeclType) {

  case Package:
  case Component:
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<IMG SRC=\"LavaIcons/package.png\"");
    if (chp) {
      code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code(">\n");
    if (pnode->LocalName.l) {
      PutComment(pnode0,false,true);
      code(pnode->LocalName.c);
      code(" := ");
    }
    if (pnode->TypeFlags.Contains(isProtected))
      code("<B>opaque package</B>");
    else
      code("<B>package</B>");
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        code(", <B>extends</B> ");
        isFirst = false;
      }
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    if (chp) {
      codeDIVid(pnode->OwnID);
      while (chp) {
        GenHTML((LavaDECL*)chp->data,localCategory,firstChild);
        chp = (CHE*)chp->successor;
      }
      codeEndDIV();
      codeEndDIV();
    }
    break;

  case VirtualType:
    myCategory = VirtualType;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    code("<IMG SRC=\"LavaIcons/patpara.png\">\n");

    PutComment(pnode0,false,true);
    code(pnode0->LocalName.c);

    if (!pnode->TypeFlags.Contains(isAbstract)) {
      code(" = ");
      PutLink(&GetDocument()->IDTable,pnode->RefID,singleFile);
    }

    for ( chpTID = (CHETID*)pnode0->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        code(", <B>overrides</B> ");
        isFirst = false;
      }
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }

    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    break;

  case Interface:
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    if (pnode->SecondTFlags.Contains(isSet)) {
      code("<IMG SRC=\"LavaIcons/settyp.png\"");
      if (chp) {
        code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    else if (pnode->SecondTFlags.Contains(isGUI)) {
      code("<IMG SRC=\"LavaIcons/formint.png\"");
      if (chp) {
        code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    else if (pnode->DeclDescType == EnumType) {
			isEnum = true;
      code("<IMG SRC=\"LavaIcons/enum.png\"");
      enumsel = (CHEEnumSelId*)((TEnumDescription*)pnode->EnumDesc.ptr)->EnumField.Items.first;
      if (enumsel) {
        code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    else {
      code("<IMG SRC=\"LavaIcons/classint.png\"");
      if (chp) {
        code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    code(">\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := ");
    if (pnode->TypeFlags.Contains(isComponent))
      code("<B>component object interface</B>");
    else if (pnode->DeclDescType == EnumType)
      code("<B>enumeration</B>");
    else {
      if (pnode->SecondTFlags.Contains(isGUI))
        code("<B>GUI service</B>");
      else
        code("<B>interface</B>");
    }
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        if (chpTID->data == objectTID)
          break;
        else
          code(", <B>extends</B> ");
        isFirst = false;
      }
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }
    isFirst = true;
    for ( chpTID = (CHETID*)pnode0->Inherits.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        code(", <B>signals</B> ");
        isFirst = false;
      }
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    if (enumsel) {
      codeDIVid(pnode->OwnID);
      while (enumsel) {
        if (isFirst)
          isFirst = false;
        else
          code("<BR>\n");
        code("<IMG SRC=\"LavaIcons/enumsel.png\"> ");
        code(enumsel->data.Id.c);
        enumsel = (CHEEnumSelId*)enumsel->successor;
      }
			if (!chp)
				codeEndDIV();
    }
    if (chp) {
			if (!isEnum)
				codeDIVid(pnode->OwnID);
			else
				code("<BR>");
      while (chp) {
        GenHTML((LavaDECL*)chp->data,localCategory,firstChild);
        chp = (CHE*)chp->successor;
      }
      codeEndDIV();
      codeEndDIV();
    }
    break;

  case CompObjSpec:
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<IMG SRC=\"LavaIcons/cospec.png\"");
    if (chp) {
      code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code(">\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := ");
    if (pnode->TypeFlags.Contains(isPersistent))
      code("<B>persistent</B> ");
    code("<B>compon. obj. spec.</B>");
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        code(", <B>supports</B> ");
        isFirst = false;
      }
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    break;

  case Impl:
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    if (pnode->SecondTFlags.Contains(isGUI))
      code("<IMG SRC=\"LavaIcons/formimpl.png\"");
    else
      code("<IMG SRC=\"LavaIcons/classimpl.png\"");
    if (chp) {
      code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code(">\n");
    PutComment(pnode0,false,true);
    code("<B>implementation of ");
    if (pnode->SecondTFlags.Contains(isGUI))
      code("GUI service </B>");
    else
      code("</B>");
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst)
        isFirst = false;
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    if (chp && (!pnode->SecondTFlags.Contains(isGUI) || chp->successor)) {
      codeDIVid(pnode->OwnID);
      while (chp) {
        GenHTML((LavaDECL*)chp->data,localCategory,firstChild);
        chp = (CHE*)chp->successor;
      }
      codeEndDIV();
      codeEndDIV();
    }
    break;

  case CompObj:
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    code("<IMG SRC=\"LavaIcons/coimpl.png\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
    codeCard(pnode->OwnID);
    code("')\"");
    code(" ALT=\"\" TITLE=\"click to hide/show details\"");
    code(">\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := <B>component object, implements </B>");
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst)
        isFirst = false;
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    break;

  case FormDef:
/*    if (pnode->ParentDECL->SecondTFlags.Contains(isGUI))
      break;
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    code("<IMG SRC=\"LavaIcons/form.png\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
    codeCard(pnode->OwnID);
    code("')\"");
    code(" ALT=\"\" TITLE=\"click to hide/show details\"");
    code(">\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := <B>form </B>");
    PutLink(&GetDocument()->IDTable,pnode->RefID,singleFile);
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);*/
    break;

  case Initiator:
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<IMG SRC=\"LavaIcons/initiato.png\"");
    if (chp) {
      code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code(">\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := <B>main program</B>");
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    if (chp) {
      codeDIVid(pnode->OwnID);
      while (chp) {
        GenHTML((LavaDECL*)chp->data,localCategory,firstChild);
        chp = (CHE*)chp->successor;
      }
      codeEndDIV();
      codeEndDIV();
    }
    break;

  case Function:
    myCategory = Attr;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<IMG SRC=\"LavaIcons/function.png\"");
    if (chp) {
      code(" ALT=\"\" TITLE=\"click to hide/show details\" STYLE=\"cursor:pointer\" ONCLICK=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code(">\n");
    PutComment(pnode0,false,true);
    if (pnode->op == OP_noOp)
      code(pnode->LocalName.c);
    else
      switch (pnode->op) {
      case OP_lessthen:
        code("operator \"&lt;\"");
        break;
      case OP_lessequal:
        code("operator \"&lt;=\"");
        break;
      case OP_lshift:
        code("operator \"&lt;&lt;\"");
        break;
      case OP_arrayGet:
        code("operator \"&lt;-[]\"");
        break;
      case OP_arraySet:
        code("operator \"[]&lt;-\"");
        break;
      default:
        code(pnode->LocalName.c);
      }
    code(" := ");
    if (pnode->TypeFlags.Contains(isProtected))
      code("<B>protected</B> ");
    if (pnode->TypeFlags.Contains(isAbstract))
      code("<B>abstract</B> ");
    if (pnode->TypeFlags.Contains(isConst))
      code("<B>read-only</B> ");
    if (pnode->TypeFlags.Contains(forceOverride))
      code("<B>force_override</B> ");
    if (pnode->TypeFlags.Contains(defaultInitializer))
      code("<B>default initializer</B>");
    else if (pnode->TypeFlags.Contains(isInitializer))
      code("<B>initializer</B>");
    else
      code("<B>function</B>");

    if (!pnode->SecondTFlags.Contains(funcImpl))
      for ( chpTID = (CHETID*)pnode0->Supports.first;
            chpTID;
            chpTID = (CHETID*)chpTID->successor) {
        if (isFirst) {
          code(", <B>overrides</B> ");
          isFirst = false;
        }
        else
          code(", ");
        PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
      }

    isFirst = true;
    for ( chpTID = (CHETID*)pnode0->Inherits.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        code(", <B>throws</B> ");
        isFirst = false;
      }
      else
        code(", ");
      PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
    }
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    if (chp) {
      codeDIVid(pnode->OwnID);
      while (chp) {
        GenHTML((LavaDECL*)chp->data,localCategory,firstChild);
        chp = (CHE*)chp->successor;
      }
      codeEndDIV(); // terminate last category DIV
      codeEndDIV(); // terminate "all children" DIV
    }
    break;

  case Attr:
  case IAttr:
  case OAttr:
    if (pnode->DeclType == Alias)
      myCategory = IsDef;
    else
      myCategory = pnode->DeclType;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<A NAME=\"");
    codeCard(pnode->OwnID);
    code("\"></A>");
    PutComment(pnode0,false,false);

    if (pnode0->DeclType == Alias)
      code("<IMG SRC=\"LavaIcons/alias.png\">\n");
    else if (pnode->DeclDescType == BasicType)
      code("<IMG SRC=\"LavaIcons/basicatt.png\">\n");
    else {
      pnodeType = GetDocument()->IDTable.GetDECL(pnode->RefID);
      if (pnodeType)
        switch (pnodeType->DeclType) {
        case VirtualType:
          code("<IMG SRC=\"LavaIcons/paramatt.png\">\n");
          break;
        case Interface:
          if (pnodeType->SecondTFlags.Contains(isSet))
            code("<IMG SRC=\"LavaIcons/setatt.png\">\n");
          else
            code("<IMG SRC=\"LavaIcons/classattr.png\">\n");
          break;
        case FormDef:
          code("<IMG SRC=\"LavaIcons/formAt.png\">\n");
          break;
        case EnumDef:
          code("<IMG SRC=\"LavaIcons/enumattr.png\">\n");
          break;
        default:
          code("<IMG SRC=\"LavaIcons/undefine.png\">\n");
        }
      else
        code("<IMG SRC=\"LavaIcons/undefine.png\">\n");
    }

    PutComment(pnode0,false,true);
    if (pnode0->TypeFlags.Contains(isStateObjectY))
      code("~");
    else if (pnode0->TypeFlags.Contains(isAnyCatY))
      code("~");
    code(pnode0->LocalName.c);

    code(" : ");

    if (pnode0->TypeFlags.Contains(substitutable))
      code("{");
    PutLink(&GetDocument()->IDTable,pnode->RefID,singleFile);
    if (pnode0->TypeFlags.Contains(substitutable))
      code("}");

    if (pnode->DeclType == Attr)
      if (pnode->TypeFlags.Contains(constituent))
        ;
      else
        if (pnode->TypeFlags.Contains(acquaintance)) 
          code("<B>, acquaintance</B>");
        else
          code("<B>, reverse reference</B>");


    if (pnode->TypeFlags.Contains(isProtected))
      code("<B>, protected</B>");

    if (pnode->TypeFlags.Contains(isOptional))
      code("<B>, optional</B>");
    else if (pnode->TypeFlags.Contains(isPlaceholder))
      code("<B>, placeholder</B>");

    if (pnode->TypeFlags.Contains(isConst))
      code("<B>, read-only</B>");

    if (pnode->TypeFlags.Contains(hasSetGet))
      code("<B>, access via set/get functions</B>");

    if (pnode->TypeFlags.Contains(isAbstract))
      code("<B>, abstract</B>");

    if (pnode->TypeFlags.Contains(consumable))
      code("<B>, consumable</B>");

    if (pnode->DeclType == Attr)
      for ( chpTID = (CHETID*)pnode0->Supports.first;
            chpTID;
            chpTID = (CHETID*)chpTID->successor) {
        if (isFirst) {
          code(", <B>overrides</B> ");
          isFirst = false;
        }
        else
          code(", ");
        PutLink(&GetDocument()->IDTable,chpTID->data,singleFile);
      }

    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);
    break;

  case NoDef:
    if (pnode->DeclDescType != ExecDesc)
      break;
  case ExecDef:
  case Require:
  case Ensure:
    switch (pnode->DeclType) {
    case ExecDef:
      myCategory = ExecDef;
      break;
    case Ensure:
      myCategory = Ensure;
      break;
    case Require:
      myCategory = Require;
      break;
    }
    if (singleFile)
      PutCategory(myCategory,parentCategory,fstChild);
    else {
      parentCategory = myCategory;
      fullName=pnode->ParentDECL->FullName;
      ConvertCLASS::IntToString(pnode->ParentDECL->OwnID,ownID);
      loc = 0;
      while (fullName.Contains("::",loc,loc)) {
        fullName.Delete(loc,2);
        fullName.Insert("_",loc++);
      }
      switch (pnode->DeclType) {
      case Require:
        fullName += "_Req";
        break;
      case Ensure:
        fullName += "_Ens";
        break;
      default:;
      }
			if (!fstChild)
				codeEndDIV();
			code("<DIV class=\"Category\">"); 
			// just to simulate a category DIV which will be terminated on the calling GenHTML level

      PutExecLink(DString(qPrintable(baseName)) + "/" + fullName + "_" + ownID + ".htm");
    }

    if (singleFile) {
      code("<DIV ID=X");
      codeCard(pnode->ParentDECL->OwnID);
      switch (pnode->DeclType) {
      case Require:
        if (pnode->ParentDECL->ParentDECL->DeclType == Impl)
          code("_ReqI");
        else
          code("_Req");
        break;
      case Ensure:
        if (pnode->ParentDECL->ParentDECL->DeclType == Impl)
          code("_EnsI");
        else
          code("_Ens");
        break;
      default:;
      }
      code(">\n\n<HR><PRE>");
    }
    else {
      DString ef(DString(qPrintable(baseFile)) + "/" + fullName + "_" + ownID + ".htm");
      execFile = QString(ef.c);
			QDir dir(baseFile);
			if (!dir.exists()) {
				bool rc = dir.mkdir(baseFile);
				if (!rc) break;
			}
      IO.OpenOutput(execFile.toAscii());
      if (!IO.Done) break;
    }

    DrawExec(fullName + ".htm",singleFile,GetDocument(),pnode0,(SynObject*)pnode0->Exec.ptr);

    if (singleFile) {
      code("</PRE><HR>\n\n");
    }
    else {
      IO.CloseOutput();
      IO.SwitchOutput(dclFileRef); // switch back to IO.StdOut
    }
    break;

  case UnDef:
  case BasicDef:
  default: ;
  }

  if (pnode->DeclType != ExecDef
  && pnode->DeclType != Ensure
  && pnode->DeclType != Require
  && pnode->DeclType != FormDef)
    codeEndDIV(); // terminate "single element" or "embedded exec" DIV
}
