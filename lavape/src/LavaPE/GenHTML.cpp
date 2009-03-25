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

  codeNl("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"");
  codeNl("    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">");
  codeNl("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
  codeNl("<head>");
  codeNl("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"/>");
  codeNl("<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\"/>");
  codeNl("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\"/>");
  codeNl("<meta name=\"GENERATOR\" content=\"Lava HTML Generator\"/>");
  code("<title>");
  code(shortFile.toAscii());
  codeNl("</title>");
  codeNl("<link rel=\"STYLESHEET\" type=\"text/css\" href=\"LavaIcons/LavaStyles.css\"/>");
  codeNl("<style type=\"text/css\" xml:space=\"preserve\">");
  codeNl(".black { color: Black }");
  codeNl(".blue { color: Blue }");
  codeNl(".green { color: Green }");
  codeNl(".red { color: Red }");
  codeNl("</style>");
  codeNl("<script language=\"JavaScript\" src=\"LavaIcons/toggleDisplay.js\" type=\"text/javascript\"></script>");
  codeNl("<script language=\"JavaScript\" src=\"LavaIcons/gotoTarget.js\" type=\"text/javascript\"></script>");
  codeNl("</head>");
  codeNl("<body>");
  code("<div class=\"center\"><h1>");
  if (baseName == "std")
    code(shortFile.toAscii());
  else
    code(dclFile.toAscii());
  codeNl("</h1>");
  if (!singleFile) {
    code("<a href=\"#index\">");
    codeNl("<b>Go To Symbol Index</b></a><br />");
    chpSimSyn=(CHESimpleSyntax*)synDef.first;
    chpSimSyn=(CHESimpleSyntax*)chpSimSyn->successor;
    if (chpSimSyn) {
      code("<a href=\"#imported\">");
      codeNl("<b>Go To Imported Files</b></a>");
    }
  }
  codeNl("</div><br /><br /><div class=\"Element\">");

  GenHTML(myDECL,localCategory,firstChild);
  GenIndex(this,false);

  codeNl("</body></html>");
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
  IO.WriteString("\n<div>");
}

static void codeElementDIV ()
{
  IO.WriteString("\n<div class=\"Element\">");
}

static void codeDIVid (int id,const char idType='S')
{
  IO.WriteString("\n<div id=\"");
	IO.Write(idType);
  codeCard(id);
  code("\">");
}

static void codeEndDIV ()
{
  IO.WriteString("</div>");
}

static void printCommentLines (DString comment) {
  DString comm;
  QString str(comment.c);

  code("<pre><i><span class=\"green\">");
  str = str.replace('<',"&lt;");
  str = str.replace('>',"&gt;");
  str = str.replace('\r',"");
  comm = qPrintable(str);
  code(comm.c);
  code(":</span></i></pre>");
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
  codeNl("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\"");
  codeNl("   \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">");
  codeNl("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
  codeNl("<head>");
  code("<meta http-equiv=\"Content-Type\" ");
  codeNl("content=\"text/html; charset=iso-8859-1\"/>");
  codeNl("<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\"/>");
  codeNl("<meta http-equiv=\"Content-Style-Type\" content=\"text/css\"/>");
  codeNl("<meta name=\"GENERATOR\" content=\"Lava HTML Generator\"/>");
  code("<title>");
  code(file.toAscii());
  codeNl("</title>");
  codeNl("<link rel=\"stylesheet\" type=\"text/css\" href=\"LavaIcons/LavaStyles.css\"/>");
  codeNl("</head>");

  codeNl("<frameset cols=\"50%,*\">");
  code("<frame src=\"");
  file = baseName + "_m.htm";
  code(file.toAscii());
  codeNl("\" name=\"frmLeft\" title=\"frmLeft\"/>");
  codeNl("<frame src=\"LavaIcons/EmptyExec.htm\" name=\"frmRight\" title=\"frmRight\"/>");
  codeNl("<noframes><body>");
  codeNl("This web page uses frames, but your browser doesn't support them.");
  codeNl("</body></noframes>");
  codeNl("</frameset>");
  codeNl("</html>");
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

  codeNl("\n\n<br /><br /><a id=\"index\"></a><h2>Index of symbols defined in this file</h2>");
  codeNl("<p>(Order: by lines from left to right,<br />");
  codeNl("capital letters before small letters)</p>");
  codeNl("<table border=\"0\" bgcolor=\"#FFFFA0\" width=\"100%\" style=\"font-size:10pt\">");
  code("<colgroup span=\"");
  codeCard(COLNO);
  codeNl("\"/>");
  for (i =  symList.begin(); i != symList.end(); ++i) {
    if (index % COLNO == 0)
      code("<tr>");
    code("<td>");
    PutLink(&dartView->GetDocument()->IDTable,(*i).refID,singleFile);
    codeNl("</td>");
    if (++index % COLNO == 0)
      codeNl("</tr>");
  }
  if (index % COLNO != 0)
    codeNl("</tr>");
  codeNl("</table>");

  chpSimSyn=(CHESimpleSyntax*)synDef.first;
  chpSimSyn=(CHESimpleSyntax*)chpSimSyn->successor;
  if (chpSimSyn) {
    codeNl("<br /><a id=\"imported\"></a><h2>Imported files</h2>");
    firstImport = true;
    for (; chpSimSyn; chpSimSyn=(CHESimpleSyntax*)chpSimSyn->successor) {
      PutFileLink(chpSimSyn->data.SyntaxName.c);
      codeNl("<br />");
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

	link = importedFile + "#T" + QString("%1").arg(refID.nID);

	if (pnode) {
		code("<a href=\"");
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
		code("<span class=\"black\">");
		if (pnode->DeclType == VirtualType) {
			link = QString("&lt;") + QString(pnode->FullName.c) + QString("&gt;");
			code((QString("&lt;") + QString(pnode->FullName.c) + QString("&gt;")).toAscii());
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
				symName.replace('>',"&gt;");
				symName.replace('&',"&amp;");
				code(symName.toAscii());
		}
		code("</span></a>");
	}
	else {
		code("<span class=\"red\">");
		code(QString("(%1,%2)").arg(nINCLstring).arg(nIDstring).toAscii());
		code("</span>");
	}
}

static void PutExecLink(DString fileName)
{
  code("<a href=\"");
  code(fileName.c);
  code("\" target=\"frmRight\"><b><i>");
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
  code("</i></b></a>");
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

  code("<a href=\"");
  code(fileName.toAscii());
  code("\">");
  code(linkText.toAscii());
  code("</a>");
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
    code("\n<div class=\"Category\" title=\"click to hide/show details\" onclick=\"toggleDisplay('V");
    codeCard(currentNode->OwnID);
    code("')\">Virtual types:</div>");
		codeDIVid(currentNode->OwnID,'V');
    break;
  case IsDef:
    code("\n<div class=\"Category\" title=\"click to hide/show details\" onclick=\"toggleDisplay('D");
    codeCard(currentNode->OwnID);
    code("')\">Declarations:</div>");
		codeDIVid(currentNode->OwnID,'D');
    break;
  case Attr:
    switch (currentNode->ParentDECL->DeclType) {
    case Interface:
    case Impl:
    case CompObjSpec:
    case CompObj:
    case Initiator:
			code("\n<div class=\"Category\" title=\"click to hide/show details\" onclick=\"toggleDisplay('F");
			codeCard(currentNode->OwnID);
			code("')\">Features:</div>");
 			codeDIVid(currentNode->OwnID,'F');
     break;
    default:
			code("\n<div class=\"Category\" title=\"click to hide/show details\" onclick=\"toggleDisplay('A");
			codeCard(currentNode->OwnID);
			code("')\">Attributes:</div>");
 			codeDIVid(currentNode->OwnID,'A');
    }
    break;
  case IAttr:
		code("\n<div class=\"Category\" title=\"click to hide/show details\" onclick=\"toggleDisplay('I");
		codeCard(currentNode->OwnID);
		code("')\">Inputs:</div>");
 		codeDIVid(currentNode->OwnID,'I');
    break;
  case OAttr:
		code("\n<div class=\"Category\" title=\"click to hide/show details\" onclick=\"toggleDisplay('O");
		codeCard(currentNode->OwnID);
		code("')\">Outputs:</div>");
 		codeDIVid(currentNode->OwnID,'O');
    break;
  case ExecDef:
  case Require:
  case Ensure:
    switch (currentNode->ParentDECL->DeclType) {
    case Function:
    case Initiator:
			code("\n<div class=\"Category\" TITLE=\"click to hide/show details\" onclick=\"toggleDisplay('X");
			codeCard(currentNode->ParentDECL->OwnID);
      switch (currentNode->DeclType) {
      case ExecDef:
			  code("')\">Exec:</div>");
        break;
      case Require:
        if (currentNode->ParentDECL->ParentDECL->DeclType == Impl)
			    code("_ReqI')\">Require:</DIV>");
        else
			    code("_Req')\">Require:</div>");
        break;
      case Ensure:
        if (currentNode->ParentDECL->ParentDECL->DeclType == Impl)
			      code("_EnsI')\">Ensure:</div>");
        else
			    code("_Ens')\">Ensure:</div>");
        break;
      }
      break;
    default:
			code("\n<div class=\"Category\" title=\"click to hide/show details\" onclick=\"toggleDisplay('X");
			codeCard(currentNode->ParentDECL->OwnID);
			code("')\">Invariant:</div>");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<img src=\"LavaIcons/package.png\" alt=\"img\"");
    if (chp) {
      code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code("/>\n");
    if (pnode->LocalName.l) {
      PutComment(pnode0,false,true);
      code(pnode->LocalName.c);
      code(" := ");
    }
    if (pnode->TypeFlags.Contains(isProtected))
      code("<b>opaque package</b>");
    else
      code("<b>package</b>");
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        code(", <b>extends</b> ");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    code("<img src=\"LavaIcons/patpara.png\" alt=\"img\"/>\n");

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
        code(", <b>overrides</b> ");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    if (pnode->SecondTFlags.Contains(isSet)) {
      code("<img src=\"LavaIcons/settyp.png\" alt=\"img\"");
      if (chp) {
        code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    else if (pnode->SecondTFlags.Contains(isGUI)) {
      code("<img src=\"LavaIcons/formint.png\" alt=\"img\"");
      if (chp) {
        code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    else if (pnode->DeclDescType == EnumType) {
			isEnum = true;
      code("<img src=\"LavaIcons/enum.png\" alt=\"img\"");
      enumsel = (CHEEnumSelId*)((TEnumDescription*)pnode->EnumDesc.ptr)->EnumField.Items.first;
      if (enumsel) {
        code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    else {
      code("<img src=\"LavaIcons/classint.png\" alt=\"img\"");
      if (chp) {
        code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
        codeCard(pnode->OwnID);
        code("')\"");
      }
    }
    code("/>\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := ");
    if (pnode->TypeFlags.Contains(isComponent))
      code("<b>component object interface</b>");
    else if (pnode->DeclDescType == EnumType)
      code("<b>enumeration</b>");
    else {
      if (pnode->SecondTFlags.Contains(isGUI))
        code("<b>GUI service</b>");
      else
        code("<b>interface</b>");
    }
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        if (chpTID->data == objectTID)
          break;
        else
          code(", <b>extends</b> ");
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
        code(", <b>signals</b> ");
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
          code("<br />\n");
        code("<img src=\"LavaIcons/enumsel.png\" alt=\"img\"/> ");
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
				code("<br />");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<img src=\"LavaIcons/cospec.png\" alt=\"img\"");
    if (chp) {
      code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code("/>\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := ");
    if (pnode->TypeFlags.Contains(isPersistent))
      code("<b>persistent</b> ");
    code("<b>compon. obj. spec.</b>");
    for ( chpTID = (CHETID*)pnode->Supports.first;
          chpTID;
          chpTID = (CHETID*)chpTID->successor) {
      if (isFirst) {
        code(", <b>supports</b> ");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    if (pnode->SecondTFlags.Contains(isGUI))
      code("<img src=\"LavaIcons/formimpl.png\" alt=\"img\"");
    else
      code("<img src=\"LavaIcons/classimpl.png\" alt=\"img\"");
    if (chp) {
      code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code("/>\n");
    PutComment(pnode0,false,true);
    code("<b>implementation of ");
    if (pnode->SecondTFlags.Contains(isGUI))
      code("GUI service </b>");
    else
      code("</b>");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    code("<img src=\"LavaIcons/coimpl.png\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
    codeCard(pnode->OwnID);
    code("')\"");
    code(" alt=\"img\" title=\"click to hide/show details\"");
    code("/>\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := <b>component object, implements </b>");
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
    code("<IMG SRC=\"LavaIcons/form.png\" STYLE=\"cursor:pointer\" onclick=\"toggleDisplay('S");
    codeCard(pnode->OwnID);
    code("')\"");
    code(" ALT=\"\" TITLE=\"click to hide/show details\"");
    code(">\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := <b>form </b>");
    PutLink(&GetDocument()->IDTable,pnode->RefID,singleFile);
    PutComment(pnode0,true,true);
    PutComment(pnode0,true,false);*/
    break;

  case Initiator:
    myCategory = IsDef;
    PutCategory(myCategory,parentCategory,fstChild);
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<img src=\"LavaIcons/initiato.png\" alt=\"img\"");
    if (chp) {
      code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code("/>\n");
    PutComment(pnode0,false,true);
    code(pnode->LocalName.c);
    code(" := <b>main program</b>");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);
    chp = (CHE*)pnode->NestedDecls.first;
    code("<img src=\"LavaIcons/function.png\" alt=\"img\"");
    if (chp) {
      code(" title=\"click to hide/show details\" style=\"cursor:pointer\" onclick=\"toggleDisplay('S");
      codeCard(pnode->OwnID);
      code("')\"");
    }
    code("/>\n");
    PutComment(pnode0,false,true);
    if (pnode->op == OP_noOp)
      code(pnode->LocalName.c);
    else
      switch (pnode->op) {
      case OP_bwAnd:
        code("operator \"&amp;\"");
        break;
      case OP_lessthan:
        code("operator \"&lt;\"");
        break;
      case OP_lessequal:
        code("operator \"&lt;=\"");
        break;
      case OP_greaterthan:
        code("operator \"&gt;\"");
        break;
      case OP_greaterequal:
        code("operator \"&gt;=\"");
        break;
      case OP_lshift:
        code("operator \"&lt;&lt;\"");
        break;
      case OP_rshift:
        code("operator \"&gt;&gt;\"");
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
      code("<b>protected</b> ");
    if (pnode->TypeFlags.Contains(isAbstract))
      code("<b>abstract</b> ");
    if (pnode->TypeFlags.Contains(isConst))
      code("<b>read-only</b> ");
    if (pnode->TypeFlags.Contains(forceOverride))
      code("<b>force_override</b> ");
    if (pnode->TypeFlags.Contains(defaultInitializer))
      code("<b>default initializer</b>");
    else if (pnode->TypeFlags.Contains(isInitializer))
      code("<b>initializer</b>");
    else
      code("<b>function</b>");

    if (!pnode->SecondTFlags.Contains(funcImpl))
      for ( chpTID = (CHETID*)pnode0->Supports.first;
            chpTID;
            chpTID = (CHETID*)chpTID->successor) {
        if (isFirst) {
          code(", <b>overrides</b> ");
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
        code(", <b>throws</b> ");
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
    code("<a id=\"T");
    codeCard(pnode->OwnID);
    code("\"></a>");
    PutComment(pnode0,false,false);

    if (pnode0->DeclType == Alias)
      code("<img src=\"LavaIcons/alias.png\" alt=\"img\"/>\n");
    else if (pnode->DeclDescType == BasicType)
      code("<img src=\"LavaIcons/basicatt.png\" alt=\"img\"/>\n");
    else {
      pnodeType = GetDocument()->IDTable.GetDECL(pnode->RefID);
      if (pnodeType)
        switch (pnodeType->DeclType) {
        case VirtualType:
          code("<img src=\"LavaIcons/paramatt.png\" alt=\"img\"/>\n");
          break;
        case Interface:
          if (pnodeType->SecondTFlags.Contains(isSet))
            code("<img src=\"LavaIcons/setatt.png\" alt=\"img\"/>\n");
          else
            code("<img src=\"LavaIcons/classattr.png\" alt=\"img\"/>\n");
          break;
        case FormDef:
          code("<img src=\"LavaIcons/formAt.png\" alt=\"img\"/>\n");
          break;
        case EnumDef:
          code("<img src=\"LavaIcons/enumattr.png\" alt=\"img\"/>\n");
          break;
        default:
          code("<img src=\"LavaIcons/undefine.png\" alt=\"img\"/>\n");
        }
      else
        code("<img src=\"LavaIcons/undefine.png\" alt=\"img\"/>\n");
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
          code("<b>, acquaintance</b>");
        else
          code("<b>, reverse reference</b>");


    if (pnode->TypeFlags.Contains(isProtected))
      code("<b>, protected</b>");

    if (pnode->TypeFlags.Contains(isOptional))
      code("<b>, optional</b>");
    else if (pnode->TypeFlags.Contains(isPlaceholder))
      code("<b>, placeholder</b>");

    if (pnode->TypeFlags.Contains(isConst))
      code("<b>, read-only</b>");

    if (pnode->TypeFlags.Contains(hasSetGet))
      code("<b>, access via set/get functions</b>");

    if (pnode->TypeFlags.Contains(isAbstract))
      code("<b>, abstract</b>");

    if (pnode->TypeFlags.Contains(consumable))
      code("<b>, consumable</b>");

    if (pnode->DeclType == Attr)
      for ( chpTID = (CHETID*)pnode0->Supports.first;
            chpTID;
            chpTID = (CHETID*)chpTID->successor) {
        if (isFirst) {
          code(", <b>overrides</b> ");
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
			code("<div class=\"Category\">"); 
			// just to simulate a category DIV which will be terminated on the calling GenHTML level

      PutExecLink(DString(qPrintable(baseName)) + "/" + fullName + "_" + ownID + ".htm");
    }

    if (singleFile) {
      code("<div id=X");
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
      code(">\n\n<hr><pre>");
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
      code("</pre><hr>\n\n");
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
