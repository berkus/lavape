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

#include "docview.h"
#include "qstring.h"
#include "qstatusbar.h"
#include "qfontmetrics.h"
#include <QScrollArea>
#include "qapplication.h"
#include "qnamespace.h"
#include "qmessagebox.h"
#include "qcheckbox.h"
#include <QMenu>
#include "qwidget.h"
#include "qevent.h"
//Added by qt3to4:
#include <QPixmap>
#include <QFocusEvent>
#include <QKeyEvent>
//#include <Q3ValueList>
#include <QMouseEvent>
#include "Constructs.h"
#include "Check.h"
#include "Comment.h"
#include "ExecUpdate.h"
#include "LavaAppBase.h"
#include "PEBaseDoc.h"
#include "Resource.h"
#include "ExecView.h"
#include "SylTraversal.h"
#include "PEBaseDoc.h"
#include "ExecFrame.h"
#include "qtextedit.h"
#include "qassistantclient.h"
#include "LavaBaseStringInit.h"
//#include "Resource.h"
#include "LavaExecsStringInit.h"
#include "BASEMACROS.h"

#pragma hdrstop
#include "debugStop.xpm"
#include "debugStopGreen.xpm"
#include "breakPoint.xpm"


#define IsPH(PTR) (!PTR || ((SynObject*)PTR)->IsPlaceHolder())
#define ADJUST(nnn,decl) \
  nnn.nINCL = myDoc->IDTable.IDTab[decl->inINCL]->nINCLTrans[nnn.nINCL].nINCL
#define ADJUST4(nnn) \
  nnn.nINCL = ckd.document->IDTable.IDTab[ckd.inINCL]->nINCLTrans[nnn.nINCL].nINCL

#define CONTEXTINI \
  text->ckd.iC = text->ckd.execIC; text->ckd.oC = text->ckd.execOC

void *focusWindow=0;
bool isExecView;

static SynObject *clipBoardObject=0;
static wxDocument *clipBoardDoc;

static unsigned ExecCount=0;
static CExecView *lastDebugStopExec=0;
/*
void dummy_func () {
        ASN1 *cid=new ASN1;
        CDPTDOD(PUT,cid,0,true);
}
*/
/////////////////////////////////////////////////////////////////////////////
// CExecView

//IMPLEMENT_DYNCREATE(CExecView, CRichEditView)


static bool ctrlPressed=false;
static bool shiftPressed=false;
static bool altPressed=false;


static void CopyUntil(ObjReference *oldRef,CHE *chpStop,ObjReference *newRef) {
  CHE *chp, *newChp;
  TDOD *oldTdod, *newTdod;
  bool isFirst=true;

  chp = (CHE*)oldRef->refIDs.first;
  do {
    oldTdod = (TDOD*)chp->data;
    newTdod = new TDODV(true);
    newChp = NewCHE(newTdod);
    *newTdod = *oldTdod;
    if (isFirst) {
      isFirst = false;
      newRef->refName = oldTdod->name;
    }
    else
      newRef->refName = newRef->refName + "." + oldTdod->name;
    newRef->refIDs.Append(newChp);
    if (chp == chpStop)
      break;
    else
      chp = (CHE*)chp->successor;
  } while (chp);
}


CExecView::CExecView(QWidget *parent,wxDocument *doc): CLavaBaseView(parent,doc,"ExecView")
{
  initialUpdateDone = false; // indicates whether OnInitialUpdate has already been executed
  makeSelectionVisible = false;
  sv = new MyScrollView(this);
  layout->addWidget(sv);
  layout->setMargin(0);
  redCtl = sv->execCont;
  sv->setFocusProxy(redCtl);
  sv->viewport()->setFocusProxy(redCtl);
  setFocusProxy(redCtl);
  QPalette palette=redCtl->palette();
  palette.setColor(QPalette::Active,QPalette::Window,Qt::white);
  redCtl->setPalette(palette);
  text = new CProgText;
  sv->text = text;
  m_ComboBar = ((CExecFrame*)GetParentFrame())->m_ComboBar;
  destroying = false;
  Base = 0;
  myDoc = 0;
  editCtlVisible = false;
  //redCtl->update();
}

CExecView::~CExecView()
{
  setFocusProxy(0);
  if (redCtl->debugStopToken)
    lastDebugStopExec = 0;
  OnCloseExec();
  delete text;
  if (lastDebugStopExec == this)
    lastDebugStopExec = 0;
}

bool CExecView::OnCreate()
{
  sv->setFont(LBaseData->m_ExecFont);
  return true;
}

void CExecView::OnCloseExec()
{
  LavaDECL *parent;
  CHE *chp;

  if (!wxTheApp->deletingMainFrame)
    DisableActions();
  --ExecCount;
  if (myDoc->mySynDef)
    ((SelfVar*)text->ckd.selfVar)->execView = 0;
  destroying = true;
  if (!ExecCount && wxTheApp->m_appWindow)
    wxTheApp->OnCloseLastExecView();
  if (!myDoc->deleting //&& myDoc->mySynDef // document is not yet closed
  && ((SelfVar*)myDECL->Exec.ptr)->IsEmptyExec()
  && (((SelfVar*)myDECL->Exec.ptr)->primaryToken == invariant_T
      || ((SelfVar*)myDECL->Exec.ptr)->primaryToken == require_T
      || ((SelfVar*)myDECL->Exec.ptr)->primaryToken == ensure_T)) {
    myDoc->SetExecItemImage(
      myDECL,
      true,
      false);
    parent = myDECL->ParentDECL;
    chp = myDoc->GetExecChe(parent, myDECL->DeclType);
    if (myDECL->WorkFlags.Contains(nonEmptyInvariant)) {
      parent->NestedDecls.Uncouple(chp);
      chp->data = 0; // to prevent deleting data
      delete chp;
    }
    else
      parent->NestedDecls.Remove(chp->predecessor);
  }
  Base->Browser->LastBrowseContext->RemoveView(this);
}


void CExecView::OnInitialUpdate()
{
  myDoc = (CPEBaseDoc*)GetDocument();
  Base = myDoc->GetLBaseData();
  CLavaPEHint* pHint = Base->actHint;
  Base->actHint = 0;
  myMainView = (wxView*)pHint->CommandData2;
  myDECL = (LavaDECL*)pHint->CommandData1;
  myExecCategory = myDECL->DeclType;
  statusBar = wxTheApp->m_appWindow->statusBar();
  myID = TID(myDECL->ParentDECL->OwnID, 0);
  forcePrimTokenSelect = false;
  clicked = false;
  insertBefore = false;
  escapePressed = false;
  doubleClick = false;
  multHint = 0;
  editCtl = 0;
  tempNo = 0;
  externalHint = false;
  nextError = false;
  deletePending = false;

  ExecCount++;
  errMsgUpdated = false;
  autoScroll = false;

////////////////////////////////////////////////////////////////////////////////////
// text:

  text->htmlGen = false;
  text->redCtl = sv->execCont;
  text->sv = sv;
  QFontInfo fi = redCtl->fontInfo();
  QString family = fi.family();
  int ptSize = fi.pointSize();
  QFontMetrics fm(redCtl->fontMetrics());
  redCtl->widthOfBlank = fm.width(" ");
  redCtl->widthOfIndent = fm.width("nn");
  text->showComments = myDECL->TreeFlags.Contains(ShowExecComments);
  text->ignored = false;
  if (myDECL->TreeFlags.Contains(leftArrows))
    text->leftArrows = true;
  else
    text->leftArrows = false;
  if (myDECL->TreeFlags.Contains(parmNames))
    text->parmNames = true;
  else
    text->parmNames = false;
  text->currentSynObjID = 0;
  text->document = myDoc;
  text->ckd.document = myDoc;
  text->ckd.myDECL = myDECL;
  text->ckd.execView = this;
  text->newLines = 0;
  text->insBlank = false;
////////////////////////////////////////////////////////////////////////////////////

  Tokens_INIT();
  new CLavaError(&plhChain,&ERR_Placeholder);

  selfVar = (SelfVar*)myDECL->Exec.ptr;
  selfVar->formParms.ptr = 0;
  selfVar->concernExecs = false;
  selfVar->execView= this;
  text->ckd.inInitialUpdate = true;
  initialUpdateDone = true;
  OnUpdate((wxView*)pHint->CommandData2, 0, pHint);
  sv->setFocusProxy(redCtl);
  text->ckd.inInitialUpdate = false;
}

void ExecContents::SetTokenFormat (CHETokenNode *currToken) {
  TToken token = currToken->data.token;

  fmt.bold = false;
  fmt.italic = false;

  if (token == Larrow_T
  || token == Rarrow_T) {
#if defined(WIN32) || defined(Linux) || defined(__Darwin)
    fmt.font.setFamily(fmt.symbolFamily);
    fmt.symbolFont = true;
#endif
#ifndef WIN32
    fmt.bold = true;
#endif
    fmt.color = QColor("#0000FF"); // blue
    if (currToken->data.flags.Contains(ignoreSynObj)) {
      fmt.italic = true;
      //fmt.color = QColor("#FF0000"); // red
      fmt.color = QColor("#00A000"); // green
    }
    else if (currToken == currToken->data.synObject->primaryTokenNode
    && currToken->data.synObject->lastError) {
      fmt.color = QColor("#FF0000"); // red
      fmt.bold = true; //fmt.dwEffects = cfe_bold;
    }
  }
  else if (token == NewLineSym_T) {
    fmt.bold = true;
    fmt.color = QColor("#0000FF"); // blue
    if (currToken->data.flags.Contains(ignoreSynObj)) {
      fmt.italic = true;
      //fmt.color = QColor("#FF0000"); // red
      fmt.color = QColor("#00A000"); // green
    }
    else if (currToken == currToken->data.synObject->primaryTokenNode
    && currToken->data.synObject->lastError) {
      fmt.color = QColor("#FF0000"); // red
      fmt.bold = true; //fmt.dwEffects = cfe_bold;
    }
  }
  else if (token == FuncRef_T
  || token == TypeRef_T
  || token == CrtblRef_T
  || token == enumConst_T
  ) {
    fmt.color = QColor("#000000"); // black
    if (token == FuncRef_T)
      fmt.bold = true;
    if (currToken->data.flags.Contains(ignoreSynObj)) {
      fmt.italic = true;
      //fmt.color = QColor("#FF0000"); // red
      fmt.color = QColor("#00A000"); // green
    }
    else if (currToken == currToken->data.synObject->primaryTokenNode
    && currToken->data.synObject->lastError) {
      fmt.color = QColor("#FF0000"); // red
      fmt.bold = true;
    }
  }
  else if (token == Comment_T) {
    fmt.italic = true;
    fmt.color = QColor("#00A000"); // green
  }
  else if (TOKENSTR[token][0].isLetter()
  || TOKENSTR[token][0]=='@'
  || TOKENSTR[token][0]=='#'
  || TOKENSTR[token][0]==':'
  || TOKENSTR[token][0]=='~') {
    if (currToken->data.flags.Contains(isOpt))
      fmt.color = QColor("#D1009E"); // pink
    else
      fmt.color = QColor("#0000FF"); // blue
    if (currToken->data.flags.Contains(ignoreSynObj)) {
      fmt.italic = true;
      //fmt.color = QColor("#FF0000"); // red
      fmt.color = QColor("#00A000"); // green
    }
    else if (currToken == currToken->data.synObject->primaryTokenNode
    && currToken->data.synObject->lastError) {
      fmt.color = QColor("#FF0000"); // red
      fmt.bold = true;
    }
    else if (currToken->data.flags.Contains(abstractPP))
      fmt.color = QColor("#FF0000"); // red
  }
  else if (currToken->data.synObject->IsPlaceHolder()
  && !currToken->data.flags.Contains(isDisabled)) {
    fmt.color = QColor("#FF0000"); // red
    if (currToken->data.flags.Contains(ignoreSynObj)) {
      fmt.italic = true;
      fmt.color = QColor("#00A000"); // green;
    }
    else if (currToken == currToken->data.synObject->primaryTokenNode
    && currToken->data.synObject->lastError)
      fmt.bold = true;
  }
  else {
    fmt.color = QColor("#000000"); // black
    if (currToken->data.flags.Contains(ignoreSynObj)) {
      fmt.italic = true;
      //fmt.color = QColor("#FF0000"); // red
      fmt.color = QColor("#00A000"); // green
    }
    else if (currToken->data.synObject->lastError
    && (currToken == currToken->data.synObject->primaryTokenNode
        || token == Rbracket_T)) {
      fmt.color = QColor("#FF0000"); // red
      fmt.bold = true;
    }
    else if (currToken->data.flags.Contains(isOpt))
      fmt.color = QColor("#D1009E"); // pink
  }
}

int ExecContents::calcIndent (CHETokenNode *currentToken) {
  SynObject *ancestor=currentToken->data.synObject;
        unsigned ind;

  while (ancestor && ancestor->startToken == currentToken)
    ancestor = ancestor->parentObject;
  if (ancestor)
    ind = ancestor->startPos+currentToken->data.indent*widthOfIndent;
  else
    ind = LEFTMARGIN+currentToken->data.indent*widthOfIndent;

        return ind;
}

void ExecContents::DrawToken (QPainter &p, CProgText *text, CHETokenNode *currentToken, bool inSelection) {
  int width, lineWidth=0, oldCW=contentsWidth, oldCH=contentsHeight;
  QPen currentPen;
  QColor currentColor;
  QFontMetrics *fm;
  QString line;
  int nl_pos, old_nl_pos, cmtWidth, startY, r, g, b;
  bool firstLine=true;
  SynObject *ancestor=currentToken->data.synObject;

  if (currentToken->data.flags.Contains(ignoreSynObj)
  && !text->showComments)
    return;

  SetTokenFormat(currentToken);

  if (inSelection && !inDebugStop) {
    currentColor = (QColor)fmt.color;
    currentColor.getRgb(&r,&g,&b);
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
    currentColor.setRgb(r,g,b);
    p.setPen(currentColor);
    p.setBackground(QBrush(Qt::black));
  }
  else
    p.setPen(fmt.color);

  if (fmt.bold)
    fmt.font.setBold(true);
  else
    fmt.font.setBold(false);
  if (fmt.italic)
    fmt.font.setItalic(true);
  else
    fmt.font.setItalic(false);

  p.setFont(fmt.font);
  fm = new QFontMetrics(p.fontMetrics());

  if (currentToken == ancestor->startToken)
          ancestor->startPos = -1;

  if (currentToken->data.newLines) {
    currentY += currentToken->data.newLines*fm->height();

    if (currentToken == currentToken->data.synObject->startToken)
      currentX = calcIndent(currentToken);
    else
      currentX = currentToken->data.synObject->startPos/*startToken->data.rect.left()*/
                 + currentToken->data.indent*widthOfIndent;
                text->lastIndent = currentX;
  }
  if (currentToken == debugStopToken)
    debugStopY = currentY-fm->ascent();
  else if (currentToken == callerStopToken)
    callerStopY = currentY-fm->ascent();
  if (inBreakPoint)
    breakPointY = currentY-fm->ascent();

  if (currentToken == currentToken->data.synObject->startToken)
    while (ancestor && ancestor->startPos == -1) {
      if (currentToken->data.token != Comment_T
      || ((TComment*)currentToken->data.synObject->comment.ptr)->trailing
      || !((TComment*)currentToken->data.synObject->comment.ptr)->inLine
      || ancestor != currentToken->data.synObject)
        ancestor->startPos = currentX;
      ancestor = ancestor->parentObject;
    }

  if (currentToken->data.token == Comment_T) {
    startY = currentY-fm->ascent();
    cmtWidth = 0;
    nl_pos = 0;

    old_nl_pos = -1;
    do {
      if (firstLine)
        firstLine = false;
      else {
        old_nl_pos = nl_pos;
        currentY += fm->height();
      }
      nl_pos = currentToken->data.str.indexOf('\n',old_nl_pos+1);
      if (nl_pos == -1) {
        line = currentToken->data.str.mid(old_nl_pos+1);
        if (line.length()) {
          p.drawText(QPoint(currentX,currentY),line);
          lineWidth=fm->width(line);
          contentsWidth = qMax(contentsWidth,currentX+lineWidth);
          cmtWidth = qMax(cmtWidth,lineWidth);
        }
        contentsHeight = qMax(contentsHeight,currentY+fm->descent());
        break;
      }
      else {
        line = currentToken->data.str.mid(old_nl_pos+1,nl_pos - old_nl_pos - 1);
        if (line.length()) {
          p.drawText(QPoint(currentX,currentY),line);
          lineWidth=fm->width(line);
          contentsWidth = qMax(contentsWidth,currentX+lineWidth);
          cmtWidth = qMax(cmtWidth,lineWidth);
        }
      }
    } while (true);
    currentToken->data.rect.setRect(currentX,startY,cmtWidth,currentY+fm->descent()-startY);
    currentX += cmtWidth;
  }

  else { // no comment token
    p.drawText(QPoint(currentX,currentY),currentToken->data.str);
    width=fm->width(currentToken->data.str);
    currentToken->data.rect.setRect(currentX,currentY-fm->ascent(),width,fm->height());
    currentX += width;
    contentsWidth = qMax(contentsWidth,currentX);
    contentsHeight = qMax(contentsHeight,currentY+fm->descent());
  }
  delete fm;
  if (fmt.symbolFont) {
    fmt.font.setFamily(fmt.fontFamily);
    fmt.symbolFont = false;
  }
}

typedef QList<int> BreakPointList;

bool ExecContents::event(QEvent *ev) {
  QHelpEvent *hev;
  QWhatsThisClickedEvent *wtcEv;
  QHelpEvent *hevWT;
  WhatNextEvent *wnev;
  QString href;

  if (ev->type() == QEvent::WhatsThis) {
    hev = (QHelpEvent*)ev;
    setWhatsThis(text(hev->pos()));
    return QWidget::event(ev);
  }
  else if (ev->type() == UEV_WhatNext) {
    wnev = (WhatNextEvent*)ev;
    hevWT = new QHelpEvent(QEvent::WhatsThis,
      wnev->pos+QPoint(100,18),
      wnev->globalPos+QPoint(100,18));
    return QWidget::event(hevWT);
  }
  else if (ev->type() == QEvent::WhatsThisClicked) {
    wtcEv = (QWhatsThisClickedEvent*)ev;
    href = wtcEv->href();
    ShowPage(QString("whatsThis/")+href);
    return true;
  }
  else if (ev->type() == QEvent::FocusIn) {
    if (execView->editCtlVisible)
      execView->editCtl->setFocus();
    return QWidget::event(ev);
  }
  else
    return QWidget::event(ev);
}

QString ExecContents::text(const QPoint &point) {
  execView->text->NewSel(&point);
  if (execView->text->newSelection) {
    wxDocManager::GetDocumentManager()->SetActiveView(execView,true);
    execView->Select();
    return execView->text->currentSynObj->whatsThisText();
  }
  return QString(tr("No specific help available here"));
}

static int nPaint=1;

void ExecContents::paintEvent (QPaintEvent *ev)
{
  fmt.fontFamily = fmt.font.family();
  QPainter p(this);

  CHETokenNode *currentToken;
  bool inSelection=false, debugStopOccurred=false;
  QPen myPen(Qt::NoPen);
  QFontMetrics *fm;
  BreakPointList bpl;
  BreakPointList::iterator it;

  if (!execView || !execView->myDoc || !execView->myDoc->mySynDef)
    return;
  p.setBackgroundMode(Qt::OpaqueMode);

  contentsWidth = 0;
  contentsHeight = 0;
  fmt.font = font();
  fm = new QFontMetrics(fontMetrics());//fmp;
  QFontInfo fi(fmt.font);
  currentX = LEFTMARGIN;
  currentY = fm->ascent();

  for (currentToken = (CHETokenNode*)execView->text->tokenChain.first;
       currentToken;
       currentToken = (CHETokenNode*)currentToken->successor) {

    inDebugStop = false;
    inBreakPoint = false;

    if (currentToken->data.flags.Contains(insertBlank)) {
      p.drawText(QPoint(currentX,currentY)," ");
      currentX += widthOfBlank;
    }

    if (!inSelection && currentToken == execView->selStartPos)
      inSelection = true;

    if (currentToken == debugStopToken || currentToken == callerStopToken) {
      inDebugStop = true;
      if (innermostStop && (stopReason == Stop_StepOut))
        p.setBackground(QBrush(QColor(180,180,255)));
      else
        p.setBackground(QBrush(QColor(255,255,0)));
    }
    if (currentToken->data.synObject->workFlags.Contains(isBrkPnt)
    && (currentToken->data.flags.Contains(primToken)
        || currentToken->data.token == assignFS_T
        || currentToken->data.token == if_T)) {
      inBreakPoint = true;
      if (!inDebugStop) {
        p.setBackground(QBrush(QColor(255,150,150)));
      }
    }
    else if (currentToken->data.token == if_T
    && currentToken->data.synObject->parentObject->workFlags.Contains(isBrkPnt)) {
      inBreakPoint = true;
      if (!inDebugStop) {
        p.setBackground(QBrush(QColor(255,130,180)));
      }
    }
    else if (inSelection && !inDebugStop) {
      p.setBackground(QBrush(Qt::black));
    }

    DrawToken(p,execView->text,currentToken,inSelection);

    if (inBreakPoint)
      bpl.append(breakPointY);

    if (inDebugStop) {
      inDebugStop = false;
      if (inSelection)
        if (currentToken == execView->selEndPos) {
          inSelection = false;
          p.setBackground(QBrush(Qt::white));
        }
        else {
          p.setBackground(QBrush(Qt::black));
        }
      else {
        p.setBackground(QBrush(Qt::white));
      }
    }
    else if (inBreakPoint) {
      inBreakPoint = false;
      if (inSelection)
        if (currentToken == execView->selEndPos) {
          inSelection = false;
          p.setBackground(QBrush(Qt::white));
        }
        else {
          p.setBackground(QBrush(Qt::black));
        }
      else {
        p.setBackground(QBrush(Qt::white));
      }
    }
    else if (inSelection)
      if (currentToken == execView->selEndPos) {
        inSelection = false;
        p.setBackground(QBrush(Qt::white));
      }
  }

  p.setBrush(QColor(210,210,210));
  p.setPen(myPen);
  contentsHeight = qMax(contentsHeight,sv->viewport()->height());
  contentsWidth = qMax(contentsWidth,miniEditRightEdge);
  resize(contentsWidth,contentsHeight);
  p.drawRect(0,0,15,contentsHeight);

  for ( it = bpl.begin(); it != bpl.end(); ++it )
    p.drawPixmap(0,*it+(fm->ascent()>14?fm->ascent()-14:0),*breakPoint);

  if (debugStopToken)
    p.drawPixmap(0,debugStopY+(fm->ascent()>14?fm->ascent()-14:0),*debugStop);
  if (callerStopToken)
    p.drawPixmap(0,callerStopY+(fm->ascent()>14?fm->ascent()-14:0),*debugStopGreen);

  if (execView->makeSelectionVisible) {
    execView->makeSelectionVisible = false;
    if (execView->autoScroll) {
      sv->ensureVisible(execView->selStartPos->data.rect.left(),execView->selStartPos->data.rect.top(),50,50);
      execView->autoScroll = false;
    }
  }
  else if (debugStopToken && execView->autoScroll) {
    sv->ensureVisible(debugStopToken->data.rect.left(),debugStopToken->data.rect.top(),50,50);
    execView->autoScroll = false;
  }
  else if (callerStopToken && execView->autoScroll) {
    sv->ensureVisible(callerStopToken->data.rect.left(),callerStopToken->data.rect.top(),50,50);
    execView->autoScroll = false;
  }
  delete fm;
}

void CExecView::OnUpdate(wxView*, unsigned undoRedo, QObject* pHint)
{
  // TODO: Add your specialized code here and/or call the base class
  LavaDECL *myNewDECL;
  CLavaPEHint *hint =(CLavaPEHint*)pHint;
  SET flags;
  QString str;
  CSearchData sData;
  bool isLastHint=false;

  externalHint = true; // to enforce UpdateParameters below and for combo-bar updates
  if (hint)
    switch (hint->com) {

    case CPECommand_OpenExecView:
      replacedObj = 0;
      toBeDrawn = 0;
      multipleUpdates = true; // to enforce RedrawExec
      break;

    case CPECommand_Exec:
      externalHint = false;
      if ((LavaDECL*)hint->CommandData1 != myDECL)
        return;
      if (hint->FirstLast.Contains(impliedExecHint)
      && undoRedo != 2)
        return;
      if (hint->FirstLast.Contains(firstHint)) {
        if (!hint->FirstLast.Contains(lastHint)) // nur firstHint
          if (hint->CommandData3 == (void*)InsMult) { // cannot be undo
            multHint = hint;
            return;
          }
          else { // must be InsFlags/DelFlags
            if (undoRedo != 1) return; // do/redo case
            // else draw
          }
      }
      else  // not firstHint
        if (hint->FirstLast.Contains(lastHint)) {
          if (undoRedo == 1)
            return; // undo case
          if (multHint) {
            hint = multHint;
            multHint = 0;
            isLastHint = true; // since lastHint flag is lost here!
          }
        }
        else
          return;

      SetSelectAtHint(hint);
      break;

    case CPECommand_Change:
      myNewDECL = myDoc->IDTable.GetDECL(myID);
      if (myNewDECL) {
        myNewDECL = myDoc->GetExecDECL(myNewDECL,myExecCategory,false,false);
        if (myNewDECL != myDECL) {
          myDECL = myNewDECL;
          text->INIT();
          Base->Browser->LastBrowseContext->RemoveView(this);
          text->ckd.myDECL = myDECL;
          selfVar = (SelfVar*)myDECL->Exec.ptr;
          selfVar->execDECL = myDECL;
          if (text->currentSynObjID) {
            sData.synObjectID = text->currentSynObjID;
            sData.execView = this;
            selfVar->MakeTable((address)&myDoc->IDTable, 0, (SynObjectBase*)myDECL, onSelect, 0,0, (address)&sData);
          }
          toBeDrawn = 0; // to enforce RedrawExec
        }
      }
      else {
        GetParentFrame()->closeMyPage();
        return;
      }
      break;

    case CPECommand_Delete:
      if (myDECL->isInSubTree((LavaDECL*)hint->CommandData1)) {
        if ((!hint->CommandData8 || (myDECL->ParentDECL->ParentDECL->DeclType == Interface))
            && ((   (undoRedo == 1 && hint->FirstLast.Contains(firstHint))
                 || (undoRedo != 1
                     && (hint->FirstLast.Contains(lastHint)
                         || hint->com == CPECommand_FromOtherDoc))
                )
                || undoRedo == 3)) {
          GetParentFrame()->closeMyPage();
          return;
        }
        else {
          if (hint->CommandData8 && (myDECL->ParentDECL->ParentDECL->DeclType == Interface))
            GetParentFrame()->closeMyPage();
          else
            deletePending = true;
          return;
        }
      }
      break;

    default: ;
    }

  if (( hint
        && (hint->com == CPECommand_OpenExecView
            || hint->com == CPECommand_Exec
            || hint->com == CPECommand_Change
            || hint->com == CPECommand_ChangeInclude
            || hint->com == CPECommand_Delete
            || hint->com == CPECommand_FromOtherDoc
            || hint->com == CPECommand_Exclude
            || hint->com == CPECommand_Include
            || hint->com == CPECommand_Insert
           )
        && (
             (undoRedo == 1 && hint->FirstLast.Contains(firstHint))
             || (undoRedo != 1
                 && (hint->FirstLast.Contains(lastHint)
                    || hint->com == CPECommand_FromOtherDoc))
           )
      )
      || undoRedo == 3
      || isLastHint) {

    if (deletePending) {
      deletePending = false;
      myNewDECL = myDoc->IDTable.GetDECL(myID);
      if (myNewDECL) {
        myNewDECL = myDoc->GetExecDECL(myNewDECL,myExecCategory,false,false);
        if (myNewDECL && (myNewDECL != myDECL)) {
          myDECL = myNewDECL;
          text->INIT();
          Base->Browser->LastBrowseContext->RemoveView(this);
          text->ckd.myDECL = myDECL;
          selfVar = (SelfVar*)myDECL->Exec.ptr;
          selfVar->execDECL = myDECL;
          if (text->currentSynObjID) {
            sData.synObjectID = text->currentSynObjID;
            sData.execView = this;
            selfVar->MakeTable((address)&myDoc->IDTable, 0, (SynObjectBase*)myDECL, onSelect, 0,0, (address)&sData);
          }
          toBeDrawn = 0; // to enforce RedrawExec
        }
        else
          if(!myNewDECL) {
            GetParentFrame()->closeMyPage();
            return;
          }
      }
      else {
        GetParentFrame()->closeMyPage();
        return;
      }
    }

    ((CExecFrame*)GetParentFrame())->m_ComboBar->OnUpdate(myDECL,externalHint);

    if (externalHint)
      ((CExecFrame*)GetParentFrame())->NewTitle(myDECL);

    text->ckd.hint = hint;
    text->ckd.undoRedo = undoRedo;

    Check();
    sData.execView = this;
    sData.doc = myDoc;
    sData.nextFreeID = 0;
    sData.execDECL = myDECL;
    selfVar->MakeTable((address)&myDoc->IDTable, 0, (SynObjectBase*)myDECL, onSetSynOID, 0,0, (address)&sData);
    RedrawExec(text->selectAt);
    //selfVar->oldFormParms = (FormParms*)selfVar->formParms.ptr;

    toBeDrawn = 0;
    multipleUpdates = false;
    externalHint = false;
    if (hint && hint->com == CPECommand_OpenExecView)
      delete hint;
    //if (active)
    //  redCtl->setFocus();
  }
}

void MyScrollView::focusInEvent(QFocusEvent *ev) {
  if (execView->initialUpdateDone
  && !execView->myDoc->deleting)
    execView->SetHelpText();
  //execView->wxView::focusInEvent(ev);
}

void ExecContents::keyPressEvent (QKeyEvent *e) {
  execView->OnChar(e);
}

void CExecView::OnChar(QKeyEvent *e)
{
  // TODO: Add your message handler code here and/or call default
  int key=e->key();
  Qt::KeyboardModifiers state=e->modifiers();
  ctrlPressed = (state & Qt::ControlModifier);
  SynObject *currentSynObj, *parent/*, *ocl*/;

  if (LBaseData->debugger->isConnected)
    switch (key) {
    case Qt::Key_Tab:
      if (state & Qt::ShiftModifier) // Shift key down ==> BACKTAB
        text->newSelection = text->FindPrecedingPlaceholder();
      else // TAB
        text->newSelection = text->FindNextPlaceholder();
      Select();
      break;
    case Qt::Key_Up:
      currentSynObj = text->currentSynObj;
      parent = currentSynObj->parentObject;
      if (parent && parent->primaryToken == ObjRef_T
      && ((ObjReference*)parent)->refIDs.first == ((ObjReference*)parent)->refIDs.last) {
        currentSynObj = parent;
        parent = currentSynObj->parentObject;
      }
    case Qt::Key_Down:
      if (text->currentSynObj->primaryToken == parameter_T)
        currentSynObj = (SynObject*)((Parameter*)text->currentSynObj)->parameter.ptr;
      else
        currentSynObj = text->currentSynObj;

      if (currentSynObj->type == VarPH_T
      || currentSynObj->primaryToken == Exp_T
      || currentSynObj->IsConstant()) {
        text->currentSynObj = currentSynObj;
        if (text->currentSynObj->primaryToken != enumConst_T
        && text->currentSynObj->primaryToken != Boolean_T
        && !text->currentSynObj->BoolAdmissibleOnly(text->ckd)
        && !text->currentSynObj->EnumAdmissibleOnly(text->ckd))
          doubleClick = true;
        Select(text->currentSynObj);
        doubleClick = false;
      }
      else if (text->currentSelection->data.token == Comment_T) {
        doubleClick = true;
        clicked = true;
        Select();
      }
      else if (!IsPH(currentSynObj)
      && currentSynObj->primaryToken != TDOD_T
      && currentSynObj->primaryToken != FuncRef_T
      && currentSynObj->primaryToken != TypeRef_T
      && currentSynObj->primaryToken != CrtblRef_T) {
        text->currentSynObj = currentSynObj;
        Select(FirstChild());
      }
      break;
    case Qt::Key_Left:
      Select(LeftSibling());
      break;
    case Qt::Key_Right:
      Select(RightSibling());
      break;
    case Qt::Key_Return:
      if (GetDocument()->changeNothing)
        return;
      if (EnableInsert()) {
        OnInsert();
        if (text->currentSelection->data.token == VarPH_T
        || text->currentSelection->data.token == Exp_T) {
          doubleClick = true;
          Select();
          doubleClick = false;
        }
      }
      else if (text->currentSynObj->IsStatement())
        OnAnd();
      break;
    case Qt::Key_F1:
      ((wxMainFrame*)wxTheApp->m_appWindow)->helpContents();
      break;
    case Qt::Key_Escape:
      break;
    default:
      ;
    }
  else
    switch (key) {

    case Qt::Key_A:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnAnd();
      break;
    case Qt::Key_C:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnCopy();
      break;
    case Qt::Key_D:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection)
      && text->currentSynObj->ReadOnlyContext() != roClause)
        OnDeclare();
      break;
    case Qt::Key_E:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnExists();
      break;
    case Qt::Key_F:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnForeach();
      break;
    case Qt::Key_I:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnIf();
      break;
    case Qt::Key_L:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnSelect();
      break;
    case Qt::Key_N:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnCreateObject();
      break;
    case Qt::Key_O:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnOr();
      break;
    case Qt::Key_Q:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnQueryItf();
      break;
    case Qt::Key_R:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnCall();
      break;
    case Qt::Key_S:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection)
      && text->currentSynObj->ReadOnlyContext() != roClause)
        OnAssign();
      break;
    case Qt::Key_T:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnTypeSwitch();
      break;
    case Qt::Key_W:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnSwitch();
      break;
    case Qt::Key_X:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnIfExpr();
      break;

    case Qt::Key_Equal:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnEq();
      break;
    case Qt::Key_Asterisk:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnMult();
      break;
    case Qt::Key_Plus:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnPlus();
      break;
    case Qt::Key_Minus:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnPlusMinus();
      break;
    case Qt::Key_Slash:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnDivide();
      break;
    case Qt::Key_Question:
      if (!Taboo() && (text->currentSynObj->StatementSelected(text->currentSelection)
      || (text->currentSynObj->ExpressionSelected(text->currentSelection) && text->currentSynObj->BoolAdmissibleOnly(text->ckd))))
        OnEvaluate();
      break;
    case Qt::Key_0:
      if (!Taboo()
      && ((text->currentSynObj->ExpressionSelected(text->currentSelection)
           && text->currentSynObj->NullAdmissible(text->ckd))
          || text->currentSynObj->IsOutputParam()))
        OnNull();
      break;
    case Qt::Key_Percent:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnModulus();
      break;
    case Qt::Key_Ampersand:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnBitAnd();
      break;
    case Qt::Key_At:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnHandle();
      break;
    case Qt::Key_NumberSign:
      if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnNe();
      else if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnOrd();
      break;
    case Qt::Key_Exclam:
      if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
        OnInvert();
      else if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
        OnNot();
      break;
    case Qt::Key_1:
      if (ctrlPressed) {
        CComboBar* bar = (CComboBar*)((CExecFrame*)GetParentFrame())->m_ComboBar;
        if (bar->LeftCombo)
          bar->LeftCombo->showPopup();
      }
      break;
    case Qt::Key_2:
      if (ctrlPressed) {
        CComboBar* bar = (CComboBar*)((CExecFrame*)GetParentFrame())->m_ComboBar;
        if (bar->RightCombo)
          bar->RightCombo->showPopup();
      }
      break;
    case Qt::Key_3:
      if (ctrlPressed) {
        CComboBar* bar = (CComboBar*)((CExecFrame*)GetParentFrame())->m_ComboBar;
        if (bar->EnumsEnable) {
          bar->TrackEnum();
        }
        else {
          if (bar->RightCombo && bar->ThirdCombo)
            bar->ThirdCombo->showPopup();
        }
      }
      break;
    case '\xe2':
      if (altPressed)
        if (shiftPressed) {
          if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
            OnGe();
        }
        else if (ctrlPressed) {
          if (!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection))
            OnBitOr();
        }
        else {
          if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
            OnLe();
        }
      else
        if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection))
          if (shiftPressed)
            OnGt();
          else
            OnLt();
      break;
    case Qt::Key_Delete: // 0x2e DEL key
    case Qt::Key_Backspace: // 0x08 BACKSPACE key

      if (GetDocument()->changeNothing)
        return;
      if (text->currentSynObj->primaryToken == parameter_T)
        return;
      if ((Ignorable() && !inIgnore)
      && !(inBaseInits
           && text->currentSynObj->primaryToken == FuncRef_T
           && text->currentSynObj->parentObject->parentObject->primaryToken == initializing_T))
        return;
      if (!text->currentSynObj->parentObject->parentObject
      && text->currentSynObj->IsPlaceHolder())
        return;
      on_editCutAction_triggered = false;
      OnDelete();
      return;
    case Qt::Key_Tab:
      if (state & Qt::ShiftModifier) // Shift key down ==> BACKTAB
        text->newSelection = text->FindPrecedingPlaceholder();
      else // TAB
        text->newSelection = text->FindNextPlaceholder();
      Select();
      break;
    case Qt::Key_Up:
      currentSynObj = text->currentSynObj;
      parent = currentSynObj->parentObject;
      if (parent && parent->primaryToken == ObjRef_T
      && ((ObjReference*)parent)->refIDs.first == ((ObjReference*)parent)->refIDs.last
      && !currentSynObj->comment.ptr) {
        currentSynObj = parent;
        parent = currentSynObj->parentObject;
      }

      if (parent
      && parent->primaryToken == parameter_T) {
        currentSynObj = parent;
        parent = currentSynObj->parentObject;
      }
      Select(parent);
      break;
    case Qt::Key_Down:
      if (text->currentSynObj->primaryToken == parameter_T)
        currentSynObj = (SynObject*)((Parameter*)text->currentSynObj)->parameter.ptr;
      else
        currentSynObj = text->currentSynObj;

      if (currentSynObj->type == VarPH_T
      || currentSynObj->primaryToken == Exp_T
      || currentSynObj->IsConstant()) {
        if (GetDocument()->changeNothing)
          return;
        text->currentSynObj = currentSynObj;
        if (text->currentSynObj->primaryToken != enumConst_T
        && text->currentSynObj->primaryToken != Boolean_T
        && !text->currentSynObj->BoolAdmissibleOnly(text->ckd)
        && !text->currentSynObj->EnumAdmissibleOnly(text->ckd))
          doubleClick = true;
        Select(text->currentSynObj);
        doubleClick = false;
      }
      else if (text->currentSelection->data.token == Comment_T) {
        if (GetDocument()->changeNothing)
          return;
        doubleClick = true;
        clicked = true;
        Select();
      }
      else if (!IsPH(currentSynObj)
      && currentSynObj->primaryToken != TDOD_T
      && currentSynObj->primaryToken != FuncRef_T
      && currentSynObj->primaryToken != TypeRef_T
      && currentSynObj->primaryToken != CrtblRef_T) {
        text->currentSynObj = currentSynObj;
        Select(FirstChild());
      }
      break;
    case Qt::Key_Left:
      Select(LeftSibling());
      break;
    case Qt::Key_Right:
      Select(RightSibling());
      break;
    case Qt::Key_Return:
      if (GetDocument()->changeNothing)
        break;
      if (EnableInsert()) {
        OnInsert();
      }
      else if (text->currentSynObj->IsStatement())
        OnAnd();
      break;
    case Qt::Key_F1:
      ((wxMainFrame*)wxTheApp->m_appWindow)->helpContents();
      break;
    case Qt::Key_Escape:
      break;
    default:
      ;
    }
}

void MyScrollView::mousePressEvent (QMouseEvent *e) {
  if (e->button() != Qt::LeftButton)
    return;
  execView->OnLButtonDown(e);
}

void MyScrollView::mouseDoubleClickEvent (QMouseEvent *e) {
  if (e->button() != Qt::LeftButton)
    return;
  execView->OnLButtonDblClk(e);
}

MyScrollView::MyScrollView (QWidget *parent) : QScrollArea(parent) {
  execView = (CExecView*)parent;
  execCont = new ExecContents(this);
  setBackgroundRole(QPalette::Base);
  setWidget(execCont);
}

ExecContents::ExecContents (MyScrollView *sv) {
  //setFocusPolicy(Qt::ClickFocus);
  this->sv = sv;
  execView = sv->execView;
  setWhatsThis(tr("No specific help available here"));
  debugStop = new QPixmap((const char**)debugStop_xpm);
  debugStopGreen = new QPixmap((const char**)debugStopGreen_xpm);
  breakPoint = new QPixmap((const char**)breakPoint_xpm);
  debugStopToken = 0;
  callerStopToken = 0;
  miniEditRightEdge = 0;
  repaintAppWindow = true;
#ifdef WIN32
  fmt.symbolFamily = "Wingdings";
#else
  fmt.symbolFamily = "Symbol";
#endif
  fmt.symbolFont = false;
  resize(10,10);
}


void CExecView::OnLButtonDown(QMouseEvent *e)
{
  // TODO: Add your message handler code here and/or call default
  QPoint pos=e->pos();
  pos = sv->viewport()->mapTo(sv,pos);
  pos = redCtl->mapFrom(sv,pos);

  clicked = true;
  if (EditOK()) {
    text->NewSel(&pos);
    if (!LBaseData->debugger->isConnected
    && text->newSelection == text->currentSelection
    && (text->currentSelection->data.synObject->primaryToken == VarPH_T
        || (text->currentSelection->data.synObject->primaryToken == Exp_T
            && !text->currentSelection->data.synObject->BoolAdmissibleOnly(text->ckd)
            && !text->currentSelection->data.synObject->EnumAdmissibleOnly(text->ckd))
        || text->currentSelection->data.synObject->primaryToken == VarName_T
        || text->currentSelection->data.token == Comment_T
        || (text->currentSelection->data.token == Const_T
            && !text->currentSelection->data.synObject->BoolAdmissibleOnly(text->ckd)
            && !text->currentSelection->data.synObject->EnumAdmissibleOnly(text->ckd)))) {
      doubleClick = true;
    }
    Select();
    setFocus();
  }
  doubleClick = false;
  clicked = false;
}

void CExecView::OnLButtonDblClk(QMouseEvent *e)
{
  // TODO: Add your message handler code here and/or call default

  if (LBaseData->debugger->isConnected) return;

  doubleClick = true;
  OnLButtonDown(e);
  doubleClick = false;
}

void CExecView::Select (SynObject *selObj)
{
  QString str, str2, msg;
  QPoint luCorner;
  QFont *pFont=&LBaseData->m_ExecFont;
  SynObject *synObj, *typeSwitchExpression, *typeRef, *funcRef;
  Expression *callExpr;
  ObjReference *objRef;
  CComment *pComment;
  TDOD *pTID;
  TIDType idtype;
  TID tid, tidOperatorFunc;
  DWORD dw;
  FuncExpression *funcExpr;
  Assignment *assigStm;
  CopyStatement *copyStm;
  Connect *connStm;
  Disconnect *disconnStm;
  MultipleOp *multOpExp;
  IfExpression *ifx;
  CHE *chpFormIn;
  unsigned iInp=1, iOut=1;
  LavaDECL *decl, *declSig, *sigFuncDecl, *finalDecl, *declSwitchExpression;
  Category cat, catSwitchExpression;
  CContext nullCtx, callCtx, callCtxSig;
  SynFlags ctxFlags;
  bool isSigFunc;

 if (selObj) {
    if (selObj->InHiddenIniClause(text->ckd,typeRef))
      text->Select(typeRef);
    else if (selObj->primaryToken == parameter_T)
      text->Select((SynObject*)((Parameter*)selObj)->parameter.ptr);
    else if (selObj->type == elsif_T) {
      if (selObj->parentObject->primaryToken == if_T)
        text->newSelection = ((IfThen*)selObj)->thenToken;
      else
        text->newSelection = ((IfxThen*)selObj)->thenToken;
      text->Select();
    }
    else {
      text->newSelection = selObj->primaryTokenNode;
      text->Select(selObj);
    }
  }
  else if (forcePrimTokenSelect && text->selectAt) {
    forcePrimTokenSelect = false;
    text->newSelection = text->selectAt->primaryTokenNode;
    text->Select();
  }
  else
    text->Select();

  autoScroll = true;
  makeSelectionVisible = true;

  ObjComboUpdate ocUpd(this);
  ocUpd.CheckLocalScope(text->ckd,text->currentSynObj);
  tempNo = ocUpd.tempNo;
  inExecHeader = ocUpd.inExecHeader;
  inFormParms = ocUpd.inFormParms;
  inBaseInits = ocUpd.inBaseInits;
  inIgnore = ocUpd.inIgnore;
  inParameter = ocUpd.inParameter;
  inForeach = ocUpd.inForeach;

  if (active) {
    redCtl->setFocus();
    wxTheApp->updateButtonsMenus();
  }

  SetHelpText();
  //redCtl->update();

  if (text->currentSelection->data.token == Comment_T) {
    if (!EditOK() || !clicked) return;
    clicked = false;

    if (doubleClick) {
      doubleClick = false;

      pComment = new CComment(this);
      synObj = text->currentSynObj;
      if (synObj->comment.ptr) {
        pComment->inline_comment->setChecked(synObj->comment.ptr->inLine);
        pComment->trailing_comment->setChecked(synObj->comment.ptr->trailing);
        pComment->text->setPlainText(synObj->comment.ptr->str.c);
      }

      pComment->exec();
      if (pComment->result() == QDialog::Accepted) {
        TComment *pCmt = new TCommentV;
        pCmt->inLine = pComment->inline_comment->isChecked();
        pCmt->trailing = pComment->trailing_comment->isChecked();
        pCmt->str = STRING(qPrintable(pComment->text->document()->toPlainText()));
        text->currentSynObj = synObj;
        PutChgCommentHint(pCmt);
        if (synObj->comment.ptr) {
          if (synObj->comment.ptr->trailing)
            text->newSelection = synObj->endToken;
          else
            text->newSelection = synObj->startToken;
          text->Select();
        }
      }
      delete pComment;

    }
    redCtl->update();
    return;
  }

  if (text->currentSelection->data.token == Tilde_T)
    text->currentSelection = text->currentSynObj->primaryTokenNode;

  switch (text->currentSynObj->primaryToken) {
  case FuncRef_T:
    if (doubleClick && EnableGotoDecl()) {
      doubleClick = false;
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      OnGotoDecl();
      redCtl->update();
      return;
    }

  case FuncPH_T:
    if (text->currentSynObj->parentObject->parentObject
    && text->currentSynObj->parentObject->parentObject->primaryToken == new_T
    && text->currentSynObj->parentObject->whereInParent
       == (address)&((NewExpression*)text->currentSynObj->parentObject->parentObject)->initializerCall.ptr) {
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(newCombo);
      redCtl->update();
      return;
    }
    else if (text->currentSynObj->parentObject->parentObject
    && text->currentSynObj->parentObject->parentObject->primaryToken == declare_T
    && ((Declare*)text->currentSynObj->parentObject->parentObject)->secondaryClause.ptr
    && (text->currentSynObj->parentObject->whereInParent ==
       (address)&((Declare*)text->currentSynObj->parentObject->parentObject)->secondaryClause.ptr)) {
      funcExpr = (FuncExpression*)text->currentSynObj->parentObject;
      objRef = (ObjReference*)funcExpr->handle.ptr;
      tid = TID(objRef->myFinalVType->OwnID,objRef->myFinalVType->inINCL);
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassInis(tid);
      //((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(newCombo);
      redCtl->update();
      return;
    }
    else if (text->currentSynObj->parentObject->parentObject
    && text->currentSynObj->parentObject->parentObject->parentObject
    && text->currentSynObj->parentObject->parentObject->parentObject->primaryToken == declare_T
    && ((Declare*)text->currentSynObj->parentObject->parentObject->parentObject)->secondaryClause.ptr
    && (text->currentSynObj->parentObject->parentObject->whereInParent ==
       (address)&((Declare*)text->currentSynObj->parentObject->parentObject->parentObject)->secondaryClause.ptr)) {
      funcExpr = (FuncExpression*)text->currentSynObj->parentObject;
      objRef = (ObjReference*)funcExpr->handle.ptr;
      tid = TID(objRef->myFinalVType->OwnID,objRef->myFinalVType->inINCL);
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassInis(tid);
      //((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(newCombo);
      redCtl->update();
      return;
    }

    if (text->currentSynObj->parentObject->primaryToken == connect_T
    || (text->currentSynObj->parentObject->parentObject
        && text->currentSynObj->parentObject->parentObject->primaryToken == connect_T)) { // connect/disconnect
      if (text->currentSynObj->parentObject->primaryToken == connect_T) {
        connStm = (Connect*)text->currentSynObj->parentObject;
        callExpr = (Expression*)connStm->signalSender.ptr;
        funcRef = (SynObject*)connStm->signalFunction.ptr;
        isSigFunc = true;
      }
      else {
        connStm = (Connect*)text->currentSynObj->parentObject->parentObject;
        callExpr = (Expression*)((FuncStatement*)connStm->callback.ptr)->handle.ptr;
        funcRef = (SynObject*)((FuncStatement*)connStm->callback.ptr)->function.ptr;
        isSigFunc = false;
      }
      if (IsPH(callExpr) && funcRef->flags.Contains(isDisabled))
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      else {
        if (callExpr)
          if (callExpr->flags.Contains(isSelfVar)
          && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last) {
            decl = text->ckd.document->IDTable.GetDECL(selfVar->typeID,text->ckd.inINCL);
            callCtx = text->ckd.lpc;
          }
          else {
            callExpr->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
            callCtx = text->ckd.tempCtx;
            decl = text->ckd.document->GetTypeAndContext(decl,callCtx);
            text->ckd.document->NextContext(decl,callCtx);
          }
        else // sender class specified
          if (((SynObject*)connStm->signalSenderClass.ptr)->primaryToken == TypeRef_T)
            decl = ((Reference*)connStm->signalSenderClass.ptr)->refDecl;
          else
            decl = 0;
        if (decl)
          if (isSigFunc)
            ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassFuncs(text->ckd,decl,0,callCtx,false,true);
          else { // slot function
//            callExpr = (Expression*)connStm->signalReceiver.ptr;
            if (((SynObject*)((FuncStatement*)connStm->callback.ptr)->function.ptr)->flags.Contains(isDisabled))
              ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
            else {
/*              callExpr->ExprGetFVType(text->ckd,declSig,cat,ctxFlags);
              callCtxSig = text->ckd.tempCtx;
              declSig = text->ckd.document->GetTypeAndContext(declSig,callCtxSig);
              text->ckd.document->NextContext(declSig,callCtxSig);
              text->ckd.tempCtx = callCtxSig;*/
//              ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowSlotFuncs(text->ckd,sigFuncDecl);
              ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassFuncs(text->ckd,decl,((Reference*)connStm->signalFunction.ptr)->refDecl,callCtx,false);
            }
          }
        else if (!callExpr)
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowSignalFuncs(text->ckd);
        else
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      }
    }

    else if (text->currentSynObj->parentObject->IsFuncInvocation()) {
      funcExpr = (FuncExpression*)text->currentSynObj->parentObject;
      callExpr = (Expression*)funcExpr->handle.ptr;
      if (!funcExpr->parentObject) {
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
        redCtl->update();
        return;
      }
      if (funcExpr->parentObject->primaryToken == initializing_T) { // base initializer call
        tid = ((Reference*)((BaseInit*)funcExpr->parentObject)->baseItf.ptr)->refID;
        if (tid.nID >= 0)
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassInis(tid);
        else
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
        redCtl->update();
        return;
      }
      if (callExpr) {
        if (callExpr->IsPlaceHolder())
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
        else {
          if (callExpr->flags.Contains(isSelfVar)
          && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last) {
            decl = text->ckd.document->IDTable.GetDECL(selfVar->typeID,text->ckd.inINCL);
            callCtx = text->ckd.lpc;
          }
          else {
            callExpr->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
            callCtx = text->ckd.tempCtx;
            decl = text->ckd.document->GetTypeAndContext(decl,callCtx);
            text->ckd.document->NextContext(decl,callCtx);
          }
          if (decl)
            if (callExpr->flags.Contains(isSelfVar)
            && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last)
              if (funcExpr->parentObject->primaryToken == signal_T)
                ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassFuncs(text->ckd,decl,0,callCtx,false,true);
              else
                ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassFuncs(text->ckd,decl,0,callCtx,true);
            else
              ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassFuncs(text->ckd,decl,0,callCtx);
          else
            ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
        }
      }
      else //static function
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowStaticFuncs(text->ckd);
    }
    else if (text->currentSynObj->parentObject->primaryToken == disconnect_T) { // connect/disconnect
disconn:
      disconnStm = (Disconnect*)text->currentSynObj->parentObject;
      if (text->currentSynObj->whereInParent == (address)&disconnStm->signalFunction.ptr) {
        callExpr = (Expression*)disconnStm->signalSender.ptr;
        isSigFunc = true;
      }
      else {
        callExpr = (Expression*)disconnStm->signalReceiver.ptr;
        isSigFunc = false;
      }
      if (callExpr->IsPlaceHolder())
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      else {
        if (callExpr->flags.Contains(isSelfVar)
        && ((ObjReference*)callExpr)->refIDs.first == ((ObjReference*)callExpr)->refIDs.last) {
          decl = text->ckd.document->IDTable.GetDECL(selfVar->typeID,text->ckd.inINCL);
          callCtx = text->ckd.lpc;
        }
        else {
          callExpr->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
          if (decl == (LavaDECL*)-1)
            decl = 0;
          else {
            callCtx = text->ckd.tempCtx;
            decl = text->ckd.document->GetTypeAndContext(decl,callCtx);
            text->ckd.document->NextContext(decl,callCtx);
          }
        }
        if (decl)
          if (isSigFunc)
//            ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowSignalFuncs(text->ckd);
            ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassFuncs(text->ckd,decl,0,callCtx,false,true);
          else { // slot function
            callExpr = (Expression*)disconnStm->signalSender.ptr;
            callExpr->ExprGetFVType(text->ckd,declSig,cat,ctxFlags);
            if (declSig != (LavaDECL*)-1) {
              callCtxSig = text->ckd.tempCtx;
              declSig = text->ckd.document->GetTypeAndContext(declSig,callCtxSig);
              text->ckd.document->NextContext(declSig,callCtxSig);
            }
            if (((SynObject*)disconnStm->callbackFunction.ptr)->flags.Contains(isDisabled))
              ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
            else {
              if (((SynObject*)disconnStm->signalFunction.ptr)->primaryToken == FuncRef_T)
                sigFuncDecl = ((Reference*)disconnStm->signalFunction.ptr)->refDecl;
              else
                sigFuncDecl = 0;
              text->ckd.tempCtx = callCtxSig;
              ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowClassFuncs(text->ckd,decl,sigFuncDecl,callCtx,false);
            }
          }
        else {
          if (((SynObject*)disconnStm->signalFunction.ptr)->primaryToken == nil_T)
            sigFuncDecl = 0;
          else
            sigFuncDecl = ((Reference*)disconnStm->signalFunction.ptr)->refDecl;
          if (isSigFunc)
            ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowSignalFuncs(text->ckd);
          else if (((SynObject*)disconnStm->callbackFunction.ptr)->flags.Contains(isDisabled))
            ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
          else
            ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowSlotFuncs(text->ckd,sigFuncDecl);
        }
      }
    }

    redCtl->update();
    return;

  case nil_T:
    if (text->currentSynObj->IsOutputParam())
      goto obj;
    else if (text->currentSynObj->parentObject->primaryToken == disconnect_T
    && text->currentSynObj->replacedType == FuncPH_T)
      goto disconn;
    else
      goto exp;

  case ObjPH_T:
obj:
    if (text->currentSynObj->parentObject->parentObject->primaryToken == assignFS_T) {
      if (text->currentSynObj->whereInParent
          != (address)&((FuncExpression*)text->currentSynObj->parentObject->parentObject)->handle.ptr) {
        funcExpr = (FuncExpression*)text->currentSynObj->parentObject->parentObject;
        decl = myDoc->IDTable.GetDECL(((Parameter*)text->currentSynObj->parentObject)->formParmID,text->ckd.inINCL);
        if (funcExpr->handle.ptr) {
          text->ckd.tempCtx = funcExpr->callCtx;
          if (funcExpr->myCtxFlags.bits)
            text->ckd.tempCtx.ContextFlags = funcExpr->myCtxFlags;
        }
        else
          text->ckd.tempCtx = text->ckd.lpc;
        finalDecl = myDoc->GetFinalMVType(decl->RefID,decl->inINCL,text->ckd.tempCtx,cat,0);
        if (finalDecl) {
          if (decl->TypeFlags.Contains(substitutable))
            text->ckd.tempCtx.ContextFlags = SET(multiContext,-1);
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,finalDecl,text->ckd.tempCtx,cat,false);
          redCtl->update();
          return;
        }
      }
    }
    else if (text->currentSynObj->parentObject->primaryToken == assign_T) {
      assigStm = (Assignment*)text->currentSynObj->parentObject;
      ((SynObject*)assigStm->exprValue.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      if (decl && decl != (LavaDECL*)-1) {
        if (ctxFlags.bits)
          text->ckd.tempCtx.ContextFlags = ctxFlags;
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,decl,text->ckd.tempCtx,cat,false);
        redCtl->update();
        return;
      }
    }
/*    else if (text->currentSynObj->parentObject->primaryToken == copy_T) {
      copyStm = (CopyStatement*)text->currentSynObj->parentObject;
      ((SynObject*)copyStm->fromObj.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      if (decl) {
        if (ctxFlags.bits)
          text->ckd.tempCtx.ContextFlags = ctxFlags;
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,decl,text->ckd.tempCtx,cat,false,true);
        redCtl->update();
        return;
      }
    }*/
    ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objCombo);
    redCtl->update();
    return;

  case Exp_T:
  case Lbracket_T:
  case Rbracket_T:
  case Boolean_T:
  case ifx_T:
exp: // Const_T
    if (text->currentSynObj->parentObject->primaryToken == parameter_T) {
      decl = myDoc->IDTable.GetDECL(((Parameter*)text->currentSynObj->parentObject)->formParmID,text->ckd.inINCL);
      funcExpr = (FuncExpression*)text->currentSynObj->parentObject->parentObject;
      if (funcExpr->handle.ptr)
        text->ckd.tempCtx = funcExpr->callCtx;
      else
        text->ckd.tempCtx = text->ckd.lpc;
      finalDecl = myDoc->GetFinalMVType(decl->RefID,decl->inINCL,text->ckd.tempCtx,cat,0);
      if (finalDecl) {
        if (decl->TypeFlags.Contains(substitutable))
          text->ckd.tempCtx.ContextFlags = SET(multiContext,-1);
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,finalDecl,text->ckd.tempCtx,cat,true);
      }
      else
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objEnumCombo);
    }
    else if (text->currentSelection->data.token == Exp_T
    && text->currentSynObj->parentObject->primaryToken == EvalStm_T) {
      TID tidBool=TID(text->ckd.document->IDTable.BasicTypesID[B_Bool],myDoc->isStd?0:1);
      decl = myDoc->IDTable.GetDECL(tidBool);
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,decl,nullCtx,valueObjectCat,true);
    }
    else if (text->currentSelection->data.token == Exp_T
    && text->currentSynObj->parentObject->primaryToken == item_T) {
      TID tidInteger=TID(text->ckd.document->IDTable.BasicTypesID[Integer],myDoc->isStd?0:1);
      decl = myDoc->IDTable.GetDECL(tidInteger);
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,decl,nullCtx,valueObjectCat,true);
    }
    else if (text->currentSelection->data.token == Exp_T
    && text->currentSynObj->parentObject->primaryToken == elsif_T) {
      ifx = (IfExpression*)text->currentSynObj->parentObject->parentObject;
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,ifx->targetDecl,ifx->targetCtx,ifx->targetCat,true);
    }
    else if (text->currentSelection->data.token == Exp_T
    && text->currentSynObj->parentObject->primaryToken == ifx_T) {
      ifx = (IfExpression*)text->currentSynObj->parentObject;
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,ifx->targetDecl,ifx->targetCtx,ifx->targetCat,true);
    }
    else if (text->currentSelection->data.token == Exp_T
    && text->currentSynObj->parentObject->primaryToken == select_T
    && text->currentSynObj->whereInParent
        == (address)&((SelectExpression*)text->currentSynObj->parentObject)->resultSet.ptr)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objSetCombo);
    else if (text->currentSelection->data.token == Exp_T
    && text->currentSynObj->parentObject->primaryToken == select_T
    && text->currentSynObj->whereInParent
        == (address)&((SelectExpression*)text->currentSynObj->parentObject)->addObject.ptr)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objCombo);
    else if (text->currentSelection->data.token == Exp_T
    && text->currentSynObj->parentObject->primaryToken == Handle_T) {
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,0,nullCtx,valueObjectCat,true);
    }
    else if (text->currentSynObj->parentObject->primaryToken == assign_T) {
      assigStm = (Assignment*)text->currentSynObj->parentObject;
      ((SynObject*)assigStm->targetObj.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      if (decl) {
        if (ctxFlags.bits)
          text->ckd.tempCtx.ContextFlags = ctxFlags;
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,decl,text->ckd.tempCtx,cat,true);
      }
      else
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objEnumCombo);
    }
    else if (text->currentSynObj->parentObject->primaryToken == copy_T) {
      copyStm = (CopyStatement*)text->currentSynObj->parentObject;
      ((SynObject*)copyStm->ontoObj.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      if (decl) {
        if (ctxFlags.bits)
          text->ckd.tempCtx.ContextFlags = ctxFlags;
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,decl,text->ckd.tempCtx,cat,true,true);
      }
      else
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objEnumCombo);
    }
    else if (text->currentSynObj->parentObject->IsMultOp()
    && text->currentSynObj->parentObject->IsExpression()) {
      multOpExp = (MultipleOp*)text->currentSynObj->parentObject;
      ((SynObject*)((CHE*)multOpExp->operands.first)->data)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      text->ckd.tempCtx = text->ckd.lpc;
      decl = text->ckd.document->GetTypeAndContext(decl,text->ckd.tempCtx);
      if (decl
      && myDoc->GetOperatorID(decl,(TOperator)(multOpExp->primaryToken-not_T),tidOperatorFunc)) {
        chpFormIn = GetFirstInput(&myDoc->IDTable,tidOperatorFunc);
        tid = ((LavaDECL*)chpFormIn->data)->RefID;
        ADJUST(tid,decl);
        if (ctxFlags.bits)
          text->ckd.tempCtx.ContextFlags = ctxFlags;
        decl = myDoc->GetFinalMVType(tid,0,text->ckd.tempCtx,cat,0);
        if (decl) {
          if (((LavaDECL*)chpFormIn->data)->TypeFlags.Contains(substitutable))
            text->ckd.tempCtx.ContextFlags = SET(multiContext,-1);
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompObjects(text->ckd,decl,text->ckd.tempCtx,cat,true);
        }
        else
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objEnumCombo);
      }
      else
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objEnumCombo);
    }
    else if (text->currentSynObj->parentObject->primaryToken == in_T
    && text->currentSynObj->whereInParent == (address)&((InSetStatement*)text->currentSynObj->parentObject)->operand2.ptr)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objSetCombo);
    else
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objEnumCombo);
    if (text->currentSelection->data.token == true_T
    || text->currentSelection->data.token == false_T
    || text->currentSelection->data.token == elseX_T
    || text->currentSelection->data.token == ifx_T) {
      redCtl->update();
      return;
    }
    break;

  case SetPH_T:
    ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objSetEnumCombo);
    redCtl->update();
    return;

  case TDOD_T:
    if (doubleClick && EnableGotoDecl()) {
      doubleClick = false;
      OnGotoDecl();
      redCtl->update();
      return;
    }
    objRef = (ObjReference*)text->currentSynObj->parentObject;
    if (objRef->flags.Contains(isDisabled)
    || objRef->flags.Contains(inExecHdr)
    || objRef->parentObject->primaryToken == Handle_T)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
    else {
      pTID = (TDOD*)text->currentSynObj;
      dw = myDoc->IDTable.GetVar(pTID->ID,idtype,text->ckd.inINCL);
      switch (idtype) {
      case noID:
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
        break;
      case localID:
        if (objRef->flags.Contains(isSelfVar)
        && objRef->refIDs.first == objRef->refIDs.last)
          decl = text->ckd.document->IDTable.GetDECL(selfVar->typeID,text->ckd.inINCL);
        else {
          ((VarName*)dw)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
          decl = text->ckd.document->GetType(decl);
        }
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowSubObjects(decl, text->ckd.lpc);
        break;
      default:
        if (pTID->fieldDecl && objRef->parentObject->primaryToken != FormParm_T)
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowSubObjects(pTID->fieldDecl, pTID->context);
        else
          ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      }
    }
    doubleClick = false;
    redCtl->update();
    return;

  case TypePH_T:
  case TypeRef_T:
    if (doubleClick && EnableGotoDecl()) {
      doubleClick = false;
      OnGotoDecl();
      redCtl->update();
      return;
    }
    if (text->currentSelection->data.token == TypeRef_T
    && Ignorable())
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
    else if (text->currentSynObj->replacedType == SetPH_T)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objSetEnumCombo);
    else if (text->currentSynObj->parentObject->primaryToken == item_T)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(typeCombo);
    else if (text->currentSynObj->parentObject->primaryToken == uuid_T
    || text->currentSynObj->parentObject->primaryToken == attach_T
    || text->currentSynObj->parentObject->primaryToken == itf_T
    || (text->currentSynObj->parentObject->primaryToken == new_T
        && ((NewExpression*)text->currentSynObj->parentObject)->itf.ptr))
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(coiCombo);
    else if(text->currentSynObj->type == TypePH_T
    && text->currentSynObj->parentObject->primaryToken == caseType_T) {
      typeSwitchExpression = (SynObject*)((TypeSwitchStatement*)text->currentSynObj->parentObject->parentObject)->caseExpression.ptr;
      typeSwitchExpression->ExprGetFVType(text->ckd,declSwitchExpression,catSwitchExpression,ctxFlags);
      if (declSwitchExpression) {
        CContext swCtx = text->ckd.tempCtx;
        if (ctxFlags.bits)
          swCtx.ContextFlags = ctxFlags;
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCompaTypes(text->ckd,declSwitchExpression,swCtx);
      }
      else
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(typeCombo);
    }
    else
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(typeCombo);
    doubleClick = false;
    redCtl->update();
    return;

  case CrtblPH_T:
  case Callee_T:
  case CompObj_T:
  case CrtblRef_T:
    if (doubleClick && EnableGotoDecl()) {
      doubleClick = false;
      OnGotoDecl();
      redCtl->update();
      return;
    }
    if (text->currentSynObj->parentObject->primaryToken == attach_T)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(attachCombo);
    else if (text->currentSynObj->parentObject->primaryToken == new_T)
      if (text->currentSynObj->parentObject->parentObject->primaryToken == select_T
      && text->currentSynObj->parentObject->whereInParent
          == (address)&((SelectExpression*)text->currentSynObj->parentObject->parentObject)->resultSet.ptr)
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(newCombo);
      else
        ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(newAndCObjCombo);
    else // "call" statement
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(callCombo);
    doubleClick = false;
    redCtl->update();
    return;

  case VarPH_T:
    str = TOKENSTR[VarPH_T];
    editToken = VarName_T;
    ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
    break;

  case VarName_T:
    str = ((VarName*)text->currentSynObj)->varName.c;
    editToken = VarName_T;
    ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
    break;

  case Const_T:
    str = ((Constant*)text->currentSynObj)->str.c;
    editToken = Const_T;
    if (doubleClick)
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
    else
      goto exp;
    break;

  case enumConst_T:
    ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(objEnumCombo);
    if (doubleClick && EnableGotoDecl()) {
      OnGotoDecl();
      doubleClick = false;
      break;
    }
    else {
      doubleClick = false;
      goto exp;
    }

  case parameter_T:
    ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
    //redCtl->update();
    return;

  default:
    if (text->currentSynObj->replacedType == Exp_T)
      goto exp;
    ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
    redCtl->update();
    return;
  }

  if (!doubleClick) {
    redCtl->update();
    return;
  }

  doubleClick = false;

  if (text->currentSelection->data.token == Exp_T
  || text->currentSelection->data.token == ExpOpt_T
  || text->currentSelection->data.token == Const_T) {
    //wxTheApp->m_appWindow->Workspace()->setUpdatesEnabled(true);
    //redCtl->repaint();
      // otherwise the MiniEdit's position would be unknown in the following code
    OnConst();
    //wxTheApp->m_appWindow->Workspace()->setUpdatesEnabled(false);
  }
  else if (text->currentSynObj->type == VarPH_T) {
    if (!editCtl)
      editCtl = new MiniEdit(redCtl);
    editCtl->setGeometry(text->currentSelection->data.rect);
    editCtl->setText(str);
    editCtl->setFixedWidth(text->currentSelection->data.rect.width()+2*editCtl->frameWidth()+10);
    editCtl->setFixedHeight(text->currentSelection->data.rect.height()+4/**editCtl->frameWidth()*/);
    int r=text->currentSelection->data.rect.right();
    redCtl->contentsWidth = qMax(redCtl->contentsWidth,r);
    editCtl->setCursorPosition(str.length());
    //QApplication::postEvent(this,new CustomEvent(UEV_ShowMiniEdit,0));
    editCtl->show();
    //qDebug() << "show MiniEdit";
    editCtl->setFocus();
    if (text->currentSynObj->IsPlaceHolder())
      editCtl->selectAll();
    //editCtl->update();
    editCtlVisible = true;
    redCtl->update();
    //wxTheApp->m_appWindow->Workspace()->setUpdatesEnabled(false);
  }
}


/////////////////////////////////////////////////////////////////////////////
// CExecView message handlers

void CExecView::SetSelectAt (SynObject *obj) {
  forcePrimTokenSelect = true;
  text->currentSynObj = obj;
  text->selectAt = obj;
  text->currentSelection = obj->primaryTokenNode;
}

void CExecView::SetSelectAtHint (CLavaPEHint *hint) {
  CHAINX *chx;
  CHE *che;
  SynObject *insObj;
  MultipleOp *multOp;

  text->selectAfter = text->selectAt;
  switch ((InsertMode)(unsigned)hint->CommandData3) {
  case InsMult:
    if (text->selectAt) break;
    multOp = (MultipleOp*)hint->CommandData4;
    insObj = (SynObject*)((CHE*)multOp->operands.first->successor)->data;
    text->selectAt = multOp; //insObj;
    text->selectAfter = multOp; //insObj;
    break;
  case InsChain:
    if (text->selectAt) break;
    che = (CHE*)hint->CommandData4;
    insObj = (SynObject*)che->data;
    text->selectAt = insObj;
    text->selectAfter = insObj;
    break;
  case DelChain:
    if (text->selectAt) break;
    chx = (CHAINX*)hint->CommandData5;
    if (chx->first) {
      che = (CHE*)hint->CommandData4;
      if (che->successor)
        text->selectAt = (SynObject*)((CHE*)che->successor)->data;
      else if (che->predecessor)
        text->selectAt = (SynObject*)((CHE*)che->predecessor)->data;
      else
        text->selectAt = (SynObject*)hint->CommandData2;
    }
    else
      text->selectAt = (SynObject*)hint->CommandData2;
    text->selectAfter = text->selectAt;
    break;
  default:
    if (text->selectAt) break;
    text->selectAt = toBeDrawn;
    text->selectAfter = toBeDrawn;
  }

  text->plhSearch = false;
}


void CExecView::Check () {
//  text->ckd.currentSynObj = 0;
  text->ckd.nErrors = 0;
  text->ckd.nPlaceholders = 0;
  ((SynObject*)myDECL->Exec.ptr)->Check(text->ckd);
}


void CExecView::RedrawExec(SynObject *selectAt)
{
  // TODO: Add your command handler code here
  CSearchData sData;

  text->tokenChain.Destroy();
  text->INIT();
  replacedObj = 0;
  //redCtl->setUpdatesEnabled(false);
  if (myDECL->TreeFlags.Contains(leftArrows))
    text->leftArrows = true;
  else
    text->leftArrows = false;
  if (myDECL->TreeFlags.Contains(parmNames))
    text->parmNames = true;
  else
    text->parmNames = false;
  text->showComments = myDECL->TreeFlags.Contains(ShowExecComments);

  text->selectAt = selectAt?selectAt:selfVar;
  text->selectAfter = text->selectAt;

  Redraw(selfVar);

  if (externalHint && text->currentSynObjID) {
    sData.synObjectID = text->currentSynObjID;
    sData.execView = this;
    selfVar->MakeTable((address)&myDoc->IDTable, 0, (SynObjectBase*)myDECL, onSelect, 0,0, (address)&sData);
    redCtl->update();
  }
  Select();
}


void CExecView::Redraw (SynObject *newObj) {
  CHETokenNode *delToken, *delTokenSucc, *oldStartToken, *oldEndToken,
    *newStartToken, *newEndToken;
  SynObject *parent;
  FuncExpression *funcExpr;
  bool mod=true, ignore=false;
  QString selText;
//  int para, startOfLine, nArrows=0, len;

  if (newObj && newObj->primaryToken == FormParms_T && selfVar->oldParmsStartToken) {
    oldStartToken = selfVar->oldParmsStartToken;
    oldEndToken = selfVar->oldParmsEndToken;
  }
  else if (replacedObj) {
    oldStartToken = replacedObj->startToken;
    oldEndToken = replacedObj->endToken;
    replacedObj = 0;
  }
  else {
    oldStartToken = 0;
    oldEndToken = 0;
  }
  if (newObj) {
    text->currentSynObj = newObj->parentObject;
    for (parent = newObj->parentObject;
         parent;
         parent = parent->parentObject) {
      if (parent->flags.Contains(ignoreSynObj)) {
        ignore = true;
        break;
      }
    }
  }

  if (oldStartToken) {
    text->currentToken = (CHETokenNode*)oldStartToken->predecessor;
    text->firstMoved = (CHETokenNode*)oldEndToken->successor;

    delToken = oldStartToken;
    do {
      delTokenSucc = (CHETokenNode*)delToken->successor;
      if (delToken == text->currentSelection)
        text->currentSelection = 0;
      text->tokenChain.Remove(text->currentToken);
      if (delToken == oldEndToken) break;
      else delToken = delTokenSucc;
    } while (true);

    if (newObj) {
      newObj->Draw(*text,newObj->whereInParent,newObj->containingChain,ignore);
      newStartToken = newObj->startToken;
      newEndToken = newObj->endToken;
      parent = newObj->parentObject;
      while (parent && mod) {
        mod = false;
        if (parent->startToken == oldStartToken) {
          mod = true;
          parent->startToken = newStartToken;
        }
        if (parent->endToken == oldEndToken) {
          mod = true;
          parent->endToken = newEndToken;
        }
        parent = parent->parentObject;
      }
    }

//    text->CorrectPositions();
  }
  else {
    newObj->Draw(*text,newObj->whereInParent,0,ignore);
    text->insertedChars = 0;
  }

  if (newObj && newObj->IsFuncInvocation()) {
    funcExpr = (FuncExpression*)newObj;
    if (funcExpr->handle.ptr) {
      if (((SynObject*)funcExpr->handle.ptr)->IsPlaceHolder())
        text->newSelection = ((SynObject*)funcExpr->handle.ptr)->primaryTokenNode;
    }
    else if (((SynObject*)funcExpr->function.ptr)->IsPlaceHolder())
      text->newSelection = ((SynObject*)funcExpr->function.ptr)->primaryTokenNode;
  }
}

bool CExecView::IsTopLevelToken ()
{
  return (text->currentSelection->data.flags.Contains(primToken)
    || text->currentSelection->data.token == Lparenth_T
    || text->currentSelection->data.token == Rparenth_T
    || text->currentSelection->data.token == Larrow_T
    || text->currentSelection->data.token == Rarrow_T
    || text->currentSynObj->primaryToken == arrayAtIndex_T
    || selStartPos == text->currentSynObj->startToken);
}

bool CExecView::IsDeletablePrimary () {
  if ((IsTopLevelToken()
       /*&& text->currentSelection->data.synObject->primaryToken != assignFS_T*/)
  || text->currentSelection->data.token == TDOD_T
  || text->currentSelection->data.token == Tilde_T)
    return true;
  return false;
}

static bool IsRelToken (TToken token)
{
  switch (token) {
  case Equal_T:
  case NotEqual_T:
  case LessThan_T:
  case GreaterThan_T:
  case LessEqual_T:
  case GreaterEqual_T:
    return true;
  default:
    return false;
  }
}


void CExecView::PutInsFlagHint(SET insFlags, SET firstLastHint) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    firstLastHint,
    myDECL,
    text->currentSynObj,
    (void*)InsFlags,
    (void*)insFlags.bits);

  myDoc->UndoMem.AddToMem(nextHint);
  if (firstLastHint.Contains(lastHint))
    myDoc->UpdateDoc(this, false);
}


void CExecView::PutPlusMinusHint() {
  if (text->currentSynObj->primaryToken == TDOD_T)
    text->currentSynObj = text->currentSynObj->parentObject;
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    SET(firstHint,lastHint,-1),
    myDECL,
    text->currentSynObj->parentObject,
    (void*)PlusMinus,
    text->currentSynObj,
    0);

  myDoc->UndoMem.AddToMem(nextHint);
  myDoc->UpdateDoc(this, false);
}

/*
void CExecView::PutArrowHint() {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    SET(firstHint,lastHint,-1),
    myDECL,
    text->currentSynObj,
    (DWORD)ToggleArrows);

  myDoc->UndoMem.AddToMem(nextHint);
  myDoc->UpdateDoc(this, false);
}
*/

void CExecView::PutDelFlagHint(SET delFlags, SET firstLastHint) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    firstLastHint,
    myDECL,
    text->currentSynObj,
    (void*)DelFlags,
    (void*)delFlags.bits);

  myDoc->UndoMem.AddToMem(nextHint);
  if (firstLastHint.Contains(lastHint))
    myDoc->UpdateDoc(this, false);
}

void CExecView::PutInsHint(SynObject *insObj, SET firstLastHint) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    firstLastHint,
    myDECL,
    text->currentSynObj->parentObject,
    (void*)InsNested,
    insObj,
    text->currentSynObj->containingChain,
    text->currentSynObj->whereInParent);

  myDoc->UndoMem.AddToMem(nextHint);
  //if (firstLastHint.Contains(lastHint))
    myDoc->UpdateDoc(this,false,nextHint);
}

void CExecView::PutInsChainHint(CHE *newChe,CHAINX *chain,CHE *pred,SET firstLastHint) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    firstLastHint,
    myDECL,
    text->currentSynObj,
    (void*)InsChain,
    newChe,
    chain,
    pred);

  myDoc->UndoMem.AddToMem(nextHint);
  //if (firstLastHint.Contains(lastHint))
    myDoc->UpdateDoc(this,false,nextHint);
}


void CExecView::PutInsMultOpHint(SynObject *multOp, SET firstLastHint) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    firstLastHint,
    myDECL,
    text->currentSynObj->parentObject,
    (void*)InsMult,
    multOp,
    text->currentSynObj->containingChain,
    text->currentSynObj->whereInParent);

  myDoc->UndoMem.AddToMem(nextHint);
  myDoc->UpdateDoc(this,false,nextHint);
  nextHint = 0;
}

void CExecView::PutIniCall(SynObject *currVarItem, bool after, bool onlyIniCall, bool replace) {
  FuncStatement *funcStm, *precIniCall=0, *succIniCall;
  MultipleOp *multOp;
  VarName *varName;
  ObjReference *varRef;
  TDOD *tdod;
  TDODC refIDs;
  Declare *dcl=(Declare*)currVarItem->parentObject->parentObject;
  SynObject *currentSynObj=text->currentSynObj;
  CHE *che, *newChe, *precV, *precQ;
  CHAINX *chx;
  bool isFirst=false;

  // exists a preceding ini call?
  if (((CHE*)currVarItem->whereInParent)->predecessor) {
    precV = (CHE*)((CHE*)currVarItem->whereInParent)->predecessor;
    precIniCall = ((SynObject*)precV->data)->iniCall;
  }
  else if (((CHE*)currVarItem->parentObject->whereInParent)->predecessor) {
    precQ = (CHE*)((CHE*)currVarItem->parentObject->whereInParent)->predecessor;
    precV = (CHE*)((Quantifier*)precQ->data)->quantVars.last;
    precIniCall = ((SynObject*)precV->data)->iniCall;
  }
  else if (!((CHE*)currVarItem->whereInParent)->successor
  && !((CHE*)currVarItem->parentObject->whereInParent)->successor) {
    isFirst = true;
    chx = 0;
  }

  // build the ini call
  if (currVarItem->IsPlaceHolder()) {
    funcStm = new FuncStatementV(true);
    funcStm->flags.INCL(isIniCallOrHandle);
    funcStm->flags.INCL(staticCall);
    ((SynObject*)funcStm->handle.ptr)->primaryToken = ExpDisabled_T;
    ((SynObject*)funcStm->handle.ptr)->type = ExpDisabled_T;
    ((SynObject*)funcStm->handle.ptr)->replacedType = ExpDisabled_T;
    ((SynObject*)funcStm->handle.ptr)->flags.INCL(isDisabled);
    funcStm->varName = currVarItem;
  }
  else {
    varName = (VarName*)currVarItem;
    tdod = new TDOD();
    tdod->ID.nID = varName->varID.nID;
    tdod->ID.nINCL = varName->varID.nINCL;
    refIDs.Append(new CHE(tdod));
    varRef = new ObjReferenceV(refIDs,varName->varName.c);
    funcStm = new FuncStatementV(varRef);
    varRef->parentObject = funcStm;
    funcStm->flags.INCL(isIniCallOrHandle);
    funcStm->flags.INCL(staticCall);
    funcStm->varName = varName;
  }

  if (isFirst) {
    text->currentSynObj = funcStm;
    funcStm->parentObject = dcl;
    funcStm->containingChain = 0;
    funcStm->whereInParent = &dcl->secondaryClause.ptr;
    PutInsHint(funcStm);
  }
  else { // insert new or replace existing ini call
    if (precIniCall) {
      chx=precIniCall->containingChain;
      che=(CHE*)precIniCall->whereInParent;
    }
    else if (((CHE*)currVarItem->whereInParent)->successor) {
      che = (CHE*)((CHE*)currVarItem->whereInParent)->successor;
      succIniCall = ((VarName*)che->data)->iniCall;
      chx=succIniCall->containingChain;
      che = 0;
    }
    else {
      che = (CHE*)((CHE*)currVarItem->parentObject->whereInParent)->successor;
      succIniCall = ((VarName*)((CHE*)((Quantifier*)che->data)->quantVars.first)->data)->iniCall;
      chx=succIniCall->containingChain;
      che = 0;
    }

    newChe = NewCHE(funcStm);
    funcStm->whereInParent = (address)newChe;
    if (after)
      if (chx) {
        if (replace) {
          text->currentSynObj = (SynObject*)((CHE*)((CHE*)precIniCall->whereInParent)->successor)->data;
          PutInsHint(funcStm,SET(lastHint,-1));
        }
        else
          PutInsChainHint(newChe,chx,che,onlyIniCall?SET(firstHint,lastHint,-1):SET(lastHint,-1));
      }
      else {
        multOp = new SemicolonOpV;
        text->currentSynObj = precIniCall;
        PutInsMultOpHint(multOp,SET());
        chx = &multOp->operands;
        text->currentSynObj = multOp;
        PutInsChainHint(newChe,chx,(CHE*)multOp->operands.first,SET(lastHint,-1));
     }
    else {
      if (chx)
        PutInsChainHint(newChe,chx,che,onlyIniCall?SET(firstHint,lastHint,-1):SET(lastHint,-1));
      else {
        multOp = new SemicolonOpV;
        text->currentSynObj = currVarItem->iniCall;
        PutInsMultOpHint(multOp,SET());
        chx = &multOp->operands;
        text->currentSynObj = multOp;
        PutInsChainHint(newChe,&multOp->operands,(CHE*)multOp->operands.first,onlyIniCall?SET(firstHint,lastHint,-1):SET(lastHint,-1));
      }
      funcStm->containingChain = chx;
    }
  }
}

void CExecView::PutChgCommentHint(TComment *pCmt) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    SET(firstHint,lastHint,-1),
    myDECL,
    text->currentSynObj,
    (void*)ChgComment,
    pCmt);

  myDoc->UndoMem.AddToMem(nextHint);
  myDoc->UpdateDoc(this, false);
}

void CExecView::PutChgOpHint(TToken token) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    SET(firstHint,lastHint,-1),
    myDECL,
    text->currentSynObj,
    (void*)ChgOp,
    (void*)token);

  myDoc->UndoMem.AddToMem(nextHint);
  myDoc->UpdateDoc(this, false);
}

void CExecView::PutDelHint(SynObject *delObj, SET firstLastHint) {
  CHAINX *chx=delObj->containingChain;
  CHE *che;
  SynObject *placeHdr=0;

  if (chx
  && ((!delObj->IsExpression()
       && (chx->first != chx->last || delObj->primaryToken == catch_T))
      || delObj->IsPlaceHolder())) {
    che = (CHE*)delObj->whereInParent;

    if (delObj->primaryToken == quant_T
    && delObj->parentObject->primaryToken == declare_T
    && ((Declare*)delObj->parentObject)->secondaryClause.ptr) {
      nextHint = new CLavaPEHint(
        CPECommand_Exec,
        myDoc,
        firstLastHint-SET(lastHint,-1),
        myDECL,
        delObj->parentObject, // parent,
        (void*)DelChain,
        che,
        chx,
        che->predecessor);
      myDoc->UndoMem.AddToMem(nextHint);
      Quantifier *quant = (Quantifier*)delObj;
      for (CHE *cheVn=(CHE*)quant->quantVars.first; cheVn; cheVn=(CHE*)cheVn->successor) {
        SynObject *iniCall = ((SynObject*)cheVn->data)->iniCall;
        che = (CHE*)iniCall->whereInParent;
        chx = iniCall->containingChain;
        nextHint = new CLavaPEHint(
          CPECommand_Exec,
          myDoc,
          cheVn->successor?SET():SET(lastHint,-1),
          myDECL,
          iniCall->parentObject, // parent,
          (void*)DelChain,
          che,
          chx,
          che->predecessor);
        myDoc->UndoMem.AddToMem(nextHint);
      }
      myDoc->UpdateDoc(this, false);
      return;
    }
    else {
      bool delMult
        = (delObj->parentObject->IsMultOp()
        && chx->first->successor == chx->last) ? true : false;
      nextHint = new CLavaPEHint(
        CPECommand_Exec,
        myDoc,
        delMult ? firstLastHint-SET(lastHint,-1) : firstLastHint,
        myDECL,
        delObj->parentObject, // parent,
        (void*)DelChain,
        che,
        chx,
        che->predecessor);
      myDoc->UndoMem.AddToMem(nextHint);
      if (delMult) {
        nextHint = new CLavaPEHint(
          CPECommand_Exec,
          myDoc,
          SET(lastHint,-1),
          myDECL,
          text->currentSynObj->parentObject->parentObject,
          (void*)DelMult,
          delObj->parentObject,
          delObj->parentObject->containingChain,
          delObj->parentObject->whereInParent);
        myDoc->UndoMem.AddToMem(nextHint);
      }
    }
  }
  else {
    if (delObj->primaryToken != TypePH_T)
        // optional type in set quantifier
      placeHdr = new SynObjectV(delObj->replacedType);
    else
      text->currentSynObjID = 0;

    nextHint = new CLavaPEHint(
      CPECommand_Exec,
      myDoc,
      3UL,
      myDECL,
      text->currentSynObj->parentObject,
      (DWORD)InsNested,
      placeHdr,
      text->currentSynObj->containingChain,
      text->currentSynObj->whereInParent);
    myDoc->UndoMem.AddToMem(nextHint);
  }

  if (firstLastHint.Contains(lastHint))
    myDoc->UpdateDoc(this, false);
}

void CExecView::PutDelNestedHint(SET firstLastHint) {
  nextHint = new CLavaPEHint(
    CPECommand_Exec,
    myDoc,
    firstLastHint,
    myDECL,
    text->currentSynObj->parentObject,
    (void*)DelNested,
    text->currentSynObj,
    text->currentSynObj->containingChain,
    text->currentSynObj->whereInParent);

  myDoc->UndoMem.AddToMem(nextHint);
  if (firstLastHint.Contains(lastHint))
    myDoc->UpdateDoc(this, false);
}


void CExecView::InsertOrReplace (SynObject *insObj) {
  switch (text->currentSynObj->primaryToken) {
  case FuncRef_T:
  case FuncPH_T:
    if (insObj->primaryToken != nil_T)
      text->currentSynObj = text->currentSynObj->parentObject;
    break;
  case TDOD_T:
    text->currentSynObj = text->currentSynObj->parentObject;
    break;
  default: ;
  }
  if (!text->currentSynObj->IsPlaceHolder()
  && text->currentSynObj->IsStatement())
    InsMultChain(insObj);
  else
    PutInsHint(insObj);
}

void CExecView::InsMultChain (SynObject *insObj) {
  MultipleOp *multOp;
  CHE *che, *newChe = NewCHE(insObj);

  if (text->currentSynObj->parentObject->IsMultOp()) {
    multOp = (MultipleOp*)text->currentSynObj->parentObject;
    che = (CHE*)text->currentSynObj->whereInParent;
    text->currentSynObj = multOp;
    if (insertBefore)
      PutInsChainHint(newChe,&multOp->operands,(CHE*)che->predecessor);
    else
      PutInsChainHint(newChe,&multOp->operands,che);
  }
  else {
    if (insObj->type == Stm_T)
      multOp = new SemicolonOpV();
    else
      multOp = new PlusOpV();
    PutInsMultOpHint(multOp);
    text->currentSynObj = multOp;
    if (insertBefore)
      PutInsChainHint(newChe,&multOp->operands,(CHE*)0,SET(lastHint,-1));
    else
      PutInsChainHint(newChe,&multOp->operands,(CHE*)multOp->operands.first,SET(lastHint,-1));
  }
}

void CExecView::InsMultOp (TToken primaryToken, MultipleOp *multOp) {
  SynObject *insObj = new SynObjectV(OperandType(primaryToken));
  CHE *newChe = NewCHE(insObj);

  // if we are on a mult op or an element of a mult op of this same type
  // then we insert a new element before or after the present location
  // else we insert a new mult op in place of the current element
  // whose only elements are the current element and a placeholder for a
  // new element before or after the current element:

  switch (text->currentSynObj->primaryToken) {
  case FuncRef_T:
  case FuncPH_T:
  case FuncDisabled_T:
  case TDOD_T:
    text->currentSynObj = text->currentSynObj->parentObject;
    break;
  default: ;
  }
  PutInsMultOpHint(multOp);
  text->currentSynObj = multOp;
  if (insertBefore)
    PutInsChainHint(newChe,&multOp->operands,(CHE*)0,SET(lastHint,-1));
  else
    PutInsChainHint(newChe,&multOp->operands,(CHE*)multOp->operands.first,SET(lastHint,-1));
}


bool CExecView::Ignorable () {
  if ((inIgnore || inExecHeader || inFormParms || inBaseInits
  || !text->currentSynObj->parentObject
  || text->currentSynObj->flags.Contains(isDisabled))
  && !inParameter)
    return true;
  else
    return false;
}

bool CExecView::Taboo (bool isEnableBreakpoints) {
  if (GetDocument()->changeNothing && !isEnableBreakpoints)
    return true;
  else if ((inExecHeader || inFormParms || inBaseInits || !text->currentSynObj->parentObject)
  && !inParameter)
    return true;
  else if (text->currentSynObj->parentObject
  && (text->currentSynObj->parentObject->primaryToken == Handle_T
      || (text->currentSynObj->parentObject->parentObject
      && text->currentSynObj->parentObject->primaryToken == ObjRef_T
      && text->currentSynObj->parentObject->parentObject->primaryToken == Handle_T)))
    return true;
  else
    return false;
}

//////////////////////////////////////////////////////////////////////

void CExecView::OnAnd()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;

  if (text->currentSynObj->ReadOnlyContext() != roClause)
    InsMultOp(Semicolon_T,new SemicolonOpV());
  else
    InsMultOp(and_T,new AndOpV());
}

void CExecView::OnBitAnd()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(BitAnd_T,new BitAndOpV());
}

void CExecView::OnBitOr()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(BitOr_T,new BitOrOpV());
}

void CExecView::OnBitXor()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(BitXor_T,new BitXorOpV());
}

void CExecView::OnClone()
{
  CloneExpressionV *mcln;
  VarName *varNamePtr;
//  char buffer[10];
  // TODO: Add your command handler code here

  if (!EditOK()) return;

  mcln = new CloneExpressionV(true);
  varNamePtr = (VarName*)mcln->varName.ptr;
  if (tempNo > 0) {
//              qsprintf(buffer,"%u",++tempNo);
    varNamePtr->varName += qPrintable(QString::number(++tempNo));
//    varNamePtr->varName += _ultoa(++tempNo,buffer,10);
        }
  InsertOrReplace(mcln);
}

void CExecView::OnSelect()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  SelectExpression *collect = new SelectExpressionV(true);
  InsertOrReplace(collect);
}

void CExecView::OnComment()
{
  // TODO: Add your command handler code here
  SynObject *synObj = text->currentSynObj;

  if (!EditOK()) return;
  clicked = false;
//!!!  setUpdatesEnabled(true);
  CComment *pComment = new CComment(this);

  if (synObj->comment.ptr) {
    pComment->inline_comment->setChecked(synObj->comment.ptr->inLine);
    pComment->trailing_comment->setChecked(synObj->comment.ptr->trailing);
    pComment->text->setPlainText(synObj->comment.ptr->str.c);
  }

  pComment->exec();
  if (pComment->result() == QDialog::Accepted) {
    TComment *pCmt = new TCommentV;
    pCmt->inLine = pComment->inline_comment->isChecked();
    pCmt->trailing = pComment->trailing_comment->isChecked();
    pCmt->str = STRING(qPrintable(pComment->text->document()->toPlainText()));
    text->currentSynObj = synObj;
    text->showComments = true;
    myDECL->TreeFlags.INCL(ShowExecComments);
//              GetDocument()->Modify(true);
    PutChgCommentHint(pCmt);
  }
  delete pComment;
}

void CExecView::OnConst()
{
  // TODO: Add your command handler code here
  if (!EditOK()) return;

  if (!editCtl)
    editCtl = new MiniEdit(redCtl);
  QFontMetrics fom(editCtl->font());
  editCtl->setGeometry(text->currentSelection->data.rect);
  if (text->currentSelection->data.token == Const_T) {
    editCtl->setText(((Constant*)text->currentSynObj)->str.c);
    editCtl->setFixedWidth(fom.width(QString(((Constant*)text->currentSynObj)->str.c)+2*editCtl->frameWidth()));
  }
  else {
    editCtl->setText(TOKENSTR[text->currentSelection->data.token]);
    editCtl->setFixedWidth(fom.width(QString(TOKENSTR[Const_T])+" ")+2*editCtl->frameWidth());
  }
  editCtl->setFixedHeight(text->currentSelection->data.rect.height()+editCtl->frameWidth());
  int r=text->currentSelection->data.rect.right();
  redCtl->contentsWidth = qMax(redCtl->contentsWidth,r);
  editCtl->show();
  editCtl->setFocus();
  editCtl->selectAll();
  editCtlVisible = true;
  editToken = Const_T;
}

void CExecView::OnDelete ()
{
  // TODO: Add your command handler code here
  bool reject=false;
  CHAINX *chx;
  CHE *chp;
  SynObject *oldCurrentSynObj = text->currentSynObj, *synObj, *optClause=0;
  ObjReference *oldRef, *newRef;
  Run *runStm;
  NewExpression *newExp;
  CloneExpression *cloneExp;
  AttachObject *attachStm;

  if (!EditOK()) return;

  if (GetDocument()->changeNothing)
    return;

  text->currentSynObjID = 0;

  if (text->currentSelection->data.token == Comment_T) {
    TComment *pCmt = new TCommentV;
    pCmt->inLine = false;
    pCmt->trailing = false;
    pCmt->str = STRING("");
    PutChgCommentHint(pCmt);
    return;
  }

  chx = text->currentSynObj->containingChain;

  if (text->currentSynObj->IsPlaceHolder()
  && !chx
  && !(text->currentSynObj->parentObject->primaryToken == quant_T
       && text->currentSelection->data.token == TypePHopt_T
       && ((Quantifier*)text->currentSynObj->parentObject)->set.ptr)
  && !text->currentSynObj->parentObject->NestedOptClause(text->currentSynObj)
  && text->currentSynObj->parentObject->primaryToken != fail_T
  && text->currentSelection->data.token != ObjPHOpt_T) {
    text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->primaryToken == elsif_T) {
      chx = text->currentSynObj->containingChain;
      if (text->currentSynObj->whereInParent == (address)chx->first)
        text->currentSynObj = text->currentSynObj->parentObject;
    }
    else if (text->currentSynObj->primaryToken == parameter_T)
      text->currentSynObj = text->currentSynObj->parentObject;
  }
  else if (text->currentSynObj->parentObject->NestedOptClause(text->currentSynObj)
  && text->currentSynObj->parentObject->primaryToken == ifdef_T
  && IsPH(text->currentSynObj)) {
    optClause = text->currentSynObj;
    text->currentSynObj = text->currentSynObj->parentObject;
  }

  else if (text->currentSynObj->primaryToken == TDOD_T) {
    if ((CHE*)text->currentSynObj->whereInParent == chx->first)
      text->currentSynObj = text->currentSynObj->parentObject;
  }

  if (on_editCutAction_triggered) {
    clipBoardObject = (SynObject*)text->currentSynObj->Clone();
    clipBoardDoc = myDoc;
  }

  chx = text->currentSynObj->containingChain;
  if (chx && chx->first == chx->last) {
    if (text->currentSynObj->IsPlaceHolder()) {
      if (text->currentSynObj->parentObject->primaryToken == case_T
        || text->currentSynObj->parentObject->primaryToken == quant_T
        || (text->currentSynObj->parentObject->primaryToken == ifdef_T
            && chx == &((IfdefStatement*)text->currentSynObj->parentObject)->ifCondition))
        reject = true;
    }
    else if (text->currentSynObj->primaryToken == case_T
    || text->currentSynObj->primaryToken == caseType_T
//    || text->currentSynObj->primaryToken == catch_T
    || text->currentSynObj->primaryToken == elsif_T
    || text->currentSynObj->primaryToken == quant_T)
      reject = true;
  }
  else if (text->currentSynObj->primaryToken == ifdef_T
  && ((text->currentSelection->data.token == then_T
      || text->currentSelection->data.token == else_T
      || (optClause && text->currentSynObj->NestedOptClause(optClause)))
      && (!((IfdefStatement*)text->currentSynObj)->thenPart.ptr
          || !((IfdefStatement*)text->currentSynObj)->elsePart.ptr)))
      reject = true;

  if (reject) {
    QMessageBox::critical(this,qApp->applicationName(),ERR_One_must_remain,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    text->currentSynObj = oldCurrentSynObj;
    return;
  }

  if (optClause/*ifdef-then/else case*/ || text->currentSelection->data.OptionalClauseToken(optClause)) {
    text->currentSynObj = optClause;
    PutDelNestedHint(SET(firstHint,lastHint,-1));
    return;
  }

  if (text->currentSynObj->primaryToken == TDOD_T) {
    CHE *che = (CHE*)text->currentSynObj->whereInParent;
    oldRef = (ObjReference*)text->currentSynObj->parentObject;
    newRef = new ObjReferenceV;
    newRef->primaryToken = oldRef->primaryToken;
    newRef->type = oldRef->type;
    newRef->replacedType = oldRef->replacedType;
    newRef->flags = oldRef->flags;
    CopyUntil(oldRef,(CHE*)che->predecessor,newRef);
    SetRefTypeFlags(newRef);

    text->currentSynObj = text->currentSynObj->parentObject;
    synObj = text->currentSynObj->parentObject;
//    if (synObj->IsFuncInvocation())
      PutInsHint(newRef);
  }
  else if (text->currentSynObj->primaryToken == ObjRef_T)
    if (text->currentSynObj->parentObject->primaryToken == signal_T
    && text->currentSynObj->flags.Contains(isSelfVar)) {
      text->currentSynObj = oldCurrentSynObj;
      return;
    }
    else if (text->currentSynObj->flags.Contains(isIniCallOrHandle))
      return;
    else
      PutDelHint(text->currentSynObj);
  else if (text->currentSynObj->primaryToken == FuncRef_T) {
    PutInsHint(new SynObjectV(FuncPH_T));
  }
  else if (text->currentSynObj->primaryToken == CrtblRef_T) {
    text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->primaryToken == run_T) {
      runStm = new RunV(true);
      PutInsHint(runStm);
    }
    else if (text->currentSynObj->primaryToken == attach_T) {
      attachStm = new AttachObjectV(true);
      PutInsHint(attachStm);
    }
    else { // NewExpression
      newExp = new NewExpressionV(true);
      PutInsHint(newExp);
    }
  }
  else if (text->currentSynObj->primaryToken == assignFS_T)
    if (text->currentSynObj->parentObject->primaryToken == run_T) {
      text->currentSynObj = text->currentSynObj->parentObject;
      runStm = new RunV(true);
      PutInsHint(runStm);
    }
    else if (text->currentSynObj->parentObject->primaryToken == new_T
      && text->currentSynObj->flags.Contains(isIniCallOrHandle)) {
      text->currentSynObj = text->currentSynObj->parentObject;
      newExp = new NewExpressionV(true);
      PutInsHint(newExp);
    }
    else {
      //if (text->currentSynObj->parentObject->primaryToken != declare_T
      //|| text->currentSynObj->whereInParent != (address)&((Declare*)text->currentSynObj->parentObject)->secondaryClause.ptr)
      if (!text->currentSynObj->flags.Contains(isIniCallOrHandle))
        PutDelHint(text->currentSynObj);
    }
  else if (text->currentSynObj->primaryToken == VarName_T
    && text->currentSynObj->parentObject->primaryToken == run_T) {
    return;
  }
  else if (text->currentSynObj->primaryToken == VarName_T
    && text->currentSynObj->parentObject->primaryToken == new_T) {
    return;
  }
  else if (text->currentSynObj->primaryToken == VarName_T
    && text->currentSynObj->parentObject->primaryToken == clone_T) {
    text->currentSynObj = text->currentSynObj->parentObject;
    cloneExp = new CloneExpressionV(true);
    PutInsHint(cloneExp);
  }
  else if (text->currentSynObj->primaryToken == nil_T
    && text->currentSynObj->replacedType == FuncPH_T) {
    PutInsHint(new SynObjectV(FuncDisabled_T));
  }
  else if (text->currentSynObj->parentObject->NestedOptClause (text->currentSynObj)
  && text->currentSynObj->IsPlaceHolder()) {
    PutDelNestedHint(SET(firstHint,lastHint,-1));
    return;
  }
  else if (text->currentSelection->data.token == ObjPHOpt_T) {
    text->selectAt = text->currentSynObj->parentObject->parentObject;
    PutInsFlagHint(SET(ignoreSynObj,-1));
  }
  else if (text->currentSynObj->type == VarPH_T
  && text->currentSynObj->parentObject->parentObject->IsDeclare()
  && ((Declare*)text->currentSynObj->parentObject->parentObject)->secondaryClause.ptr) {
    chp = (CHE*)text->currentSynObj->whereInParent;
    if (chp->predecessor)
      text->selectAt = (SynObject*)((CHE*)chp->predecessor)->data;
    else
      text->selectAt = (SynObject*)((CHE*)chp->successor)->data;
    PutDelHint(text->currentSynObj,SET(firstHint,-1));
    PutDelHint(text->currentSynObj->iniCall,SET(lastHint,-1));
  }
  else
    PutDelHint(text->currentSynObj);
}

void CExecView::OnDivide()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(Slash_T,new DivideOpV());
}

void CExecView::OnEditCopy()
{
  // TODO: Add your command handler code here

  if (!EditOK())
    return;
  if (!EnableCopy())
    return;

  if (clipBoardObject)
    delete clipBoardObject;
  if (text->currentSynObj->primaryToken == TDOD_T)
    clipBoardObject = (SynObject*)text->currentSynObj->parentObject->Clone();
  else
    clipBoardObject = (SynObject*)text->currentSynObj->Clone();
  clipBoardDoc = myDoc;
  //wxTheApp->updateButtonsMenus();
}

void CExecView::OnEditCut()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (clipBoardObject)
    delete clipBoardObject;
  on_editCutAction_triggered = true;
  OnDelete();
}

void CExecView::OnEditPaste()
{
  // TODO: Add your command handler code here
  SynObject *insObj;
  CHE *che, *newChe;
  CHAINX *chx, *chxp;
  MultipleOp *multOp;

  if (!EditOK()) return;

  chx = text->currentSynObj->containingChain;
  insObj = (SynObject*)clipBoardObject->Clone();
  insObj->MakeTable((address)&myDoc->IDTable,0,insObj,onConstrCopy,0,0);
  insObj->MakeTable((address)&myDoc->IDTable,0,insObj,onCopy,0,0);
//  insObj->MakeTable((address)&myDoc->IDTable,0,insObj->parentObject,onConstrCopy,0,0);
//  insObj->MakeTable((address)&myDoc->IDTable,0,insObj->parentObject,onCopy,0,0);

  if (text->currentSynObj->primaryToken == TDOD_T)
    text->currentSynObj = text->currentSynObj->parentObject;

  if (text->currentSynObj->IsPlaceHolder()
  || (!text->currentSynObj->IsStatement()
      && !text->currentSynObj->IsRepeatableClause(chxp))) {
    insObj->replacedType = text->currentSynObj->type;
    InsertOrReplace(insObj);
  }
  else if (chx) {
    che = (CHE*)text->currentSynObj->whereInParent;
    newChe = NewCHE(insObj);
    if (insertBefore)
      che = (CHE*)che->predecessor;
    text->currentSynObj = text->currentSynObj->parentObject;
    PutInsChainHint(newChe,
      chx,
      che);
  }
  else {
    multOp = new SemicolonOpV();
    PutInsMultOpHint(multOp);
    text->currentSynObj = multOp;
    newChe = NewCHE(insObj);
    if (insertBefore)
      PutInsChainHint(newChe,&multOp->operands,(CHE*)0,SET(lastHint,-1));
    else
      PutInsChainHint(newChe,&multOp->operands,(CHE*)multOp->operands.first,SET(lastHint,-1));
  }
}

void CExecView::OnEditUndo ()
{

  // TODO: Add your command handler code here

  if (!EditOK()) return;
  //text.selectAt = text->currentSynObj;
  GetDocument()->UndoMem.OnEditUndo();
}

void CExecView::OnEditRedo ()
{

  // TODO: Add your command handler code here

  if (!EditOK()) return;
  //text.selectAt = text->currentSynObj;
  GetDocument()->UndoMem.OnEditRedo();
}

void CExecView::OnEq()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->IsBinaryOp())
    PutChgOpHint(Equal_T);
  else {
    BinaryOp *binOp = new BinaryOpV(Equal_T);
    InsertOrReplace(binOp);
  }

}

void CExecView::OnDeclare()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  Declare *dcl = new DeclareV(true);
  InsertOrReplace(dcl);
}

void CExecView::OnExists()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  Exists *ex = new ExistsV(text->currentSynObj->ReadOnlyContext() == noROContext);
  InsertOrReplace(ex);
}

void CExecView::OnForeach()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  Foreach *foreach = new ForeachV(text->currentSynObj->ReadOnlyContext() == noROContext);
  InsertOrReplace(foreach);
}

void CExecView::OnGe()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->IsBinaryOp())
    PutChgOpHint(GreaterEqual_T);
  else {
    BinaryOp *binOp = new BinaryOpV(GreaterEqual_T);
    InsertOrReplace(binOp);
  }
}

void CExecView::OnGt()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->IsBinaryOp())
    PutChgOpHint(GreaterThan_T);
  else {
    BinaryOp *binOp = new BinaryOpV(GreaterThan_T);
    InsertOrReplace(binOp);
  }
}

void CExecView::OnIf()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  IfStatement *ifStm = new IfStatementV(true);
  InsertOrReplace(ifStm);
}

void CExecView::OnIfdef()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  IfdefStatement *ifdefStm = new IfdefStatementV(true);
  InsertOrReplace(ifdefStm);
}

void CExecView::OnIfExpr()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  IfExpression *ifExp = new IfExpressionV(true);
  InsertOrReplace(ifExp);
}

void CExecView::OnElseExpr()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  ElseExpression *elseExp = new ElseExpressionV(true);
  InsertOrReplace(elseExp);
}

void CExecView::OnIn()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InSetStatement *inSetStm = new InSetStatementV(true);
  InsertOrReplace(inSetStm);
}

void CExecView::OnInsert()
{
  // TODO: Add your command handler code here
  SynObject *synObj = text->currentSynObj;

  if (!EditOK()) return;

  if (insertBefore)
    InsertBefore();
  else
    InsertAfter();
}

void CExecView::OnInsertBefore()
{
  // TODO: Add your command handler code here

  insertBefore = !insertBefore;
  //wxTheApp->updateButtonsMenus();
}

void CExecView::InsertAfter()
{
  // TODO: Add your command handler code here
  SynObject *insObj, *currVarItem, *newVarItem;
  Quantifier *qf;
  Declare *dcl;
  CHAINX *chx;
  CHE *che, *newChe;
  QString str;
  bool withSet=true, isInsert=false;

  if ((text->currentSelection->data.token == Tilde_T
  || text->currentSelection->data.token == Dollar_T
  || text->currentSelection->data.token == Equal_T)
  && text->currentSynObj->primaryToken == TypeRef_T)
    text->currentSelection = text->currentSynObj->primaryTokenNode;

  switch (text->currentSelection->data.token) {
  case elsif_T:
  case then_T:
    che = (CHE*)text->currentSynObj->whereInParent;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->primaryToken == if_T) {
      insObj = new IfThenV(true);
      newChe = NewCHE(insObj);
      PutInsChainHint(newChe,
        chx,
        che);
    }
    else {
      insObj = new IfxThenV(true);
      newChe = NewCHE(insObj);
      PutInsChainHint(newChe,
        chx,
        che);
    }
    return;

  case catch_T:
    che = (CHE*)text->currentSynObj->whereInParent;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    insObj = new CatchClauseV(true);
    newChe = NewCHE(insObj);
    PutInsChainHint(newChe,
      chx,
      che);
    return;

  case case_T:
    che = (CHE*)text->currentSynObj->whereInParent;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    insObj = new BranchV(true);
    newChe = NewCHE(insObj);
    PutInsChainHint(newChe,
      chx,
      che);
    return;

  case caseType_T:
    che = (CHE*)text->currentSynObj->whereInParent;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    insObj = new TypeBranchV(true);
    newChe = NewCHE(insObj);
    PutInsChainHint(newChe,
      chx,
      che);
    return;

  case TDOD_T:
    if (text->currentSynObj->primaryToken != ObjRef_T)
      text->currentSynObj = text->currentSynObj->parentObject;
  case TypePH_T:
  case TypePHopt_T:
  case TypeRef_T:
  case SetPH_T:
  case CrtblPH_T:
  case Callee_T:
  case CompObj_T:
  case CrtblRef_T:
    if (text->currentSynObj->primaryToken == quant_T) {
      if (text->currentSynObj->parentObject->primaryToken == declare_T)
        withSet = false;
      goto quantCase;
    }
    if (text->currentSynObj->parentObject->primaryToken == quant_T) {
      text->currentSynObj = text->currentSynObj->parentObject;
      if (text->currentSynObj->parentObject->primaryToken == declare_T)
        withSet = false;
      goto quantCase;
    }
    else
      goto deflt;
  case from_T:
  case in_T:
quantCase:
    qf = new QuantifierV(withSet);
    newChe = NewCHE(qf);
    che = (CHE*)text->currentSynObj->whereInParent;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->primaryToken == declare_T
    && ((Declare*)text->currentSynObj)->secondaryClause.ptr) {
      dcl = (Declare*)text->currentSynObj;
      PutInsChainHint(newChe,chx,che,SET(firstHint,-1));
	    newVarItem = (SynObject*)((CHE*)qf->quantVars.first)->data;
      PutIniCall(newVarItem);
    }
    else
      PutInsChainHint(newChe,chx,che);
    return;

  case VarPH_T:
  case VarName_T:
    if (!EditOK())
      return;
    che = (CHE*)text->currentSynObj->whereInParent;
    currVarItem = text->currentSynObj;
    chx = currVarItem->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    newVarItem = new SynObjectV(VarPH_T);
    newChe = NewCHE(newVarItem);
    qf = (Quantifier*)text->currentSynObj;
   // newVarItem->parentObject = qf;
   // newVarItem->whereInParent = (address)newChe;
	  //newVarItem->containingChain = &qf->quantVars;
   // newChe->predecessor = (CHE*)currVarItem->whereInParent;
    //:needed in PutIniCall although PutInsChainHint isn't executed immediately
    text->selectAt = newVarItem;
    if (text->currentSynObj->parentObject->IsDeclare()
    && ((Declare*)text->currentSynObj->parentObject)->secondaryClause.ptr) {
      PutInsChainHint(newChe,chx,che,SET(firstHint,-1));
      PutIniCall(newVarItem);
    }
    else
      PutInsChainHint(newChe,chx,che);
    return;

  default:
deflt:
    if (text->currentSynObj->primaryToken == quant_T) {
      if (text->currentSynObj->parentObject->primaryToken == declare_T)
        withSet = false;
      goto quantCase;
    }
    if (text->currentSynObj->parentObject
    && text->currentSynObj->parentObject->IsFuncInvocation()
    && !text->currentSynObj->containingChain)
      text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->containingChain) {
      insObj = new SynObjectV(text->currentSynObj->replacedType);
      newChe = NewCHE(insObj);
      che = (CHE*)text->currentSynObj->whereInParent;
      chx = text->currentSynObj->containingChain;
      text->currentSynObj = text->currentSynObj->parentObject;
      PutInsChainHint(newChe,
        chx,
        che);
    }
    else // single <stm>
      OnAnd();
  }
}


void CExecView::InsertBefore()
{
  // TODO: Add your command handler code here
  SynObject *insObj, *currVarItem, *newVarItem;
  Quantifier *qf;
  Declare *dcl;
  CHAINX *chx;
  CHE *che, *newChe;
  QString str;
  bool withSet=true, isInsert=false;

  if ((text->currentSelection->data.token == Tilde_T
  || text->currentSelection->data.token == Dollar_T
  || text->currentSelection->data.token == Equal_T)
  && text->currentSynObj->primaryToken == TypeRef_T)
    text->currentSelection = text->currentSynObj->primaryTokenNode;
  switch (text->currentSelection->data.token) {
  case elsif_T:
  case then_T:
    chx = text->currentSynObj->containingChain;
    che = (CHE*)text->currentSynObj->whereInParent;
    che = (CHE*)che->predecessor;
    text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->primaryToken == if_T) {
      insObj = new IfThenV(true);
      newChe = NewCHE(insObj);
      PutInsChainHint(newChe,
        chx,
        che);
    }
    else {
      insObj = new IfxThenV(true);
      newChe = NewCHE(insObj);
      PutInsChainHint(newChe,
        chx,
        che);
    }
    return;

  case catch_T:
    che = (CHE*)text->currentSynObj->whereInParent;
    che = (CHE*)che->predecessor;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    insObj = new CatchClauseV(true);
    newChe = NewCHE(insObj);
    PutInsChainHint(newChe,
      chx,
      che);
    return;

  case case_T:
    che = (CHE*)text->currentSynObj->whereInParent;
    che = (CHE*)che->predecessor;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    insObj = new BranchV(true);
    newChe = NewCHE(insObj);
    PutInsChainHint(newChe,
      chx,
      che);
    return;

  case caseType_T:
    che = (CHE*)text->currentSynObj->whereInParent;
    che = (CHE*)che->predecessor;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    insObj = new TypeBranchV(true);
    newChe = NewCHE(insObj);
    PutInsChainHint(newChe,
      chx,
      che);
    return;

  case TDOD_T:
    if (text->currentSynObj->primaryToken != ObjRef_T)
      text->currentSynObj = text->currentSynObj->parentObject;
  case TypePH_T:
  case TypePHopt_T:
  case TypeRef_T:
  case SetPH_T:
  case CrtblPH_T:
  case Callee_T:
  case CompObj_T:
  case CrtblRef_T:
    if (text->currentSynObj->primaryToken == quant_T) {
      if (text->currentSynObj->parentObject->primaryToken == declare_T)
        withSet = false;
      goto quantCase;
    }
    if (text->currentSynObj->parentObject->primaryToken == quant_T) {
      text->currentSynObj = text->currentSynObj->parentObject;
      if (text->currentSynObj->parentObject->primaryToken == declare_T)
        withSet = false;
      goto quantCase;
    }
    else
      goto deflt;
  case from_T:
  case in_T:
quantCase:
    qf = new QuantifierV(withSet);
    newChe = NewCHE(qf);
    che = (CHE*)text->currentSynObj->whereInParent;
    che = (CHE*)che->predecessor;
    chx = text->currentSynObj->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->primaryToken == declare_T
    && ((Declare*)text->currentSynObj)->secondaryClause.ptr) {
      dcl = (Declare*)text->currentSynObj;
      PutInsChainHint(newChe,chx,che,SET(firstHint,-1));
	    newVarItem = (SynObject*)((CHE*)qf->quantVars.first)->data;
      PutIniCall(newVarItem,false);
    }
    else
      PutInsChainHint(newChe,chx,che);
    return;

  case VarPH_T:
  case VarName_T:
    if (!EditOK())
      return;
    che = (CHE*)text->currentSynObj->whereInParent;
    che = (CHE*)che->predecessor;
    currVarItem = text->currentSynObj;
    chx = currVarItem->containingChain;
    text->currentSynObj = text->currentSynObj->parentObject;
    newVarItem = new SynObjectV(VarPH_T);
    newChe = NewCHE(newVarItem);
    qf = (Quantifier*)text->currentSynObj;
    //newVarItem->parentObject = qf;
    //newVarItem->whereInParent = (address)newChe;
    //newVarItem->containingChain = (CHAINX*)chx;
    ////:needed in PutIniCall although PutInsChainHint isn't executed immediately
    if (text->currentSynObj->parentObject->IsDeclare()
    && ((Declare*)text->currentSynObj->parentObject)->secondaryClause.ptr) {
      PutInsChainHint(newChe,chx,che,SET(firstHint,-1));
      PutIniCall(newVarItem,false);
    }
    else
      PutInsChainHint(newChe,chx,che);
    text->selectAt = newVarItem;
    return;

  default:
deflt:
    if (text->currentSynObj->primaryToken == quant_T) {
      if (text->currentSynObj->parentObject->primaryToken == declare_T)
        withSet = false;
      goto quantCase;
    }
    if (text->currentSynObj->parentObject
    && text->currentSynObj->parentObject->IsFuncInvocation()
    && !text->currentSynObj->containingChain)
      text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->containingChain) {
      insObj = new SynObjectV(text->currentSynObj->replacedType);
      newChe = NewCHE(insObj);
      che = (CHE*)text->currentSynObj->whereInParent;
      che = (CHE*)che->predecessor;
      chx = text->currentSynObj->containingChain;
      text->currentSynObj = text->currentSynObj->parentObject;
      PutInsChainHint(newChe,
        chx,
        che);
    }
    else // single <stm>
      OnAnd();
  }
}

void CExecView::OnInvert()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InvertOp *invertOp = new InvertOpV(true);
  InsertOrReplace(invertOp);
}

void CExecView::OnEditSel()
{
  // TODO: Add your message handler code here and/or call default

  if (text->currentSynObj->BoolAdmissibleOnly(text->ckd)
  || text->currentSynObj->EnumAdmissibleOnly(text->ckd))
    return;
  doubleClick = true;
  clicked = true;
  Select();
  clicked = false;
  doubleClick = false;
}

void CExecView::OnLe()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->IsBinaryOp())
    PutChgOpHint(LessEqual_T);
  else {
    BinaryOp *binOp = new BinaryOpV(LessEqual_T);
    InsertOrReplace(binOp);
  }
}

void CExecView::OnLshift()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSelection->data.token == Rshift_T)
    PutChgOpHint(Lshift_T);
  else
    InsMultOp(Lshift_T,new LshiftOpV());
}

void CExecView::OnLt()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->IsBinaryOp())
    PutChgOpHint(LessThan_T);
  else {
    BinaryOp *binOp = new BinaryOpV(LessThan_T);
    InsertOrReplace(binOp);
  }
}

void CExecView::OnMult()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->type == ObjPH_T
      && text->currentSynObj->parentObject->IsFuncInvocation()
      && text->currentSynObj->whereInParent
      == (address)&((FuncExpression*)text->currentSynObj->parentObject)->handle.ptr) {
    SynObject *ast = new SynObjectV(Mult_T);
    ast->type = ObjPH_T;
    PutInsHint(ast);
  }
  else
    InsMultOp(Mult_T,new MultOpV());
}

void CExecView::OnNe()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->IsBinaryOp())
    PutChgOpHint(NotEqual_T);
  else {
    BinaryOp *binOp = new BinaryOpV(NotEqual_T);
    InsertOrReplace(binOp);
  }
}

void CExecView::OnShowComments()
{
  // TODO: Add your command handler code here
  if (!EditOK()) return;
  text->showComments = !text->showComments;
  if (text->showComments)
    myDECL->TreeFlags.INCL(ShowExecComments);
  else
    myDECL->TreeFlags.EXCL(ShowExecComments);

  RedrawExec(text->selectAt);
}

void CExecView::OnNot()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  LogicalNot *logNot = new LogicalNotV(true);
  InsertOrReplace(logNot);
}

void CExecView::OnNull()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  NullConst *nConst = new NullConstV();
  if (text->currentSynObj->primaryToken == FuncPH_T
  || text->currentSynObj->primaryToken == FuncRef_T)
    nConst->replacedType = FuncPH_T;
  InsertOrReplace(nConst);
}

void CExecView::OnOr()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(or_T,new OrOpV());
}

void CExecView::OnPlus()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(Plus_T,new PlusOpV());
}

void CExecView::OnPlusMinus()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSelection->data.token == Plus_T
  && text->currentSynObj->IsMultOp())
    text->currentSynObj = ((CHETokenNode*)text->currentSelection->successor)->data.synObject;
  PutPlusMinusHint();
}

void CExecView::OnRshift()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSelection->data.token == Lshift_T)
    PutChgOpHint(Rshift_T);
  else
    InsMultOp(Rshift_T,new RshiftOpV());
}

void CExecView::OnShowOptionals()
{
  SynObject *delObj=0, *insObj=0, *thenPart=0, *elsePart=0, *branch=0, *orgunit=0, *oid=0, *iid=0,
            *statement=0, *secondaryClause=0;
  CHAINX *chx;
  CHE *outParm;
  unsigned nOpt=0;
  bool isNewClause, isCompObjSpec=false, isFirst=true, hasOpt=false, singleParm;
  AttachObject *attachObject;
  Foreach *foreachStm;
  Declare *declareStm;
  Exists *existsStm;
  TryStatement *tryStm;
  Quantifier *quant;
  FailStatement *failStm;
  FuncStatement *funcStm;
  CloneExpression *cloneExpr;
  NewExpression *newExpr;
  LavaDECL *decl;

  // TODO: Add your command handler code here

  if (!EditOK()) return;

  if  ((  (text->currentSelection->data.token == TypePH_T
          || text->currentSelection->data.token == TypePHopt_T
          || text->currentSelection->data.token == TypeRef_T
          || text->currentSelection->data.token == SetPH_T
          || text->currentSelection->data.token == ObjRef_T
          )
        && text->currentSynObj->parentObject->primaryToken == quant_T)
       || text->currentSelection->data.token == FuncRef_T)
    text->currentSynObj = text->currentSynObj->parentObject;

  //forcePrimTokenSelect = true;
  if (text->currentSynObj->primaryToken == attach_T
  || text->currentSynObj->primaryToken == run_T
  || text->currentSynObj->primaryToken == new_T) {
    isNewClause = text->currentSynObj->primaryToken == new_T;
    attachObject = (AttachObject*)text->currentSynObj;
    if (((AttachObject*)text->currentSynObj)->objType.ptr && !IsPH(((AttachObject*)text->currentSynObj)->objType.ptr)) {
      decl = myDoc->IDTable.GetDECL(((Reference*)((AttachObject*)text->currentSynObj)->objType.ptr)->refID,text->ckd.inINCL);
      if (decl && decl->DeclType == CompObjSpec) {
        isCompObjSpec = true;
        hasOpt = true;
      }
    }
    else if (text->currentSynObj->primaryToken == attach_T)
      hasOpt = true;

    if (isNewClause) {
      newExpr = (NewExpression*)text->currentSynObj;
      if (!isCompObjSpec && !newExpr->butStatement.ptr) nOpt++;
    }
//    if (hasOpt && !attachObject->orgunit.ptr) nOpt++;
    if (hasOpt && !attachObject->url.ptr) nOpt++;
    if (isNewClause)
      if (!isCompObjSpec && !newExpr->butStatement.ptr) {
        insObj = new SynObjectV(Stm_T);
        text->currentSynObj = insObj;
        insObj->parentObject = newExpr;
        insObj->whereInParent = (address)&newExpr->butStatement.ptr;
        if (isFirst) {
          isFirst = false;
          --nOpt;
          if (nOpt)
            PutInsHint(insObj,SET(firstHint,-1));
          else
            PutInsHint(insObj,SET(firstHint,lastHint,-1));
        }
        else {
          --nOpt;
          if (nOpt)
            PutInsHint(insObj,SET());
          else
            PutInsHint(insObj,SET(lastHint,-1));
        }
      }

    if (hasOpt && !attachObject->url.ptr) {
      insObj = new SynObjectV(Exp_T);
      text->currentSynObj = insObj;
      insObj->parentObject = attachObject;
      insObj->whereInParent = (address)&attachObject->url.ptr;
        if (isFirst) {
          isFirst = false;
          --nOpt;
          if (nOpt)
            PutInsHint(insObj,SET(firstHint,-1));
          else
            PutInsHint(insObj,SET(firstHint,lastHint,-1));
        }
        else {
          --nOpt;
          if (nOpt)
            PutInsHint(insObj,SET());
          else
            PutInsHint(insObj,SET(lastHint,-1));
        }
    }
  }
  else if (text->currentSynObj->primaryToken == clone_T) {
    cloneExpr = (CloneExpression*)text->currentSynObj;
    if (!cloneExpr->butStatement.ptr) {
      insObj = new SynObjectV(Stm_T);
      text->currentSynObj = insObj;
      insObj->parentObject = cloneExpr;
      insObj->whereInParent = (address)&cloneExpr->butStatement.ptr;
      PutInsHint(insObj,SET(firstHint,lastHint,-1));
    }
  }
  else if (text->currentSynObj->primaryToken == foreach_T) {
    foreachStm = (Foreach*)text->currentSynObj;
    if (!foreachStm->primaryClause.ptr) nOpt++;
    if (!foreachStm->secondaryClause.ptr) nOpt++;
    if (!foreachStm->primaryClause.ptr) {
      insObj = new SynObjectV(Stm_T);
      text->currentSynObj = insObj;
      insObj->parentObject = foreachStm;
      insObj->whereInParent = (address)&foreachStm->primaryClause.ptr;
      if (nOpt > 1)
        PutInsHint(insObj,SET(firstHint,-1));
      else
        PutInsHint(insObj,SET(firstHint,lastHint,-1));
    }
    if (!foreachStm->secondaryClause.ptr) {
      insObj = new SynObjectV(Stm_T);
      text->currentSynObj = insObj;
      insObj->parentObject = foreachStm;
      insObj->whereInParent = (address)&foreachStm->secondaryClause.ptr;
      if (nOpt > 1)
        PutInsHint(insObj,SET(lastHint,-1));
      else
        PutInsHint(insObj,SET(firstHint,lastHint,-1));
    }
  }
  else if (text->currentSynObj->primaryToken == exists_T) {
    existsStm = (Exists*)text->currentSynObj;
    if (!existsStm->secondaryClause.ptr) {
      insObj = new SynObjectV(Stm_T);
      text->currentSynObj = insObj;
      insObj->parentObject = existsStm;
      insObj->whereInParent = (address)&existsStm->secondaryClause.ptr;
      PutInsHint(insObj,SET(firstHint,lastHint,-1));
    }
  }
  else if (text->currentSynObj->primaryToken == declare_T) {
    declareStm = (Declare*)text->currentSynObj;
    if (!declareStm->secondaryClause.ptr)
  		RestoreIniCalls();
  }
  else if (text->currentSynObj->primaryToken == try_T) {
    tryStm = (TryStatement*)text->currentSynObj;
    if (!tryStm->catchClauses.first) {
      text->currentSynObj->InsertBranch(branch,chx);
      PutInsChainHint(NewCHE(branch),chx,(CHE*)chx->first,SET(firstHint,lastHint,-1));
    }
/*  if (!tryStm->elsePart.ptr) {
      insObj = new SynObjectV(Stm_T);
      text->currentSynObj = insObj;
      insObj->parentObject = tryStm;
      insObj->whereInParent = (address)&tryStm->elsePart.ptr;
      PutInsHint(insObj,SET(firstHint,lastHint,-1));
    }*/
  }
  else if (text->currentSynObj->primaryToken == quant_T
  && ((Quantifier*)text->currentSynObj)->set.ptr) {
    quant = (Quantifier*)text->currentSynObj;
    if (!quant->elemType.ptr) {
      insObj = new SynObjectV(TypePH_T);
      insObj->parentObject = quant;
      insObj->whereInParent = (address)&quant->elemType.ptr;
      text->currentSynObj = insObj;
      PutInsHint(insObj,SET(firstHint,lastHint,-1));
    }
  }
  else if (text->currentSynObj->primaryToken == fail_T) {
    failStm = (FailStatement*)text->currentSynObj;
    if (!failStm->exception.ptr) {
      insObj = new SynObjectV(Exp_T);
      insObj->parentObject = failStm;
      insObj->whereInParent = (address)&failStm->exception.ptr;
      text->currentSynObj = insObj;
      PutInsHint(insObj,SET(firstHint,lastHint,-1));
    }
  }
  else if (text->currentSynObj->primaryToken == ifx_T) {
    text->currentSynObj->InsertBranch(branch,chx);
    if (branch)
      PutInsChainHint(NewCHE(branch),chx,(CHE*)chx->first,SET(firstHint,lastHint,-1));
  }
  else if (text->currentSynObj->primaryToken == assignFS_T) {
    funcStm = (FuncStatement*)text->currentSynObj;
    if (funcStm->outputs.first == funcStm->outputs.last)
      singleParm = true;
    else
      singleParm = false;
    for (outParm=(CHE*)funcStm->outputs.first;
         outParm;
         outParm = (CHE*)outParm->successor) {
      text->currentSynObj = (SynObject*)((Parameter*)outParm->data)->parameter.ptr;
      if (singleParm)
        PutDelFlagHint(SET(ignoreSynObj,-1),SET(firstHint,lastHint,-1));
      else if (outParm == (CHE*)funcStm->outputs.first)
        PutDelFlagHint(SET(ignoreSynObj,-1),SET(firstHint,-1));
      else if (outParm == (CHE*)funcStm->outputs.last)
        PutDelFlagHint(SET(ignoreSynObj,-1),SET(lastHint,-1));
      else
        PutDelFlagHint(SET(ignoreSynObj,-1),SET());
    }
  }
  else if (text->currentSynObj->primaryToken == ifdef_T) {
    ((IfdefStatementV*)text->currentSynObj)->Insert2Optionals(thenPart,elsePart);
    if (thenPart || elsePart) {
      if (thenPart && elsePart) {
        text->currentSynObj = thenPart;
        PutInsHint(thenPart,SET(firstHint,-1));
        text->currentSynObj = elsePart;
        PutInsHint(elsePart,SET(lastHint,-1));
      }
      else if (thenPart) {
        text->currentSynObj = thenPart;
        PutInsHint(thenPart,SET(firstHint,lastHint,-1));
      }
      else {
        text->currentSynObj = elsePart;
        PutInsHint(elsePart,SET(firstHint,lastHint,-1));
      }
    }
  }
  else { // if, [type]switch
    text->currentSynObj->Insert2Optionals(elsePart,branch,chx);
    if (elsePart || branch) {
      if (elsePart && branch) {
        PutInsChainHint(NewCHE(branch),chx,(CHE*)chx->first,SET(firstHint,-1));
        text->currentSynObj = elsePart;
        PutInsHint(elsePart,SET(lastHint,-1));
      }
      else if (branch) {
        if (text->currentSynObj->IsIfStmExpr())
          PutInsChainHint(NewCHE(branch),chx,(CHE*)chx->first,SET(firstHint,lastHint,-1));
        else
          PutInsChainHint(NewCHE(branch),chx,0,SET(firstHint,lastHint,-1));
      }
      else {
        text->currentSynObj = elsePart;
        PutInsHint(elsePart,SET(firstHint,lastHint,-1));
      }
    }
  }
}

void CExecView::RestoreIniCalls() {
  Declare *declareStm = (Declare*)text->currentSynObj;
  CHE *chpq, *chpv;

  for (chpq = (CHE*)declareStm->quantifiers.first;
       chpq;
       chpq = (CHE*)chpq->successor) {
    for (chpv = (CHE*)((Quantifier*)chpq->data)->quantVars.first;
         chpv;
         chpv = (CHE*)chpv->successor) {
      text->currentSynObj = (Quantifier*)chpq->data;
      PutIniCall((SynObject*)chpv->data,true,true);
    }
  }
}

bool CExecView::EnableGotoDecl()
{
  // TODO: Add your command update UI handler code here
  DWORD dw;
  TIDType idtype=noID;
  TDOD *tdod;
  TID tid, setTid, tidOperatorFunc;
  CHE *che;
  LavaDECL *decl, *setDecl;
  SynObject *typeObj, *synObj;
  Quantifier *quant;
  CloneExpression *makeClone;
  Category cat;
  SynFlags ctxFlags;

  if (text->currentSelection->data.token == Comment_T)
    return false;

  switch (text->currentSynObj->primaryToken) {
  case FuncRef_T:
  case TypeRef_T:
  case CrtblRef_T:
    dw = myDoc->IDTable.GetVar(((Reference*)text->currentSynObj)->refID,idtype,text->ckd.inINCL);
    if (idtype != noID) return true;
    break;
  case enumConst_T:
    dw = myDoc->IDTable.GetVar(((EnumConst*)text->currentSynObj)->refID,idtype,text->ckd.inINCL);
    if (idtype != noID) return true;
    break;
  case TDOD_T:
    tdod = (TDOD*)text->currentSynObj;
    dw = myDoc->IDTable.GetVar(tdod->ID,idtype,text->ckd.inINCL);
    synObj = (SynObject*)dw;
    if (idtype == noID) return false;
    if (!tdod->errorChain.first) return true;
    for (che = (CHE*)tdod->errorChain.first; che; che = (CHE*)che->successor)
      if (((CLavaError*)che->data)->IDS == &ERR_Broken_ref)
        return false;
    return true;
  case VarName_T:
    typeObj = ((VarName*)text->currentSynObj)->TypeRef();
    if (typeObj)
      if (typeObj->IsPlaceHolder())
        return false;
      else
        return true;
    else if (text->currentSynObj->parentObject->primaryToken == quant_T) { // typeless quant. set var
      quant = (Quantifier*)text->currentSynObj->parentObject;
      ((SynObject*)quant->set.ptr)->ExprGetFVType(text->ckd,setDecl,cat,ctxFlags);
      setDecl = text->ckd.document->GetType(setDecl);
      if (!setDecl) return false;
      if (((SynObject*)quant->set.ptr)->primaryToken == intIntv_T
      || ((SynObject*)quant->set.ptr)->primaryToken == TypeRef_T) {
        tid = OWNID(setDecl);
      }
      else
        text->ckd.document->IDTable.GetParamID(setDecl,tid,isSet);
      if (tid.nID > 0) return true;
    }
    else {  // clone temp variable
      makeClone = (CloneExpression*)text->currentSynObj->parentObject;
      ((CloneExpression*)makeClone->fromObj.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      if (!decl) return false;
      decl = text->ckd.document->GetType(decl);
      tid = OWNID(decl);
      if (tid.nID > 0) return true;
    }
    break;
  default:
    if (text->currentSynObj->primaryToken == arrayAtIndex_T) {
      ((SynObject*)((ArrayAtIndex*)text->currentSynObj)->arrayObj.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      decl = text->ckd.document->GetType(decl);
    }
    else if (text->currentSynObj->IsMultOp() && text->currentSynObj->ExpressionSelected(text->currentSelection)) {
      ((SynObject*)((CHE*)((MultipleOp*)text->currentSynObj)->operands.first)->data)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      decl = text->ckd.document->GetType(decl);
    }
    else if (text->currentSynObj->IsBinaryOp()) {
      ((SynObject*)((BinaryOp*)text->currentSynObj)->operand1.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      decl = text->ckd.document->GetType(decl);
    }
    else if (text->currentSynObj->IsUnaryOp()) {
      ((SynObject*)((UnaryOp*)text->currentSynObj)->operand.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
      decl = text->ckd.document->GetType(decl);
    }
    else
      return false;
    myDoc->GetOperatorID(decl,(TOperator)(text->currentSynObj->primaryToken-not_T),tidOperatorFunc);
    if (tidOperatorFunc.nID > 0) return true;
  }

  return false;
}

void CExecView::OnGotoDecl()
{
  // TODO: Add your command handler code here
  SynObject *synObj, *parent;
  TIDType idtype;
  TDOD *tdod;
  TID tid, setTid, tidOperatorFunc;
  LavaDECL *decl, *setDecl;
  SynObject *typeObj;
  CloneExpression *makeClone;
  Quantifier *quant;
  TID itfTid;
  Category cat;
  SynFlags ctxFlags;

  if (!EditOK()) return;

  Base->Browser->LastBrowseContext = new CBrowseContext(this,text->currentSynObj);
  switch (text->currentSelection->data.token) {
    case FuncRef_T:
    case TypeRef_T:
    case CrtblRef_T:
      Base->Browser->BrowseDECL(GetDocument(),((Reference*)text->currentSynObj)->refID);
      break;
    case enumConst_T:
      Base->Browser->BrowseDECL(GetDocument(),((EnumConst*)text->currentSynObj)->refID, &((EnumConst*)text->currentSynObj)->Id);
      break;
    case VarName_T:
      typeObj = ((VarName*)text->currentSynObj)->TypeRef();
      if (typeObj) {
        Select(typeObj);
      }
      else if (text->currentSynObj->parentObject->primaryToken == quant_T) { // typeless quant. set var
        quant = (Quantifier*)text->currentSynObj->parentObject;
        quant = (Quantifier*)text->currentSynObj->parentObject;
        ((SynObject*)quant->set.ptr)->ExprGetFVType(text->ckd,setDecl,cat,ctxFlags);
        setDecl = text->ckd.document->GetType(setDecl);
        if (!setDecl) return;
        if (((SynObject*)quant->set.ptr)->primaryToken == intIntv_T
        || ((SynObject*)quant->set.ptr)->primaryToken == TypeRef_T)
          tid = OWNID(setDecl);
        else
          text->ckd.document->IDTable.GetParamID(setDecl,tid,isSet);
        Base->Browser->BrowseDECL(GetDocument(),tid);
      }
      else {  // clone temp variable
        makeClone = (CloneExpression*)text->currentSynObj->parentObject;
        ((CloneExpression*)makeClone->fromObj.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
        decl = text->ckd.document->GetType(decl);
        tid = OWNID(decl);
        Base->Browser->BrowseDECL(GetDocument(),tid);
      }
      break;
    case TDOD_T:
      tdod = (TDOD*)text->currentSynObj;
      synObj = (SynObject*)myDoc->IDTable.GetVar(tdod->ID,idtype,text->ckd.inINCL);
      if (idtype == globalID)
        Base->Browser->BrowseDECL(GetDocument(),tdod->ID);
      else if (((ObjReference*)tdod->parentObject)->OutOfScope(text->ckd)) {
        if (synObj->parentObject)
          for (parent = synObj->parentObject;
               parent->parentObject;
               parent = parent->parentObject);
        else
          parent = synObj;
        myDoc->OpenExecView(((SelfVar*)parent)->execDECL);
        if (tdod->parentObject->flags.Contains(isSelfVar))
          ((CExecView*)((SelfVar*)parent)->execView)->Select((SynObject*)((SelfVar*)parent)->execName.ptr);
        else
          ((CExecView*)((SelfVar*)parent)->execView)->Select(synObj);
      }
      else if (tdod->parentObject->flags.Contains(isSelfVar))
        Select((SynObject*)((SelfVar*)synObj)->execName.ptr);
      else
        Select(synObj);
      break;
    default:
      if (text->currentSynObj->primaryToken == arrayAtIndex_T) {
        ((SynObject*)((ArrayAtIndex*)text->currentSynObj)->arrayObj.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
        decl = text->ckd.document->GetType(decl);
      }
      else if (text->currentSynObj->IsMultOp()) {
        ((SynObject*)((CHE*)((MultipleOp*)text->currentSynObj)->operands.first)->data)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
        decl = text->ckd.document->GetType(decl);
      }
      else if (text->currentSynObj->IsBinaryOp()) {
        ((SynObject*)((BinaryOp*)text->currentSynObj)->operand1.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
        decl = text->ckd.document->GetType(decl);
      }
      else if (text->currentSynObj->IsUnaryOp()) {
        ((SynObject*)((UnaryOp*)text->currentSynObj)->operand.ptr)->ExprGetFVType(text->ckd,decl,cat,ctxFlags);
        decl = text->ckd.document->GetType(decl);
      }
      myDoc->GetOperatorID(decl,(TOperator)(text->currentSynObj->primaryToken-not_T),tidOperatorFunc);
      Base->Browser->BrowseDECL(GetDocument(),tidOperatorFunc);
  }
}

void CExecView::OnGotoImpl()
{
  if (!EditOK()) return;
  LavaDECL* decl;
  Base->Browser->LastBrowseContext = new CBrowseContext(this,text->currentSynObj);
  switch (text->currentSelection->data.token) {
  case FuncRef_T:
    decl = GetDocument()->IDTable.GetDECL(((Reference*)text->currentSynObj)->refID);
    if (decl && (decl->TypeFlags.Contains(isAbstract) || decl->TypeFlags.Contains(isNative))) {
      QMessageBox::information(this, qApp->applicationName(),ERR_NoImplForAbstract,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
      Base->Browser->DestroyLastBrsContext();
    }
    else
      if (!Base->Browser->GotoImpl(GetDocument(),((Reference*)text->currentSynObj)->refID)) {
  //      AfxMessageBox(&ERR_NoFuncImpl, MB_OK+MB_ICONINFORMATION);
        QMessageBox::critical(this,qApp->applicationName(),ERR_NoFuncImpl,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        Base->Browser->DestroyLastBrsContext();
      }
    break;
  case TypeRef_T:
    if (!Base->Browser->GotoImpl(GetDocument(),((Reference*)text->currentSynObj)->refID)) {
//      AfxMessageBox(&ERR_NoClassImpl, MB_OK+MB_ICONINFORMATION);
        QMessageBox::critical(this,qApp->applicationName(),ERR_NoClassImpl,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
      Base->Browser->DestroyLastBrsContext();
    }
    break;
  case CrtblRef_T:
    if ((text->currentSynObj->parentObject->primaryToken == run_T)
    || text->currentSynObj->parentObject->primaryToken == new_T)
    if (!Base->Browser->GotoImpl(GetDocument(),((Reference*)text->currentSynObj)->refID)) {
//        AfxMessageBox(&ERR_NoClassImpl, MB_OK+MB_ICONINFORMATION);
        QMessageBox::critical(this,qApp->applicationName(),ERR_NoClassImpl,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
        Base->Browser->DestroyLastBrsContext();
      }
    break;
  default: ;
  }
}

void CExecView::OnFindReferences()
{
  CSearchData sData;

  if (!EditOK()) return;

  sData.findRefs.FWhere = findInThisView;
//  sData.findRefs.enumID = sData.enumID;
  Base->Browser->LastBrowseContext = new CBrowseContext(this,text->currentSynObj);
  switch (text->currentSelection->data.token) {
  case TypeRef_T:
  case CrtblRef_T:
  case FuncRef_T:
//    Base->Browser->OnFindRefs(myDoc,text->ckd.document->IDTable.GetDECL(((Reference*)text->currentSynObj)->refID,text->ckd.inINCL),sData.enumID,sData.findRefs);
    if (Base->Browser->OnFindRefs(myDoc,text->ckd.document->IDTable.GetDECL(((Reference*)text->currentSynObj)->refID,text->ckd.inINCL),sData.findRefs)
      && (sData.findRefs.FWhere == findInThisView)) {
      sData.findRefs.refTid = ((Reference*)text->currentSynObj)->refID;
      sData.execDECL = myDECL;//->ParentDECL;
      sData.doc = myDoc;
      text->ckd.selfVar->MakeTable((address)&text->ckd.document->IDTable, 0, (SynObjectBase*)myDECL, onSearch, 0,0, (address)&sData);
    }
    break;
  case enumConst_T:
    sData.findRefs.enumID = ((EnumConst*)text->currentSynObj)->Id;
//    Base->Browser->OnFindRefs(myDoc,text->ckd.document->IDTable.GetDECL(((EnumConst*)text->currentSynObj)->refID,text->ckd.inINCL),sData.enumID,sData.findRefs);
    if (Base->Browser->OnFindRefs(myDoc,text->ckd.document->IDTable.GetDECL(((EnumConst*)text->currentSynObj)->refID,text->ckd.inINCL),sData.findRefs)
     && (sData.findRefs.FWhere == findInThisView)) {
      sData.findRefs.refTid = ((EnumConst*)text->currentSynObj)->refID;
      sData.execDECL = myDECL;//->ParentDECL;
      sData.doc = myDoc;
      text->ckd.selfVar->MakeTable((address)&text->ckd.document->IDTable, 0, (SynObjectBase*)myDECL, onSearch, 0,0, (address)&sData);
    }
    break;
  case TDOD_T:
    if (((ObjReference*)text->currentSynObj->parentObject)->refIDs.first == (CHE*)((TDOD*)text->currentSynObj)->whereInParent) {
      //Base->Browser->OnFindRefs(myDoc,0,sData.enumID,sData.findRefs);
      if (Base->Browser->OnFindRefs(myDoc,0,sData.findRefs)) {
        sData.findRefs.refTid = ((TDOD*)text->currentSynObj)->ID;
        sData.execDECL = myDECL;//->ParentDECL;
        sData.doc = myDoc;
        text->ckd.selfVar->MakeTable((address)&text->ckd.document->IDTable, 0, (SynObjectBase*)myDECL, onSearch, 0,0, (address)&sData);
      }
    }
    else
      //Base->Browser->OnFindRefs(myDoc,text->ckd.document->IDTable.GetDECL(((TDOD*)text->currentSynObj)->ID,text->ckd.inINCL),sData.enumID,sData.findRefs);
      Base->Browser->OnFindRefs(myDoc,text->ckd.document->IDTable.GetDECL(((TDOD*)text->currentSynObj)->ID,text->ckd.inINCL),sData.findRefs);
    break;
  case VarName_T:
    //Base->Browser->OnFindRefs(myDoc,0,sData.enumID,sData.findRefs);
    sData.findRefs.searchName = ((VarName*)text->currentSynObj)->varName;
    if (Base->Browser->OnFindRefs(myDoc,0,sData.findRefs)) {
      sData.findRefs.refTid = ((VarName*)text->currentSynObj)->varID;
      sData.execDECL = myDECL;//->ParentDECL;
      sData.doc = myDoc;
      text->ckd.selfVar->MakeTable((address)&text->ckd.document->IDTable, 0, (SynObjectBase*)myDECL, onSearch, 0,0, (address)&sData);
    }
    break;
  default: ;
  }
}


void CExecView::OnSwitch()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  SwitchStatement *swStm = new SwitchStatementV(true);
  InsertOrReplace(swStm);
}

void CExecView::OnTrue()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  BoolConst *trueConst = new BoolConstV(true);
  InsertOrReplace(trueConst);
}

void CExecView::OnFalse()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  BoolConst *falseConst = new BoolConstV(false);
  InsertOrReplace(falseConst);
}

void CExecView::OnXor()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(xor_T,new XorOpV());
}
bool CExecView::EditOK()
{
  // TODO: Add your message handler code here and/or call default
  QString str;
  SynObject *synObj;
  Declare *dcl;
  VarName *varName;
  TToken token=editToken;
  VarConstCheck rc;

  if (editCtlVisible) {
    rc = editCtl->InputIsCorrect(token,m_ComboBar);
    if (rc == noChange) {
      editCtl->hide();
      editCtlVisible = false;
      redCtl->setFocus();
      return true;
    }
    else if (rc == correct) {
      editCtl->hide();
      editCtlVisible = false;
      redCtl->setFocus();
      if (escapePressed || !editCtl->isModified())
        return true;
      editCtl->setModified(false);
      str = editCtl->text();
      if (editToken == VarName_T) {
        varName = new VarNameV(str.toAscii());
        if (text->currentSynObj->primaryToken == VarName_T) {
          varName->varID = ((VarName*)text->currentSynObj)->varID;
          varName->flags = text->currentSynObj->flags;
        }
        if (text->currentSynObj->parentObject->parentObject->IsDeclare()) {
          dcl = (Declare*)text->currentSynObj->parentObject->parentObject;
          if (dcl->secondaryClause.ptr) {
            PutInsHint(varName,SET(firstHint,-1));
            PutIniCall(varName,true,false,true);
          }
          else
            PutInsHint(varName);
        }
        else
          PutInsHint(varName);
      }
      else {
        synObj = new ConstantV(str.toAscii());


//        B_Bool,Char,Integer,Float,Double,VLString,Any,ComponentObj,
//        Enumeration,B_Object,B_RefToObject,B_SetOfObject

        switch (token) {
        case IntConst_T:
          ((Constant*)synObj)->constType = Integer;
          break;
        case HexConst_T:
          ((Constant*)synObj)->constType = Integer;
          break;
        case OctalConst_T:
          ((Constant*)synObj)->constType = Integer;
          break;
        case BitConst_T:
          ((Constant*)synObj)->constType = Bitset;
          break;
        case DoubleConst_T:
          ((Constant*)synObj)->constType = Double;
          break;
        case FloatConst_T:
          ((Constant*)synObj)->constType = Float;
          break;
        case CharConst_T:
          ((Constant*)synObj)->constType = Char;
          break;
        case StringConst_T:
          ((Constant*)synObj)->constType = VLString;
          break;
        }
        PutInsHint(synObj);
      }
      return true;
    }
    else
      return false;
  }
  else
    return true;
}


SynObject *CExecView::FirstChild ()
{
  SynObject *currentSynObj=text->currentSynObj;
  ObjReference *objRef;
  CHETokenNode *nextTokenNode=currentSynObj->startToken;

  if (currentSynObj->primaryToken  == parameter_T)
    currentSynObj = (SynObject*)((Parameter*)currentSynObj)->parameter.ptr;
  else if (currentSynObj->primaryToken == ObjRef_T
  && ((ObjReference*)currentSynObj)->refIDs.first
        == ((ObjReference*)currentSynObj)->refIDs.last)
    return currentSynObj;

  for ( ; nextTokenNode; nextTokenNode = (CHETokenNode*)nextTokenNode->successor) {
    if (nextTokenNode->data.synObject != currentSynObj) {
      if (nextTokenNode->data.synObject->parentObject == currentSynObj
      || nextTokenNode->data.synObject->parentObject->primaryToken == parameter_T)
        return nextTokenNode->data.synObject;
      if (nextTokenNode->data.synObject->parentObject
      && (nextTokenNode->data.synObject->parentObject->primaryToken == quant_T
          || nextTokenNode->data.synObject->parentObject->primaryToken == FormParm_T
          || nextTokenNode->data.synObject->primaryToken == TDOD_T)
      && (nextTokenNode->data.synObject->parentObject->parentObject == currentSynObj
          || (nextTokenNode->data.synObject->parentObject->parentObject
              && nextTokenNode->data.synObject->parentObject->primaryToken == ObjRef_T
              && nextTokenNode->data.synObject->parentObject->parentObject->primaryToken == parameter_T
              && nextTokenNode->data.synObject->parentObject->parentObject->parentObject == currentSynObj))) {
        if (nextTokenNode->data.synObject->primaryToken == TDOD_T) {
          objRef = (ObjReference*)nextTokenNode->data.synObject->parentObject;
          if (((ObjReference*)objRef)->refIDs.first
              == ((ObjReference*)objRef)->refIDs.last)
            return nextTokenNode->data.synObject;
        }
        return nextTokenNode->data.synObject->parentObject;
      }
    }
  }
  return text->currentSynObj;
}

SynObject *CExecView::LeftSibling ()
{
  CHETokenNode *nextTokenNode=(CHETokenNode*)text->currentSynObj->startToken->predecessor;
  SynObject *currentSynObj=text->currentSynObj, *parent=currentSynObj->parentObject, *select;

  if (!parent)
    return text->currentSynObj;
  if (parent->primaryToken == ObjRef_T
  && ((ObjReference*)parent)->refIDs.first == ((ObjReference*)parent)->refIDs.last) {
    currentSynObj = parent;
    parent = currentSynObj->parentObject;
  }

  if (parent
  && parent->primaryToken == parameter_T) {
    currentSynObj = parent;
    parent = currentSynObj->parentObject;
  }

  for ( ; nextTokenNode; nextTokenNode = (CHETokenNode*)nextTokenNode->predecessor) {
    if (nextTokenNode->data.token == Comma_T
    && nextTokenNode->data.synObject->primaryToken == FormParms_T)
      nextTokenNode = (CHETokenNode*)nextTokenNode->predecessor;
    else if (nextTokenNode->data.token == Colon_T
    && nextTokenNode->data.synObject->primaryToken == FormParms_T
    && ((CHETokenNode*)nextTokenNode->predecessor)->data.token == outputs_T)
      nextTokenNode = (CHETokenNode*)nextTokenNode->predecessor->predecessor;

    if (nextTokenNode->data.synObject != currentSynObj
    && nextTokenNode->data.synObject->parentObject == parent)
      return nextTokenNode->data.synObject;
    select = AdjacentSynObj(currentSynObj,nextTokenNode);
    if (select)
      return select;
  }
  return text->currentSynObj;
}

SynObject *CExecView::RightSibling ()
{
  CHETokenNode *nextTokenNode=(CHETokenNode*)text->currentSynObj->endToken->successor;
  SynObject *currentSynObj=text->currentSynObj, *parent=currentSynObj->parentObject, *select;

  if (!parent)
    return text->currentSynObj;
  if (parent->primaryToken == ObjRef_T
  && ((ObjReference*)parent)->refIDs.first == ((ObjReference*)parent)->refIDs.last) {
    currentSynObj = parent;
    parent = currentSynObj->parentObject;
  }

  if (parent
  && parent->primaryToken == parameter_T) {
    currentSynObj = parent;
    parent = currentSynObj->parentObject;
  }

  for ( ; nextTokenNode; nextTokenNode = (CHETokenNode*)nextTokenNode->successor) {
    if (nextTokenNode->data.token == Comma_T
    && nextTokenNode->data.synObject->primaryToken == FormParms_T)
      nextTokenNode = (CHETokenNode*)nextTokenNode->successor;
    else if (nextTokenNode->data.token == inputs_T
    || nextTokenNode->data.token == outputs_T)
      nextTokenNode = (CHETokenNode*)nextTokenNode->successor->successor;

    if (nextTokenNode->data.synObject != currentSynObj
    && nextTokenNode->data.synObject->parentObject == parent)
      return nextTokenNode->data.synObject;
    select = AdjacentSynObj(currentSynObj,nextTokenNode);
    if (select)
      return select;
  }
  return text->currentSynObj;
}

SynObject *CExecView::AdjacentSynObj(SynObject *currentSynObj, CHETokenNode *nextTokenNode)
{
  // basically tests whether nextTokenNode->data.synObject differs from currentSynObj
  // but has the same parentObject as the latter, with certain exceptions

  if (nextTokenNode->data.synObject->parentObject
  && nextTokenNode->data.synObject->parentObject != currentSynObj
  && (nextTokenNode->data.synObject->parentObject->primaryToken == quant_T
      || nextTokenNode->data.synObject->primaryToken == TDOD_T)
  && nextTokenNode->data.synObject->parentObject->parentObject == currentSynObj->parentObject)
    return nextTokenNode->data.synObject->parentObject;
  else if (nextTokenNode->data.synObject->parentObject
  && nextTokenNode->data.synObject->parentObject->parentObject
  && nextTokenNode->data.synObject->parentObject->parentObject != currentSynObj
  && nextTokenNode->data.synObject->parentObject->parentObject->primaryToken == quant_T
  && nextTokenNode->data.synObject->parentObject->parentObject->parentObject == currentSynObj->parentObject)
    return nextTokenNode->data.synObject->parentObject->parentObject;
  else if (nextTokenNode->data.synObject->parentObject
  && nextTokenNode->data.synObject->parentObject != currentSynObj
  && nextTokenNode->data.synObject->parentObject->primaryToken == parameter_T
  && nextTokenNode->data.synObject->parentObject->parentObject == currentSynObj->parentObject)
    return nextTokenNode->data.synObject->parentObject;
  else if (nextTokenNode->data.token == TDOD_T
  && nextTokenNode->data.synObject->parentObject->parentObject->primaryToken == parameter_T
  && nextTokenNode->data.synObject->parentObject->parentObject->parentObject == currentSynObj->parentObject)
    return nextTokenNode->data.synObject;
  else if (currentSynObj->primaryToken == FormParm_T) {
    if (nextTokenNode->data.token == TDOD_T
    && nextTokenNode->data.synObject->parentObject->parentObject->primaryToken == FormParm_T)
      return nextTokenNode->data.synObject->parentObject->parentObject;
    else if (nextTokenNode->data.token == TypeRef_T
    && nextTokenNode->data.synObject->parentObject->primaryToken == FormParm_T)
      return nextTokenNode->data.synObject->parentObject;
  }
  else if (currentSynObj->primaryToken == FuncRef_T
  && currentSynObj->parentObject->type == implementation_T
  && nextTokenNode->data.synObject->parentObject
  && nextTokenNode->data.synObject->parentObject->primaryToken == FormParm_T)
    return nextTokenNode->data.synObject->parentObject->parentObject;
  return 0;
}

void CExecView::OnToggleArrows()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  text->leftArrows = !text->leftArrows;
  if (text->leftArrows)
    myDECL->TreeFlags.INCL(leftArrows);
  else
    myDECL->TreeFlags.EXCL(leftArrows);

  RedrawExec(text->selectAt);
}

void CExecView::OnNewLine()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  if (text->currentSynObj->flags.Contains(newLine))
    PutDelFlagHint(SET(newLine,-1));
  else
    PutInsFlagHint(SET(newLine,-1));
}

void CExecView::OnModulus()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  InsMultOp(Percent_T,new ModulusOpV());
}

void CExecView::OnIgnore()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;

  if (((  text->currentSynObj->primaryToken == TypePH_T
       || text->currentSynObj->primaryToken == TypeRef_T
       || text->currentSynObj->primaryToken == SetPH_T
       || text->currentSynObj->primaryToken == ObjRef_T)
      && text->currentSynObj->parentObject->primaryToken == quant_T)
    || (text->currentSynObj->parentObject
        && text->currentSynObj->parentObject->IsFuncInvocation()))
    text->currentSynObj = text->currentSynObj->parentObject;

  if (text->currentSynObj->flags.Contains(ignoreSynObj))
    PutDelFlagHint(SET(ignoreSynObj,-1));
  else
    PutInsFlagHint(SET(ignoreSynObj,-1));
}


void CExecView::OnInsertRef (QString &refName, TID &refID, bool isStaticCall, TID *vtypeID, bool fromNew)
{
  Reference *ref;
  ObjReference *objRef;
  FuncStatement *funcStm, *oldFuncStm;
  FuncExpression *funcExpr, *oldFuncExpr;
  Expression *newHdl;
  Run *runStm, *oldRunStm;
  NewExpression *newExp, *oldCreateExp;
  AttachObject *oldAttachExp;
  VarName *varNamePtr;
  CHE *chpParam, *chpFormParm, *newChe;
  TDODC handle;
  TDODV *tdod;
  LavaDECL *decl, *decl2;
  TID tid;
  ParameterV *param;
  bool first = true;
  QString str = "";

  decl = myDoc->IDTable.GetDECL(refID);
  switch (text->currentSynObj->primaryToken) {
  case FuncRef_T:
  case FuncPH_T:
  case nil_T:
    if (text->currentSynObj->parentObject->parentObject
    && text->currentSynObj->parentObject->parentObject->primaryToken == new_T
    && text->currentSynObj->parentObject->whereInParent
       == (address)&((NewExpression*)text->currentSynObj->parentObject->parentObject)->initializerCall.ptr) {
      text->currentSynObj = (SynObject*)((NewExpression*)text->currentSynObj->parentObject->parentObject)->objType.ptr;
      goto crtbl;
    }
    if (text->currentSynObj->parentObject->primaryToken == assignFX_T) {
      text->currentSynObj = text->currentSynObj->parentObject;
      oldFuncExpr = (FuncExpression*)text->currentSynObj;
      funcExpr = new FuncExpressionV(new ReferenceV(FuncPH_T,refID,refName.toAscii()));
      if (isStaticCall) {
        funcExpr->flags.INCL(staticCall);
        if (vtypeID)
          funcExpr->vtypeID = *vtypeID;
      }
      if (((FuncExpression*)text->currentSynObj)->handle.ptr) {
        newHdl = (Expression*)((FuncExpression*)text->currentSynObj)->handle.ptr->Clone();
        funcExpr->handle.ptr = newHdl;
        newHdl->MakeTable((address)&myDoc->IDTable,0,newHdl->parentObject,onConstrCopy,0,0);
        newHdl->MakeTable((address)&myDoc->IDTable,0,newHdl->parentObject,onCopy,0,0);
        newHdl->parentObject = funcExpr;
        newHdl->whereInParent = (address)&funcExpr->handle.ptr;
      }
      chpFormParm = (CHE*)decl->NestedDecls.first;
      chpParam = (CHE*)oldFuncExpr->inputs.first;
      while (chpParam && chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == IAttr) {
        param = (ParameterV*)chpParam->data->Clone();
        decl2 = (LavaDECL*)chpFormParm->data;
        param->formParmID = TID(decl2->OwnID,decl2->inINCL);
        param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onConstrCopy,0,0);
        param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onCopy,0,0);
        newChe = NewCHE(param);
        param->containingChain = (CHAINX*)&funcExpr->inputs;
        param->whereInParent = (address)newChe;
        funcExpr->inputs.Append(newChe);
        chpFormParm = (CHE*)chpFormParm->successor;
        chpParam = (CHE*)chpParam->successor;
      }
      funcExpr->replacedType = oldFuncExpr->replacedType;
      funcExpr->flags += oldFuncExpr->flags;
      if (!isStaticCall)
        funcExpr->flags.EXCL(staticCall);
      if (fromNew)
        PutInsHint(funcExpr,SET(lastHint,-1));
      else
        PutInsHint(funcExpr);
    }
    else if (text->currentSynObj->parentObject->primaryToken == assignFS_T) {
      text->currentSynObj = text->currentSynObj->parentObject;
      oldFuncStm = (FuncStatement*)text->currentSynObj;
      funcStm = new FuncStatementV(new ReferenceV(FuncPH_T,refID,refName.toAscii()));
      if (isStaticCall || ((FuncStatement*)text->currentSynObj)->flags.Contains(isIniCallOrHandle)) {
        funcStm->flags.INCL(staticCall);
        funcStm->varName = oldFuncStm->varName;
        if (vtypeID)
          funcStm->vtypeID = *vtypeID;
      }
      if (((FuncExpression*)text->currentSynObj)->handle.ptr) {
        newHdl = (Expression*)((FuncExpression*)text->currentSynObj)->handle.ptr->Clone();
        funcStm->handle.ptr = newHdl;
        newHdl->MakeTable((address)&myDoc->IDTable,0,newHdl->parentObject,onConstrCopy,0,0);
        newHdl->MakeTable((address)&myDoc->IDTable,0,newHdl->parentObject,onCopy,0,0);
        newHdl->parentObject = funcStm;
        newHdl->whereInParent = (address)&funcStm->handle.ptr;
      }
      chpFormParm = (CHE*)decl->NestedDecls.first;
      chpParam = (CHE*)oldFuncStm->inputs.first;
      while (chpParam && chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == IAttr) {
        param = (ParameterV*)chpParam->data->Clone();
        decl2 = (LavaDECL*)chpFormParm->data;
        param->formParmID = TID(decl2->OwnID,decl2->inINCL);
        param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onConstrCopy,0,0);
        param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onCopy,0,0);
        newChe = NewCHE(param);
        param->containingChain = (CHAINX*)&funcStm->outputs;
        param->whereInParent = (address)newChe;
        funcStm->inputs.Append(newChe);
        chpFormParm = (CHE*)chpFormParm->successor;
        chpParam = (CHE*)chpParam->successor;
      }
      chpParam = (CHE*)oldFuncStm->outputs.first;
      while (chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == IAttr)
        chpFormParm = (CHE*)chpFormParm->successor;
      while (chpParam && chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == OAttr) {
        param = (ParameterV*)chpParam->data->Clone();
        decl2 = (LavaDECL*)chpFormParm->data;
        param->formParmID = TID(decl2->OwnID,decl2->inINCL);
        param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onConstrCopy,0,0);
        param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onCopy,0,0);
        newChe = NewCHE(param);
        param->containingChain = (CHAINX*)&funcStm->outputs;
        param->whereInParent = (address)newChe;
        funcStm->outputs.Append(newChe);
        chpFormParm = (CHE*)chpFormParm->successor;
        chpParam = (CHE*)chpParam->successor;
      }
      funcStm->replacedType = oldFuncStm->replacedType;
      funcStm->flags += oldFuncStm->flags;
      if (!isStaticCall && !funcStm->flags.Contains(isIniCallOrHandle))
        funcStm->flags.EXCL(staticCall);
      if (fromNew)
        PutInsHint(funcStm,SET(lastHint,-1));
      else
        PutInsHint(funcStm);
    }
    else if (text->currentSynObj->parentObject->primaryToken == connect_T
    || text->currentSynObj->parentObject->primaryToken == disconnect_T
    || text->currentSynObj->parentObject->primaryToken == signal_T) {
      PutInsHint(new ReferenceV(FuncPH_T,refID,refName.toAscii()));
    }
    else if (text->currentSynObj->parentObject->primaryToken == disconnect_T) {
    }
    break;
  case CrtblPH_T:
  case Callee_T:
  case CompObj_T:
  case CrtblRef_T:
crtbl:
    text->currentSynObj = text->currentSynObj->parentObject;
    if (text->currentSynObj->primaryToken == run_T) {
      oldRunStm = (Run*)text->currentSynObj;
      switch (decl->DeclType) {
      case Initiator:
        runStm = new RunV(new ReferenceV(CrtblPH_T,refID,refName.toAscii()));
        chpFormParm = (CHE*)decl->NestedDecls.first;
        chpParam = (CHE*)oldRunStm->inputs.first;
        while (chpParam && chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == IAttr) {
          param = (ParameterV*)chpParam->data->Clone();
          decl2 = (LavaDECL*)chpFormParm->data;
          param->formParmID = TID(decl2->OwnID,decl2->inINCL);
          param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onConstrCopy,0,0);
          param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onCopy,0,0);
          newChe = NewCHE(param);
          param->containingChain = (CHAINX*)&runStm->inputs;
          param->whereInParent = (address)newChe;
          runStm->inputs.Append(newChe);
          chpFormParm = (CHE*)chpFormParm->successor;
          chpParam = (CHE*)chpParam->successor;
        }
        break;
      }
      runStm->replacedType = runStm->replacedType;
      runStm->flags += oldRunStm->flags;
      PutInsHint(runStm);
    }
    else if (text->currentSynObj->primaryToken == new_T) {
      oldCreateExp = (NewExpression*)text->currentSynObj;
      decl = myDoc->IDTable.GetDECL(refID);
      switch (decl->DeclType) {
      case CompObjSpec:
        ref = new ReferenceV(CrtblPH_T,refID,refName.toAscii());
        newExp = new NewExpressionV(
          ref,
          true,
          true);
        varNamePtr = (VarName*)newExp->varName.ptr;
        if (tempNo > 1) {
//                                      sprintf_s(buffer,"%u",tempNo);
          varNamePtr->varName += qPrintable(QString::number(tempNo));
//          varNamePtr->varName += _ultoa(tempNo,buffer,10);
                                }
        break;
      case Interface:
        newExp = new NewExpressionV(
          new ReferenceV(CrtblPH_T,refID,refName.toAscii()),
          false,
          false);
        varNamePtr = (VarName*)newExp->varName.ptr;
        if (tempNo > 1) {
//                                      sprintf_s(buffer,10,"%u",tempNo);
          varNamePtr->varName += qPrintable(QString::number(tempNo));
//          varNamePtr->varName += _ultoa(tempNo,buffer,10);
                                }
        break;
      case Function:
        newExp = (NewExpressionV*)text->currentSynObj;
        oldFuncStm = (FuncStatement*)newExp->initializerCall.ptr;
        ref = new ReferenceV(FuncPH_T,refID,refName.toAscii());
        funcStm = new FuncStatementV(ref);
        funcStm->flags.INCL(staticCall);
        funcStm->replacedType = CrtblPH_T;
        ref->parentObject = funcStm;
        if (oldFuncStm) {
          chpFormParm = (CHE*)decl->NestedDecls.first;
          chpParam = (CHE*)oldFuncStm->inputs.first;
          while (chpParam && chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == IAttr) {
            param = (ParameterV*)chpParam->data->Clone();
            decl2 = (LavaDECL*)chpFormParm->data;
            param->formParmID = TID(decl2->OwnID,decl2->inINCL);
            param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onConstrCopy,0,0);
            param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onCopy,0,0);
            newChe = NewCHE(param);
            param->containingChain = (CHAINX*)&funcStm->outputs;
            param->whereInParent = (address)newChe;
            funcStm->inputs.Append(newChe);
            chpFormParm = (CHE*)chpFormParm->successor;
            chpParam = (CHE*)chpParam->successor;
          }
          chpParam = (CHE*)oldFuncStm->outputs.first;
          while (chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == IAttr)
            chpFormParm = (CHE*)chpFormParm->successor;
          while (chpParam) {
            param = (ParameterV*)chpParam->data->Clone();
            if (chpFormParm && ((LavaDECL*)chpFormParm->data)->DeclType == OAttr) {
              decl = (LavaDECL*)chpFormParm->data;
              param->formParmID = TID(decl->OwnID,decl->inINCL);
              chpFormParm = (CHE*)chpFormParm->successor;
            }
            param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onConstrCopy,0,0);
            param->MakeTable((address)&myDoc->IDTable,0,param->parentObject,onCopy,0,0);
            newChe = NewCHE(param);
            param->containingChain = (CHAINX*)&funcStm->outputs;
            param->whereInParent = (address)newChe;
            funcStm->outputs.Append(newChe);
            chpParam = (CHE*)chpParam->successor;
          }
          funcStm->replacedType = oldFuncStm->replacedType;
          funcStm->flags += oldFuncStm->flags;
        }
        decl2 = decl->ParentDECL;
        tid.nID = decl2->OwnID;
        tid.nINCL = decl2->inINCL;
        str = decl2->LocalName.c;
        if (newExp->initializerCall.ptr) {
          text->currentSynObj = (SynObject*)newExp->objType.ptr;
          ref = new ReferenceV(CrtblPH_T,tid,str.toAscii());
          PutInsHint(ref,SET(firstHint,-1));
          funcStm->handle.ptr
            = ((FuncStatement*)newExp->initializerCall.ptr)->handle.ptr->Clone();
          text->currentSynObj = (SynObject*)newExp->initializerCall.ptr;
          text->selectAt = ref;
          PutInsHint(funcStm,SET(lastHint,-1));
          return;
        }
        else {
          Category targetCat = newExp->targetCat;
          newExp = new NewExpressionV(
            funcStm,
            false,
            false);
          funcStm->parentObject = newExp;
          varNamePtr = (VarName*)newExp->varName.ptr;
          if (tempNo > 1) {
            varNamePtr->varName += qPrintable(QString::number(tempNo));
                                        }
          newExp->objType.ptr = new ReferenceV(CrtblPH_T,tid,str.toAscii());
          if (targetCat == stateObjectCat)
            ((Reference*)newExp->objType.ptr)->flags.INCL(isStateObjectX);
          tdod = new TDODV(true);
          ((VarName*)newExp->varName.ptr)->MakeTable((address)&myDoc->IDTable,0,newExp,onNewID,(address)&newExp->varName.ptr,0);
          ((VarName*)newExp->varName.ptr)->varID.nINCL = -1;
            // to avoid a second assignment of an ID in execUpdate
          tdod->ID.nID = ((VarName*)newExp->varName.ptr)->varID.nID;
          newChe = new CHE(tdod);
          handle.Append(newChe);
          objRef = new ObjReferenceV(handle,varNamePtr->varName.c);
          objRef->parentObject = funcStm;
          objRef->whereInParent = (address)&funcStm->handle.ptr;
          tdod->parentObject = objRef;
          objRef->flags.INCL(isDisabled);
          objRef->flags.INCL(isTempVar);
          funcStm->handle.ptr = objRef;
        }
        break;
      }
      newExp->replacedType = oldCreateExp->replacedType;
      //newExp->flags += oldCreateExp->flags;
      PutInsHint(newExp);
    }
    else if (text->currentSynObj->primaryToken == attach_T) {
      oldAttachExp = (AttachObject*)text->currentSynObj;
      text->currentSynObj = (SynObject*)oldAttachExp->objType.ptr;
      ref = new ReferenceV(CrtblPH_T,refID,refName.toAscii());
      PutInsHint(ref);
    }
    break;
  default: // TypeRef
    ref = new ReferenceV(text->currentSynObj->type,refID,refName.toAscii());
    decl = myDoc->IDTable.GetDECL(refID);
    if (((Expression*)text->currentSynObj->parentObject)->targetCat == stateObjectCat)
      ref->flags.INCL(isStateObjectX);
    if (text->currentSynObj->parentObject->primaryToken != select_T) {
      PutInsHint(ref);
      return;
    }
    //durch select mit nicht-leerer extract-Klausel ersetzen???
  }
}

void CExecView::OnInsertObjRef (QString &refName, TDODC &refIDs, bool append)
{
  ObjReference *newRef, *oldRef;
  CHE *chp;
  CHAINX *chx;

  if (append) {
    chx = text->currentSynObj->containingChain;
    chp = (CHE*)text->currentSynObj->whereInParent;
    TDODC *refCopy = new TDODC(refIDs);
    oldRef = (ObjReference*)text->currentSynObj->parentObject;
    newRef = new ObjReferenceV;
    newRef->primaryToken = oldRef->primaryToken;
    newRef->type = oldRef->type;
    newRef->replacedType = oldRef->replacedType;
    CopyUntil(oldRef,chp,newRef);
    newRef->refName += qPrintable(refName);
    newRef->refIDs.last->successor = refCopy->first;
    refCopy->first->predecessor = newRef->refIDs.last;
    newRef->refIDs.last = refCopy->last;
    refCopy->first = 0;
    refCopy->last = 0;
    delete refCopy;
    text->currentSynObj = text->currentSynObj->parentObject;
  }
  else {
    newRef = new ObjReferenceV(refIDs,qPrintable(refName));
    if (text->currentSynObj->type == TDOD_T)
      text->currentSynObj = text->currentSynObj->parentObject;
    newRef->replacedType = text->currentSynObj->type;
  }
  newRef->parentObject = text->currentSynObj->parentObject;

  for (chp = (CHE*)newRef->refIDs.first; chp; chp = (CHE*)chp->successor)
    ((SynObject*)chp->data)->parentObject = newRef;

  SetRefTypeFlags(newRef);

  PutInsHint(newRef);
}

void CExecView::SetRefTypeFlags (ObjReference *newRef) {
  DWORD dw;
  TIDType idtype;
  LavaDECL *decl;

  dw = myDoc->IDTable.GetVar(((TDOD*)((CHE*)newRef->refIDs.first)->data)->ID,idtype);
  if (idtype == localID) {
    newRef->flags.INCL(isLocalVar);
    if (((SynObject*)dw)->parentObject
    && ((SynObject*)dw)->parentObject->parentObject
    && ((SynObject*)dw)->parentObject->parentObject->IsDeclare())
      newRef->flags.INCL(isDeclareVar);
    if (((SynObject*)dw)->type == implementation_T)
      newRef->flags.INCL(isSelfVar);
    else
      switch (((SynObject*)dw)->parentObject->primaryToken) {
      case new_T:
      case clone_T:
        newRef->flags.INCL(isTempVar);
        break;
      default: ;
      }
  }
  dw = myDoc->IDTable.GetVar(((TDOD*)((CHE*)newRef->refIDs.last)->data)->ID,idtype);
  if (idtype == globalID) {
    decl = *(LavaDECL**)dw;
    switch (decl->DeclType) {
    case Attr:
      newRef->flags.INCL(isMemberVar);
      if (((TDOD*)((CHE*)((CHE*)newRef->refIDs.last)->predecessor)->data)->IsStateObject(text->ckd))
        newRef->flags.INCL(isStateObjMember);
      break;
    case IAttr:
      newRef->flags.INCL(isInputVar);
      break;
    case OAttr:
      newRef->flags.INCL(isOutputVar);
      break;
    }
  }
}

void CExecView::OnAttach()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  AttachObject *newOp = new AttachObjectV(true);
  InsertOrReplace(newOp);
}

void CExecView::OnQueryItf()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  QueryItf *newOp = new QueryItfV(true);
  InsertOrReplace(newOp);
}

void CExecView::OnStaticCall()
{
  SynObject *synObj;

  if (!EditOK()) return;

  if (text->currentSynObj->IsStatement())
    synObj = new FuncStatementV(false,false,true);
  else
    synObj = new FuncExpressionV(false,true);
  InsertOrReplace(synObj);
}

void CExecView::OnAssign()
{
  if (!EditOK()) return;
  Assignment *assig = new AssignmentV(true);
  InsertOrReplace(assig);
}

void CExecView::OnConnect()
{
  QMenu connectMenu;
  QAction *triggeredAction;

  QAction *sdrObjAction = connectMenu.addAction("signal sender object");
  QAction *sdrClassAction = connectMenu.addAction("signal sender class");
  triggeredAction = connectMenu.exec(QCursor::pos());

  if (triggeredAction == sdrObjAction)
    InsertOrReplace(new ConnectV(false));
  else if (triggeredAction == sdrClassAction)
    InsertOrReplace(new ConnectV(true));
}

void CExecView::OnDisconnect()
{
  InsertOrReplace(new DisconnectV(true));
}

void CExecView::OnEmitSignal()
{
  SignalV *emitStm=new SignalV(true);
  ObjReferenceV *objRef;
  FuncStatementV *funcStm;
  TDODV *tdod;
  TDODC tdodc;
  CHE *newChe;

  tdod = new TDODV(true);
  tdod->name = STRING("self");
  tdod->ID = selfVar->varID;
  newChe = new CHE(tdod);
  tdodc.Append(newChe);
  objRef = new ObjReferenceV(tdodc,"self");
  objRef->flags.INCL(isDisabled);
  objRef->flags.INCL(isSelfVar);
  funcStm = new FuncStatementV;
  funcStm->parentObject = emitStm;
  emitStm->sCall.ptr = funcStm;
  funcStm->handle.ptr = objRef;
  tdod->parentObject = objRef;
  tdod->containingChain = (CHAINX*)&objRef->refIDs;
  objRef->parentObject = funcStm;
  objRef->whereInParent = (address)&emitStm->sCall.ptr;
  InsertOrReplace(emitStm);
}

void CExecView::OnTypeSwitch()
{
  if (!EditOK()) return;
  InsertOrReplace(new TypeSwitchStatementV(true));
}

void CExecView::OnInterval()
{
  SynObject *synObj, *newObj;

  if (!EditOK()) return;

  if (text->currentSynObj->primaryToken == TDOD_T)
    synObj = text->currentSynObj->parentObject;
  else
    synObj = text->currentSynObj;

  if ((synObj->parentObject->primaryToken == in_T
      && synObj->whereInParent == (address)&((InSetStatement*)synObj->parentObject)->operand2.ptr)
  || (synObj->parentObject->primaryToken == quant_T
      && synObj->whereInParent == (address)&((Quantifier*)synObj->parentObject)->set.ptr)) {
    newObj = new IntegerIntervalV(true);
    newObj->type = SetPH_T;
  }
  else {
    newObj = new ArrayAtIndexV(true);
    if (synObj->parentObject->primaryToken == assign_T
    && synObj->whereInParent
      == (address)&((Assignment*)synObj->parentObject)->targetObj.ptr)
      newObj->type = arrayAtIndex_T; // -> no expression
  }
  newObj->replacedType = synObj->replacedType;
  InsertOrReplace(newObj);
}
/*
void CExecView::OnUuid()
{
  if (!EditOK()) return;
  GetUUID *newOp = new GetUUIDV(true);
  InsertOrReplace(newOp);
}
*/
void CExecView::OnCall()
{
  if (!EditOK()) return;
  Run *newCall = new RunV(true);
  InsertOrReplace(newCall);
}

void CExecView::OnCreateObject()
{
  NewExpressionV *newExp;
  // TODO: Add your command handler code here

  if (!EditOK()) return;

  newExp = new NewExpressionV(true);
  InsertOrReplace(newExp);
}

void CExecView::OnAssert()
{
  if (!EditOK()) return;
  AssertStatement *assertStm = new AssertStatementV(true);
  InsertOrReplace(assertStm);
}

void CExecView::OnIgnoreStm()
{
  if (!EditOK()) return;
  IgnoreStatement *ignoreStm = new IgnoreStatementV(true);
  InsertOrReplace(ignoreStm);
}

void CExecView::OnNextError()
{
  CHETokenNode *currToken;

  if (!EditOK()) return;

  if (nErrors+nPlaceholders) {
    currToken = (CHETokenNode*)text->currentSelection;
    if (!((currToken->data.flags.Contains(primToken)
          && currToken->data.synObject->lastError)
         || text->currentSynObj->IsPlaceHolder())
    || myDoc->ErrorPageVisible())
      currToken = (CHETokenNode*)currToken->successor;
    else {
      nextError = true;
      Select(currToken->data.synObject);
      return;
    }

    for ( ; currToken; currToken = (CHETokenNode*)currToken->successor) {
      if ((currToken->data.flags.Contains(primToken)
          && currToken->data.synObject->lastError
          && currToken == currToken->data.synObject->primaryTokenNode) // because of multiple ops.
      || (currToken->data.synObject->IsPlaceHolder()
          && !currToken->data.flags.Contains(isDisabled))) {
        nextError = true;
        Select(currToken->data.synObject);
        //redCtl->update();
        return;
      }
    }
//    QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    for ( currToken = (CHETokenNode*)text->tokenChain.first;
          currToken;
          currToken = (CHETokenNode*)currToken->successor) {
      if ((currToken->data.flags.Contains(primToken)
          && currToken->data.synObject->lastError)
      || (currToken->data.synObject->IsPlaceHolder()
          && !currToken->data.flags.Contains(isDisabled))) {
        nextError = true;
        Select(currToken->data.synObject);
        //redCtl->update();
        return;
      }
    }
  }
  else
    QMessageBox::critical(this,qApp->applicationName(),ERR_NoErrors,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
}

void CExecView::OnPrevError()
{
  CHETokenNode *currToken;

  if (!EditOK()) return;

  if (nErrors+nPlaceholders) {
    currToken = (CHETokenNode*)text->currentSelection;
    if (!(currToken->data.flags.Contains(primToken)
          && currToken->data.synObject->lastError)
    || myDoc->ErrorPageVisible())
      currToken = (CHETokenNode*)currToken->predecessor;
    else {
      nextError = true;
      Select(currToken->data.synObject);
//                      redCtl->update();
      return;
    }

    for ( ; currToken; currToken = (CHETokenNode*)currToken->predecessor) {
      if ((currToken->data.flags.Contains(primToken)
          && currToken == currToken->data.synObject->primaryTokenNode // because of multiple ops.
          && currToken->data.synObject->lastError)
      || (currToken->data.synObject->IsPlaceHolder()
          && !currToken->data.flags.Contains(isDisabled))) {
        nextError = true;
        Select(currToken->data.synObject);
        //redCtl->update();
        return;
      }
    }
//    QMessageBox::critical(this, qApp->name(), "No (more) errors found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
    for ( currToken = (CHETokenNode*)text->tokenChain.last;
          currToken;
          currToken = (CHETokenNode*)currToken->predecessor) {
      if ((currToken->data.flags.Contains(primToken)
          && currToken->data.synObject->lastError)
      || (currToken->data.synObject->IsPlaceHolder()
          && !currToken->data.flags.Contains(isDisabled))) {
        nextError = true;
        Select(currToken->data.synObject);
        //redCtl->update();
        return;
      }
    }
  }
  else
    QMessageBox::critical(this,qApp->applicationName(),ERR_NoErrors,QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
}

void CExecView::OnNextComment()
{
  CHETokenNode *startToken = (CHETokenNode*)text->currentSelection, *currToken;

  if (!EditOK()) return;

  for (currToken = (CHETokenNode*)startToken->successor ; currToken; currToken = (CHETokenNode*)currToken->successor) {
    if (currToken->data.token == Comment_T) {
      text->newSelection = currToken;
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      text->Select();
      redCtl->update();
      return;
    }
  }
  for ( currToken = (CHETokenNode*)text->tokenChain.first;
        currToken != startToken;
        currToken = (CHETokenNode*)currToken->successor) {
    if (currToken->data.token == Comment_T) {
      text->newSelection = currToken;
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      text->Select();
      redCtl->update();
      return;
    }
  }
}

void CExecView::OnPrevComment()
{
  CHETokenNode *startToken = (CHETokenNode*)text->currentSelection, *currToken;

  if (!EditOK()) return;

  for (currToken = (CHETokenNode*)startToken->predecessor; currToken; currToken = (CHETokenNode*)currToken->predecessor) {
    if (currToken->data.token == Comment_T) {
      text->newSelection = currToken;
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      text->Select();
      redCtl->update();
      return;
    }
  }
//  QMessageBox::critical(this, qApp->name(), "No (more) comments found", QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
  for (currToken = (CHETokenNode*)text->tokenChain.last;
       currToken != startToken;
       currToken = (CHETokenNode*)currToken->predecessor) {
    if (currToken->data.token == Comment_T) {
      text->newSelection = currToken;
      ((CExecFrame*)GetParentFrame())->m_ComboBar->ShowCombos(disableCombo);
      text->Select();
      redCtl->update();
      return;
    }
  }
}

void CExecView::OnConflict()
{
  if (!EditOK()) return;
  Base->Browser->LastBrowseContext = new CBrowseContext(this,text->currentSynObj);
  Select((TDOD*)((CHE*)((ObjReference*)text->currentSynObj->parentObject)->conflictingAssig->refIDs.last)->data);
}

bool CExecView::ConflictSelected()
{
  CHE *che;

  if (text->currentSynObj->primaryToken != TDOD_T)
    return false;

  che = (CHE*)text->currentSynObj->whereInParent;
  if (che->successor || !text->currentSynObj->lastError)
    return false;

  for (che = (CHE*)text->currentSynObj->errorChain.first;
       che
       && ((CLavaError*)che->data)->IDS != &ERR_SingleAssViol
       &&((CLavaError*)che->data)->IDS != &ERR_PrevDescAssig
       &&((CLavaError*)che->data)->IDS != &ERR_PrevAncestAssig;
       che = (CHE*)che->successor);
  if (che)
    return true;
  else
    return false;
}

void CExecView::OnToggleCategory()
{
  if (!EditOK()) return;

  if (text->currentSynObj->primaryToken == VarName_T) {
    if (text->currentSynObj->flags.Contains(isStateObjectX)) {
      PutDelFlagHint(SET(isStateObjectX,-1),SET(firstHint,-1));
      PutInsFlagHint(SET(isAnyCatX,-1),SET(lastHint,-1));
    }
    else if (text->currentSynObj->flags.Contains(isAnyCatX))
      PutDelFlagHint(SET(isAnyCatX,-1),SET(firstHint,lastHint,-1));
    else
      PutInsFlagHint(SET(isStateObjectX,-1),SET(firstHint,lastHint,-1));
  }
  else {
    if (text->currentSynObj->flags.Contains(isStateObjectX))
      PutDelFlagHint(SET(isStateObjectX,-1),SET(firstHint,lastHint,-1));
    else
      PutInsFlagHint(SET(isStateObjectX,-1),SET(firstHint,lastHint,-1));
  }
}

bool CExecView::ToggleCatEnabled()
{
  //LavaDECL *decl;
  //NewExpression *newExp;
  //bool cat;
  SynFlags ctxFlags;

  if (Ignorable())
    return false;

  switch (text->currentSynObj->primaryToken) {
  case new_T:
  case attach_T:
    return true;
    break;

  case VarName_T:
    if (!((Quantifier*)text->currentSynObj->parentObject)->set.ptr)
      return true;
    break;

  //case TypeRef_T:
  //  return true;
  //  break;

  //case CrtblRef_T:
  //  if (text->currentSynObj->parentObject->primaryToken != new_T)
  //    return false;
  //  decl = ((Reference*)text->currentSynObj)->refDecl;
  //  if (decl->SecondTFlags.Contains(isSet))
  //    return true;
  //  else
  //    return false;
  //  break;

  default:
    if (text->currentSynObj->IsConstant() && text->currentSynObj->primaryToken != nil_T)
      return true;
  }
  return false;
}

void CExecView::OnToggleSubstitutable()
{
  if (!EditOK()) return;

  if (text->currentSynObj->flags.Contains(isSubstitutable)) {
    PutDelFlagHint(SET(isSubstitutable,-1));
  }
  else
    PutInsFlagHint(SET(isSubstitutable,-1));
}

bool CExecView::ToggleSubstitutableEnabled()
{
  LavaDECL *decl;
  CContext ctx;

  if (Ignorable()
  || text->currentSynObj->primaryToken != TypeRef_T
  || text->currentSynObj->parentObject->primaryToken == caseType_T
  || text->currentSynObj->parentObject->primaryToken == attach_T
  || text->currentSynObj->parentObject->primaryToken == new_T
  || text->currentSynObj->parentObject->primaryToken == itf_T
  || text->currentSynObj->parentObject->primaryToken == qua_T)
    return false;

  decl = myDoc->IDTable.GetDECL(((Reference*)text->currentSynObj)->refID,text->ckd.inINCL);
  if (myDoc->IDTable.otherOContext(decl, myDECL))
    return true;
  else
    return false;
}

void CExecView::OnToggleClosed()
{
  if (!EditOK()) return;

  if (((SynObject*)text->currentSynObj)->flags.Contains(isClosed)) {
    PutDelFlagHint(SET(isClosed,-1));
    ((Expression*)text->currentSynObj)->closedLevel = INT_MAX;
  }
  else {
    PutInsFlagHint(SET(isClosed,-1));
    ((Expression*)text->currentSynObj)->closedLevel = 0;
  }
}

void CExecView::OnToggleParmNames()
{
  if (!EditOK()) return;

  text->parmNames = !text->parmNames;
  if (text->parmNames)
    myDECL->TreeFlags.INCL(parmNames);
  else
    myDECL->TreeFlags.EXCL(parmNames);

  RedrawExec(text->selectAt);

}

void CExecView::OnCopy()
{
  CopyStatementV *cpy;

  if (!EditOK()) return;

  cpy = new CopyStatementV(true);
  InsertOrReplace(cpy);
}

void CExecView::OnEvaluate()
{
  SynObject *synObj;

  if (!EditOK()) return;

  if (text->currentSynObj->StatementSelected(text->currentSelection))
    synObj = new EvalStatementV(true);
  else
    synObj = new EvalExpressionV(true);
  InsertOrReplace(synObj);

}

void CExecView::OnInputArrow()
{
  if (!EditOK()) return;
  if (text->currentSynObj->type == FuncPH_T || text->currentSynObj->type == CrtblPH_T)
    text->currentSynObj = text->currentSynObj->parentObject;

  if (text->currentSynObj->flags.Contains(inputArrow))
    PutDelFlagHint(SET(inputArrow,-1));
  else
    PutInsFlagHint(SET(inputArrow,-1));
}

bool CExecView::event(QEvent *ev) {
  if (ev->type() == UEV_ShowMiniEdit) {
    editCtl->show();
    qDebug() << "show MiniEdit-Event";
    editCtl->setFocus();
    if (text->currentSynObj->IsPlaceHolder())
      editCtl->selectAll();
    editCtlVisible = true;
    return true;
  }
  else
    return CLavaBaseView::event(ev);
}

void CExecView::focusInEvent(QFocusEvent *ev)
{
//!!!  CTextEdit::focusInEvent(ev);

  // TODO: Add your message handler code here
  focusWindow = (void*)this;
  isExecView = true;
  redCtl->setFocus();
  wxView::focusInEvent(ev);
}

void CExecView::focusOutEvent(QFocusEvent *ev)
{
  // TODO: Add your message handler code here
  focusWindow = 0;
}

void CExecView::OnFunctionCall()
{
  // TODO: Add your command handler code here
  SynObject *synObj;

  if (!EditOK()) return;

  if (text->currentSynObj->StatementSelected(text->currentSelection))
    synObj = new FuncStatementV(true);
  else
    synObj = new FuncExpressionV(true);
  InsertOrReplace(synObj);
}

void CExecView::OnHandle()
{
  // TODO: Add your command handler code here

  if (!EditOK()) return;
  HandleOp *handleOp = new HandleOpV(true);
  InsertOrReplace(handleOp);
}

void CExecView::OnOptLocalVar()
{
        // TODO: Add your command handler code here
  if (!EditOK()) return;

  if (text->currentSynObj->flags.Contains(isOptionalExpr)) {
    PutDelFlagHint(SET(isOptionalExpr,-1));
  }
  else
    PutInsFlagHint(SET(isOptionalExpr,-1));

}

void CExecView::OnItem()
{
        // TODO: Add your command handler code here

  if (!EditOK()) return;
  EnumItem *item = new EnumItemV(true);
  InsertOrReplace(item);
}

void CExecView::OnQua()
{
        // TODO: Add your command handler code here

  if (!EditOK()) return;
  ExtendExpression *qua = new ExtendExpressionV(true);
  InsertOrReplace(qua);
}

void CExecView::OnSucceed()
{
        // TODO: Add your command handler code here

  if (!EditOK()) return;
  SucceedStatement *succStm = new SucceedStatementV();
  InsertOrReplace(succStm);
}

void CExecView::OnFail()
{
        // TODO: Add your command handler code here

  if (!EditOK()) return;
  FailStatement *fStm = new FailStatementV();
  InsertOrReplace(fStm);
}

void CExecView::OnOld()
{
        // TODO: Add your command handler code here

  if (!EditOK()) return;
  OldExpression *oldExpr = new OldExpressionV();
  InsertOrReplace(oldExpr);
}

void CExecView::OnOrd()
{
        // TODO: Add your command handler code here

  if (!EditOK()) return;
  OrdOp *ordOp = new OrdOpV(true);
  InsertOrReplace(ordOp);
}

void CExecView::OnTryStatement()
{
        // TODO: Add your command handler code here

  if (!EditOK()) return;
  TryStatement *tryStm = new TryStatementV(true);
  InsertOrReplace(tryStm);
}

void CExecView::SetHelpText () {
  QString helpMsg;
  SynObject *ocl;
  CHAINX *chxp;

  if (text->currentSynObj->primaryToken == Exp_T
  || text->currentSynObj->IsEditableConstant())
    helpMsg = ID_ENTER_CONST;
  else if (text->currentSelection->data.token == Comment_T)
    helpMsg = ID_EDIT_COMMENT;
  else if (text->currentSelection->data.OptionalClauseToken(ocl)
  || (text->currentSynObj->IsRepeatableClause(chxp)
      && !text->currentSynObj->IsIfClause()))
    helpMsg = ID_OPT_CLAUSE;
  else if (!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection)
  && !text->currentSynObj->IsPlaceHolder())
    helpMsg = ID_INSERT_STM;
  else if (ToggleCatEnabled())
    helpMsg = ID_TOGGLE_CATEGORY;
  else if (!Ignorable() && text->currentSynObj->ExpressionSelected(text->currentSelection)
  && !text->currentSynObj->IsPlaceHolder())
    helpMsg = ID_REPLACE_EXP;
  else if (text->currentSynObj->type == VarPH_T)
    helpMsg = ID_ENTER_VARNAME;
  else
    helpMsg = ID_F1_HELP;

  if (!inIgnore)
    UpdateErrMsg(helpMsg);
  else
    statusBar->showMessage(helpMsg);
}

void CExecView::OnInsertEnum (QString &itemName, TID &typeID)
{
  EnumConst *item;

  item = new EnumConstV(text->currentSynObj->type,typeID,itemName);
  PutInsHint(item);
}


///////////////////////////////////////////////////////////////////////////////
// UpdateUI functions:

void CExecView::UpdateUI()
{
  if (editCtlVisible || !initialUpdateDone)
    return;
  OnUpdateOptLocalVar(LBaseData->optLocalVarActionPtr);
  OnUpdateHandle(LBaseData->handleActionPtr);
  OnUpdateInputArrow(LBaseData->toggleInputArrowsActionPtr);
  OnUpdateEditSel(LBaseData->editSelItemActionPtr);
  OnUpdateEvaluate(LBaseData->evaluateActionPtr);
  OnUpdateToggleSubstitutable(LBaseData->toggleSubstTypeActionPtr);
  OnUpdateToggleClosed(LBaseData->toggleClosedActionPtr);
  OnUpdateToggleParmNames(LBaseData->parmNameActionPtr);
  OnUpdateConflict(LBaseData->conflictingAssigActionPtr);
  OnUpdateToggleCategory(LBaseData->toggleCategoryActionPtr);
  OnUpdateNextComment(LBaseData->nextCommentActionPtr);
  OnUpdatePrevComment(LBaseData->prevCommentActionPtr);
  OnUpdateNextError(LBaseData->nextErrorActionPtr);
  OnUpdatePrevError(LBaseData->prevErrorActionPtr);
//  OnUpdateUuid(LBaseData->action);
  OnUpdateInterval(LBaseData->intervalActionPtr);
  OnUpdateIgnore(LBaseData->commentOutActionPtr);
  OnUpdateModulus(LBaseData->modulusActionPtr);
  OnUpdateNewLine(LBaseData->newLineActionPtr);
  OnUpdateShowOptionals(LBaseData->showOptsActionPtr);
  OnUpdateGotoDecl(LBaseData->gotoDeclActionPtr);
  OnUpdateGotoImpl(LBaseData->gotoImplActionPtr);
  OnUpdateFindReferences(LBaseData->findRefsActionPtr);
  OnUpdateToggleArrows(LBaseData->toggleArrowsActionPtr);
  OnUpdateComment(LBaseData->editCommentActionPtr);
  OnUpdateButtonEnum();
  OnUpdateNewFunc();
  OnUpdateNewPFunc();
  OnUpdateEditCut(LBaseData->editCutActionPtr);
  OnUpdateEditCopy(LBaseData->editCopyActionPtr);
  OnUpdateEditPaste(LBaseData->editPasteActionPtr);
  OnUpdateEditUndo(LBaseData->editUndoActionPtr);
  OnUpdateEditRedo(LBaseData->editRedoActionPtr);
  OnUpdateBitOr(LBaseData->bitOrActionPtr);
  OnUpdateBitAnd(LBaseData->bitAndActionPtr);
  OnUpdateBitXor(LBaseData->bitXorActionPtr);
  OnUpdateDelete(LBaseData->deleteActionPtr);
  OnUpdateDivide(LBaseData->divideActionPtr);
  OnUpdateEq(LBaseData->equalActionPtr);
  OnUpdateNe(LBaseData->notEqualActionPtr);
  OnUpdateLe(LBaseData->lessEqualActionPtr);
  OnUpdateLt(LBaseData->lessThanActionPtr);
  OnUpdateGe(LBaseData->greaterEqualActionPtr);
  OnUpdateGt(LBaseData->greaterThanActionPtr);
  OnUpdateTrue(LBaseData->trueActionPtr);
  OnUpdateFalse(LBaseData->falseActionPtr);
  OnUpdateFunctionCall(LBaseData->functionCallActionPtr);
  OnUpdateStaticCall(LBaseData->staticCallActionPtr);
  OnUpdateInsert(LBaseData->insertActionPtr);
  OnUpdateInsertBefore(LBaseData->insertBeforeActionPtr);
  OnUpdateInvert(LBaseData->invertActionPtr);
  OnUpdateLshift(LBaseData->leftShiftActionPtr);
  OnUpdateRshift(LBaseData->rightShiftActionPtr);
  OnUpdatePlusMinus(LBaseData->toggleSignActionPtr);
  OnUpdateMult(LBaseData->multiplicationActionPtr);
  OnUpdateShowComments(LBaseData->toggleCommentsActionPtr);
  OnUpdateNull(LBaseData->nullActionPtr);
  OnUpdatePlus(LBaseData->additionActionPtr);
  OnUpdateOrd(LBaseData->ordActionPtr);

  OnUpdateDbgStart(LBaseData->DbgActionPtr);
  OnUpdateDbgStepNext(LBaseData->DbgStepNextActPtr);
  OnUpdateDbgStepNextFunction(LBaseData->DbgStepNextFunctionActPtr);
  OnUpdateDbgStepinto(LBaseData->DbgStepintoActPtr);
  OnUpdateDbgStepout(LBaseData->DbgStepoutActPtr);
  OnUpdateDbgRunToSel(LBaseData->DbgRunToSelActPtr);
  OnUpdateDbgBreakpoint(LBaseData->DbgBreakpointActPtr);
  OnUpdateDbgClearBreakpoints(LBaseData->DbgClearBreakpointsActPtr);
  OnUpdateDbgStop(LBaseData->DbgStopActionPtr);

  OnUpdateDeclare(LBaseData->declareButton);
  OnUpdateExists(LBaseData->existsButton);
  OnUpdateForeach(LBaseData->foreachButton);
  OnUpdateSelect(LBaseData->selectButton);
  OnUpdateIn(LBaseData->elInSetButton);
  OnUpdateIf(LBaseData->ifButton);
  OnUpdateIfdef(LBaseData->ifdefButton);
  OnUpdateIfExpr(LBaseData->ifxButton);
  OnUpdateElseExpr(LBaseData->elsexButton);
  OnUpdateSwitch(LBaseData->switchButton);
  OnUpdateTypeSwitch(LBaseData->typeSwitchButton);
  OnUpdateAnd(LBaseData->andButton);
  OnUpdateOr(LBaseData->orButton);
  OnUpdateXor(LBaseData->xorButton);
  OnUpdateNot(LBaseData->notButton);
  OnUpdateAssert(LBaseData->assertButton);
  OnUpdateIgnoreButton(LBaseData->ignoreButton);
  OnUpdateTry(LBaseData->tryButton);
  OnUpdateSucceed(LBaseData->succeedButton);
  OnUpdateFail(LBaseData->failButton);
  OnUpdateCall(LBaseData->runButton);
  OnUpdateAssign(LBaseData->setButton);
  OnUpdateCreate(LBaseData->newButton);
  OnUpdateOld(LBaseData->oldButton);
  OnUpdateClone(LBaseData->cloneButton);
  OnUpdateCopy(LBaseData->copyButton);
  OnUpdateQua(LBaseData->scaleButton);
  OnUpdateConnect(LBaseData->connectButton);
  OnUpdateDisconnect(LBaseData->disconnectButton);
  OnUpdateEmitSignal(LBaseData->emitButton);
  OnUpdateItem(LBaseData->itemButton);
  OnUpdateAttach(LBaseData->attachButton);
  OnUpdateQueryItf(LBaseData->qryItfButton);
}

void CExecView::DisableActions()
{

  if (!myDoc || !myDoc->mySynDef)
    return;

  //CLavaMainFrame* frame = (CLavaMainFrame*)wxTheApp->m_appWindow;

  //LBaseData->editPasteActionPtr->setEnabled(false);
  LBaseData->optLocalVarActionPtr->setEnabled(false);
  LBaseData->handleActionPtr->setEnabled(false);
  LBaseData->toggleInputArrowsActionPtr->setEnabled(false);
  LBaseData->editSelItemActionPtr->setEnabled(false);
  LBaseData->evaluateActionPtr->setEnabled(false);
  LBaseData->toggleSubstTypeActionPtr->setEnabled(false);
  LBaseData->toggleClosedActionPtr->setEnabled(false);
  LBaseData->parmNameActionPtr->setEnabled(false);
  LBaseData->conflictingAssigActionPtr->setEnabled(false);
  LBaseData->toggleCategoryActionPtr->setEnabled(false);
  LBaseData->intervalActionPtr->setEnabled(false);
  LBaseData->commentOutActionPtr->setEnabled(false);
  LBaseData->modulusActionPtr->setEnabled(false);
  LBaseData->newLineActionPtr->setEnabled(false);
  LBaseData->showOptsActionPtr->setEnabled(false);
  LBaseData->gotoDeclActionPtr->setEnabled(false);
  LBaseData->gotoImplActionPtr->setEnabled(false);
  LBaseData->findRefsActionPtr->setEnabled(false);
  LBaseData->toggleArrowsActionPtr->setEnabled(false);
  LBaseData->editCommentActionPtr->setEnabled(false);
  LBaseData->editCutActionPtr->setEnabled(false);
  LBaseData->editCopyActionPtr->setEnabled(false);
  LBaseData->editPasteActionPtr->setEnabled(false);
  LBaseData->editUndoActionPtr->setEnabled(false);
  LBaseData->editRedoActionPtr->setEnabled(false);
  LBaseData->bitOrActionPtr->setEnabled(false);
  LBaseData->bitAndActionPtr->setEnabled(false);
  LBaseData->bitXorActionPtr->setEnabled(false);
  LBaseData->deleteActionPtr->setEnabled(false);
  LBaseData->divideActionPtr->setEnabled(false);
  LBaseData->equalActionPtr->setEnabled(false);
  LBaseData->notEqualActionPtr->setEnabled(false);
  LBaseData->lessEqualActionPtr->setEnabled(false);
  LBaseData->lessThanActionPtr->setEnabled(false);
  LBaseData->greaterEqualActionPtr->setEnabled(false);
  LBaseData->greaterThanActionPtr->setEnabled(false);
  LBaseData->trueActionPtr->setEnabled(false);
  LBaseData->falseActionPtr->setEnabled(false);
  LBaseData->functionCallActionPtr->setEnabled(false);
  LBaseData->staticCallActionPtr->setEnabled(false);
  LBaseData->insertActionPtr->setEnabled(false);
  LBaseData->insertBeforeActionPtr->setEnabled(false);
  LBaseData->invertActionPtr->setEnabled(false);
  LBaseData->leftShiftActionPtr->setEnabled(false);
  LBaseData->rightShiftActionPtr->setEnabled(false);
  LBaseData->toggleSignActionPtr->setEnabled(false);
  LBaseData->multiplicationActionPtr->setEnabled(false);
  LBaseData->toggleCommentsActionPtr->setEnabled(false);
  LBaseData->nullActionPtr->setEnabled(false);
  LBaseData->additionActionPtr->setEnabled(false);
        LBaseData->ordActionPtr->setEnabled(false);

  LBaseData->DbgActionPtr->setEnabled(false);
  LBaseData->DbgStepNextActPtr->setEnabled(false);
  LBaseData->DbgStepNextFunctionActPtr->setEnabled(false);
  LBaseData->DbgStepintoActPtr->setEnabled(false);
  LBaseData->DbgStepoutActPtr->setEnabled(false);
  LBaseData->DbgRunToSelActPtr->setEnabled(false);
        LBaseData->DbgBreakpointActPtr->setEnabled(false);
  LBaseData->DbgClearBreakpointsActPtr->setEnabled(false);
  LBaseData->DbgStopActionPtr->setEnabled(false);

  CExecView::DisableKwdButtons();
}

void CExecView::DisableKwdButtons() {
  LBaseData->declareButton->setEnabled(false);
  LBaseData->existsButton->setEnabled(false);
  LBaseData->foreachButton->setEnabled(false);
  LBaseData->selectButton->setEnabled(false);
  LBaseData->elInSetButton->setEnabled(false);
  LBaseData->ifButton->setEnabled(false);
  LBaseData->ifdefButton->setEnabled(false);
  LBaseData->ifxButton->setEnabled(false);
  LBaseData->elsexButton->setEnabled(false);
  LBaseData->switchButton->setEnabled(false);
  LBaseData->typeSwitchButton->setEnabled(false);
  LBaseData->andButton->setEnabled(false);
  LBaseData->orButton->setEnabled(false);
  LBaseData->xorButton->setEnabled(false);
  LBaseData->notButton->setEnabled(false);
  LBaseData->assertButton->setEnabled(false);
  LBaseData->ignoreButton->setEnabled(false);
  LBaseData->tryButton->setEnabled(false);
  LBaseData->succeedButton->setEnabled(false);
  LBaseData->failButton->setEnabled(false);
  LBaseData->runButton->setEnabled(false);
  LBaseData->setButton->setEnabled(false);
  LBaseData->newButton->setEnabled(false);
  LBaseData->oldButton->setEnabled(false);
  LBaseData->cloneButton->setEnabled(false);
  LBaseData->copyButton->setEnabled(false);
  LBaseData->scaleButton->setEnabled(false);
  LBaseData->connectButton->setEnabled(false);
  LBaseData->disconnectButton->setEnabled(false);
  LBaseData->emitButton->setEnabled(false);
  LBaseData->itemButton->setEnabled(false);
  LBaseData->attachButton->setEnabled(false);
  LBaseData->qryItfButton->setEnabled(false);
}


void CExecView::OnUpdateGotoImpl(QAction* action)
{
  switch (text->currentSelection->data.token) {
  case FuncRef_T:
  case TypeRef_T:
    action->setEnabled(true);
    return;
  case CrtblRef_T:
    if (text->currentSynObj->parentObject->primaryToken == new_T
    || text->currentSynObj->parentObject->primaryToken == run_T)
      action->setEnabled(true);
    return;
  default: ;
  }
  action->setEnabled(false);
}

void CExecView::OnUpdateModulus(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateFindReferences(QAction* action)
{
  switch (text->currentSelection->data.token) {
  case VarName_T:
  case TDOD_T:
  case FuncRef_T:
  case TypeRef_T:
  case CrtblRef_T:
  case enumConst_T:
    action->setEnabled(true);
    return;
  default: ;
  }
  action->setEnabled(false);
}

void CExecView::OnUpdateGotoDecl(QAction* action)
{
  action->setEnabled(EnableGotoDecl());
}

void CExecView::OnUpdateEditSel(QAction* action)
{
  // TODO: Add your message handler code here and/or call default


  action->setEnabled(
    (text->currentSynObj->primaryToken == Exp_T
        && !text->currentSynObj->BoolAdmissibleOnly(text->ckd)
        && !text->currentSynObj->EnumAdmissibleOnly(text->ckd))
    || (text->currentSynObj->IsConstant()
        && text->currentSynObj->primaryToken != enumConst_T
        && text->currentSynObj->primaryToken != Boolean_T)
    || text->currentSynObj->type == VarPH_T
    || text->currentSelection->data.token == Comment_T);
}

bool CExecView::EnableCut()
{
  if (GetDocument()->changeNothing)
    return false;

  if ((Ignorable() && !inIgnore)
  && !(inBaseInits
       && text->currentSynObj->primaryToken == FuncRef_T
       && text->currentSynObj->parentObject->parentObject->primaryToken == initializing_T))
    return false;

  if (text->currentSynObj->parentObject
  && !text->currentSynObj->parentObject->parentObject
  && text->currentSynObj->IsPlaceHolder())
    return false;

  if (text->currentSynObj->primaryToken == VarName_T
  && (text->currentSynObj->parentObject->primaryToken == new_T
      || text->currentSynObj->parentObject->primaryToken == run_T))
    return false;

  if (text->currentSynObj->IsFuncInvocation()
  && text->currentSynObj->flags.Contains(isIniCallOrHandle)
  && text->currentSynObj->parentObject->primaryToken != new_T)
    return false;

  if (text->currentSynObj->primaryToken == TDOD_T
  && text->currentSynObj->parentObject->parentObject->IsFuncInvocation()) {
    if (text->currentSynObj->parentObject->parentObject->parentObject->primaryToken == new_T)
      return (text->currentSynObj->parentObject->parentObject->whereInParent
        != (address)&((NewExpression*)text->currentSynObj->parentObject->parentObject->parentObject)->initializerCall.ptr);
    else if (text->currentSynObj->parentObject->flags.Contains(isSelfVar)
    && text->currentSynObj->parentObject->parentObject->primaryToken == signal_T)
      return false;
  }

  if (text->currentSynObj->primaryToken == parameter_T)
    return false;

  return (IsDeletablePrimary()
    || (text->currentSynObj->IsFuncInvocation()
        && text->currentSynObj->parentObject->primaryToken != new_T)
    || (text->currentSynObj->IsPlaceHolder()
        && text->currentSynObj->parentObject->primaryToken != parameter_T));
}

void CExecView::OnUpdateEditCut(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(EnableCut());
}

void CExecView::OnUpdateButtonEnum()
{
  CComboBar* bar = ((CExecFrame*)GetParentFrame())->m_ComboBar;
  bar->IDC_ButtonEnum->setEnabled(bar->EnumsEnable);
  if (bar->EnumsShow)
    bar->IDC_ButtonEnum->show();
  else
    bar->IDC_ButtonEnum->hide();
}

void CExecView::OnUpdateNewFunc()
{
  CComboBar* bar = ((CExecFrame*)GetParentFrame())->m_ComboBar;
  bar->IDC_NewFunc->setEnabled(bar->NewFuncEnable);
  if (bar->NewFuncShow)
    bar->IDC_NewFunc->show();
  else
    bar->IDC_NewFunc->hide();
}

void CExecView::OnUpdateNewPFunc()
{
  CComboBar* bar = ((CExecFrame*)GetParentFrame())->m_ComboBar;
  bar->IDC_NewPFunc->setEnabled(bar->NewPFuncEnable);
  if (bar->NewFuncShow)
    bar->IDC_NewPFunc->show();
  else
    bar->IDC_NewPFunc->hide();
}

void CExecView::OnUpdateAnd(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateBitAnd(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateBitOr(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateBitXor(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateClone(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateSelect(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateComment(QAction* action)
{
  // TODO: Add your command update UI handler code here

  if (Taboo())
    action->setEnabled(false);
  else
    action->setEnabled(!editCtlVisible && IsTopLevelToken());
}

void CExecView::OnUpdateDelete(QAction* action)
{
  // TODO: Add your command update UI handler code here

  OnUpdateEditCut(action);
}

void CExecView::OnUpdateDivide(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateOptLocalVar(QAction* action)
{
        // TODO: Add your command update UI handler code here
        bool isDeclareVar;

  if (text->currentSynObj->primaryToken == VarName_T
  && text->currentSynObj->parentObject
  && text->currentSynObj->parentObject->parentObject
  && text->currentSynObj->parentObject->parentObject->IsDeclare())
    isDeclareVar = true;
  else
    isDeclareVar = false;
  action->setEnabled(!Taboo() && isDeclareVar);

  action->setChecked(text->currentSynObj->flags.Contains(isOptionalExpr));
//  if (action->m_pOther)
//    ((QToolBar*)action->m_pOther)->GetToolBarCtrl().PressButton(action->m_nID,showArrow);
}

void CExecView::OnUpdateHandle(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateFunctionCall(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && (text->currentSynObj->StatementSelected(text->currentSelection)
    || (text->currentSynObj->ExpressionSelected(text->currentSelection))));
}

void CExecView::OnUpdateInputArrow(QAction* action)
{
  bool showArrow;
  SynObject *synObj=text->currentSynObj;

  if (synObj->type == FuncPH_T || (synObj->type == CrtblPH_T && synObj->parentObject->primaryToken == run_T))
    synObj = synObj->parentObject;

  showArrow = (synObj->IsFuncInvocation() || synObj->primaryToken == run_T)
    && synObj->flags.Contains(inputArrow);
  action->setEnabled(!Taboo() && (synObj->IsFuncInvocation() || synObj->primaryToken == run_T));

  action->setChecked(showArrow);
}

void CExecView::OnUpdateEvaluate(QAction* action)
{
  action->setEnabled(!Taboo() && (text->currentSynObj->StatementSelected(text->currentSelection)
    || (text->currentSynObj->ExpressionSelected(text->currentSelection) /*&& text->currentSynObj->BoolAdmissibleOnly(text->ckd)*/)));

}

void CExecView::OnUpdateCopy(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection)
    && text->currentSynObj->ReadOnlyContext() != roClause);
}

void CExecView::OnUpdateToggleSubstitutable(QAction* action)
{
  action->setEnabled(ToggleSubstitutableEnabled());
  action->setChecked(text->currentSynObj->flags.Contains(isSubstitutable));
}

void CExecView::OnUpdateToggleClosed(QAction* action)
{
  action->setEnabled(text->currentSynObj->primaryToken == VarName_T
    && text->currentSynObj->parentObject->parentObject->primaryToken == declare_T);
  action->setChecked(text->currentSynObj->flags.Contains(isClosed));
}

void CExecView::OnUpdateToggleCategory(QAction* action)
{
  if (GetDocument()->changeNothing) {
    action->setEnabled(false);
    return;
  }

  action->setEnabled(ToggleCatEnabled());
  //action->setChecked(text->currentSynObj->flags.Contains(isStateObjectX));
}

void CExecView::OnUpdateConflict(QAction* action)
{
  action->setEnabled(ConflictSelected());
}

void CExecView::OnUpdateNextComment(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(text->showComments);
}

void CExecView::OnUpdatePrevComment(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(text->showComments);
}

void CExecView::OnUpdateNextError(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(true);
}

void CExecView::OnUpdatePrevError(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(true);
}

void CExecView::OnUpdateAssert(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateIgnoreButton(QToolButton *pb)
{
  CHAINX *chain;
  CHE *chp;
  bool hasMandInput=false;

  chp = GetFirstInput(myDECL->ParentDECL);
  if (!chp) {
    pb->setEnabled(false);
    return;
  }

  for (; chp; chp = (CHE*)chp->successor) {
    if (((LavaDECL*)chp->data)->DeclType == IAttr
    && !((LavaDECL*)chp->data)->TypeFlags.Contains(isOptional)) {
      hasMandInput = true;
      break;
    }
  };

  if (!hasMandInput || Taboo()) {
    pb->setEnabled(false);
    return;
  }

  if (!text->currentSynObj->parentObject->parentObject) {
    if (text->currentSynObj->primaryToken == Stm_T) {
      pb->setEnabled(true);
      return;
    }
    else if (text->currentSynObj->primaryToken == ignore_T) {
      pb->setEnabled(false);
      return;
    }
    else if (insertBefore) {
      pb->setEnabled(true);
      return;
    }
    else {
      pb->setEnabled(false);
      return;
    }
  }

  if (text->currentSynObj->parentObject->parentObject->parentObject) {
    pb->setEnabled(false);
    return;
  }
  if (text->currentSynObj->parentObject->primaryToken != Semicolon_T) {
    pb->setEnabled(false);
    return;
  }

  chain = text->currentSynObj->containingChain;
  chp = (CHE*)text->currentSynObj->whereInParent;
  if (chp->predecessor) {
    pb->setEnabled(false);
    return;
  }

  if (text->currentSynObj->primaryToken == Stm_T) {
    pb->setEnabled(true);
    return;
  }
  else if (text->currentSynObj->primaryToken == ignore_T) {
    pb->setEnabled(false);
    return;
  }
  else if (insertBefore) {
    pb->setEnabled(true);
    return;
  }
  else {
    pb->setEnabled(false);
    return;
  }
}

void CExecView::OnUpdateCreate(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateOld(QToolButton *pb)
{
  pb->setEnabled(!Taboo()
    && !text->currentSynObj->InOldExpression()
    && text->currentSynObj->ExpressionSelected(text->currentSelection)
    && (myDECL->DeclType == Ensure
        || (myDECL->DeclType == ExecDef
            && myDECL->ParentDECL->DeclType == Function)));
}

void CExecView::OnUpdateCall(QToolButton *pb)
{
  pb->setEnabled(!Taboo()
    && text->currentSynObj->StatementSelected(text->currentSelection)
    && text->currentSynObj->ReadOnlyContext() != roClause);
}
/*
void CExecView::OnUpdateUuid(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}
*/
void CExecView::OnUpdateStaticCall(QAction* action)
{
  action->setEnabled(!Taboo() && (text->currentSynObj->StatementSelected(text->currentSelection)
    || (text->currentSynObj->ExpressionSelected(text->currentSelection)
        //&& text->currentSynObj->parentObject->primaryToken != assign_T
        )));
}

void CExecView::OnActivateView(bool bActivate, wxView *deactiveView)
{
  // TODO: Speziellen Code hier einfgen und/oder Basisklasse aufrufen
  QString empty;

  if (GetDocument()->mySynDef && !destroying && bActivate) {
    active = true;
    if (Base)
      SetHelpText();
    redCtl->setFocus();
    //if (initialUpdateDone)
    //  wxTheApp->updateButtonsMenus();
  }
  else if (!bActivate) {
    active = false;
    DisableActions();
  }
}

void CExecView::OnUpdateInterval(QAction* action)
{
  if (Taboo()) {
    action->setEnabled(false);
    return;
  }
  action->setEnabled(text->currentSynObj->ExpressionSelected(text->currentSelection)
    || (text->currentSynObj->primaryToken == ObjPH_T
        && !text->currentSynObj->parentObject->IsFuncInvocation())
    || text->currentSynObj->primaryToken == SetPH_T);
}

void CExecView::OnUpdateExists(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateTypeSwitch(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateConnect(QToolButton *pb)
{
  pb->setEnabled(!Taboo()
    && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateDisconnect(QToolButton *pb)
{
  pb->setEnabled(!Taboo()
    && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateEmitSignal(QToolButton *pb)
{
  pb->setEnabled(!Taboo()
    && text->currentSynObj->StatementSelected(text->currentSelection)
    && selfVar->primaryToken != initiator_T);
}

void CExecView::OnUpdateAssign(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection)
    && text->currentSynObj->ReadOnlyContext() != roClause);
}

void CExecView::OnUpdateAttach(QToolButton *pb)
{
  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateQueryItf(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateIgnore(QAction* action)
{
  // TODO: Add your command update UI handler code here
  SynObject *synObj=text->currentSynObj;
  CHAINX *chx=synObj->containingChain;

  if (GetDocument()->changeNothing) {
    action->setEnabled(false);
    return;
  }

  if ((Ignorable() && !inIgnore)
  || synObj->IsPlaceHolder()
  || !synObj->IsStatement()) {
    action->setEnabled(false);
    return;
  }

  action->setEnabled(true);

  bool ig = synObj->flags.Contains(ignoreSynObj);
  action->setChecked(ig);
}

void CExecView::OnUpdateFail(QToolButton *pb)
{
        // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo()
    && text->currentSynObj->StatementSelected(text->currentSelection)
    /*&& !text->currentSynObj->InReadOnlyContext()*/);
}

void CExecView::OnUpdateQua(QToolButton *pb)
{
        // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateItem(QToolButton *pb)
{
        // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateNewLine(QAction* action)
{
  // TODO: Add your command update UI handler code here
  CHETokenNode *startToken, *predToken;
  SynObject *parent;
  bool inFormParm=false;

  if (GetDocument()->changeNothing) {
    action->setEnabled(false);
    return;
  }

  if (text->currentSynObj->flags.Contains(inExecHdr)) {
    action->setEnabled(false);
    return;
  }

  for (parent = text->currentSynObj; parent; parent = parent->parentObject) {
    if (parent->primaryToken == FormParms_T) {
      inFormParm = true;
      break;
    }
  }

  if (inFormParm) {
    action->setEnabled(false);
    return;
  }

  startToken = text->currentSynObj->startToken;
  predToken = (CHETokenNode*)startToken->predecessor;
  if (!predToken) {
    action->setEnabled(false);
    return;
  }

  bool nl = text->currentSynObj->flags.Contains(newLine);
  if (nl)
    action->setEnabled(true);
  else
    if (predToken->data.rect.top() < startToken->data.rect.top()) {
      action->setEnabled(false);
      return;
    }
    else
      action->setEnabled(true);

  action->setChecked(nl);
}

void CExecView::OnUpdateToggleArrows(QAction* action)
{
  // TODO: Add your command update UI handler code here

  if (GetDocument()->changeNothing) {
    action->setEnabled(false);
    return;
  }

  action->setEnabled(true);
  action->setChecked(!myDECL->TreeFlags.Contains(leftArrows));
}

void CExecView::OnUpdateToggleParmNames(QAction* action)
{
  // TODO: Add your command update UI handler code here

  if (GetDocument()->changeNothing) {
    action->setEnabled(false);
    return;
  }

  action->setEnabled(true);
  action->setChecked(myDECL->TreeFlags.Contains(parmNames));
}

bool CExecView::EnableCopy()
{
  if ((Ignorable() && !inIgnore)
  || !text->currentSynObj->parentObject)
    return false;

  if (text->currentSynObj->primaryToken == parameter_T)
    return false;

  if (text->currentSelection->data.token == initialize_T)
    return false;

  return ((IsDeletablePrimary()
    && text->currentSynObj->primaryToken != quant_T
    && text->currentSynObj->primaryToken != VarName_T
    && text->currentSynObj->primaryToken != VarPH_T
    && (text->currentSynObj->primaryToken != TDOD_T
        || ((CHE*)text->currentSynObj->whereInParent)->predecessor == 0)
    && !( text->currentSynObj->parentObject->IsSelfVar()
          && text->currentSynObj->whereInParent == (address)&((SelfVar*)text->currentSynObj->parentObject)->execName.ptr))
    || (text->currentSynObj->containingChain
        && text->currentSynObj->primaryToken != VarName_T
        && text->currentSynObj->primaryToken != VarPH_T
        && text->currentSynObj->primaryToken != TDOD_T)
    || (text->currentSynObj->parentObject
        && text->currentSynObj->parentObject->IsFuncInvocation()));
}

void CExecView::OnUpdateEditCopy(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(EnableCopy());
}

bool CExecView::EnablePaste()
{
  if (GetDocument()->changeNothing)
    return false;

  if ((Ignorable() && !inIgnore)
  || !clipBoardObject
  || clipBoardDoc != myDoc)
    return false;

  if (clipBoardObject->primaryToken == ObjRef_T) {
    if (!text->currentSynObj->IsPlaceHolder()
    || (text->currentSynObj->type != Exp_T
        && text->currentSynObj->type != ObjPH_T))
      return false;
  }
  else if (text->currentSynObj->flags.Contains(isIniCallOrHandle))
    return false;
  else if (text->currentSelection->data.token == initialize_T)
    return false;
  else if (text->currentSynObj->type != clipBoardObject->type)
    return false;

  return true;
}

void CExecView::OnUpdateTry(QToolButton *pb)
{
        // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo()
    && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateSucceed(QToolButton *pb)
{
        // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo()
    && text->currentSynObj->StatementSelected(text->currentSelection)
    /*&& !text->currentSynObj->InReadOnlyContext()*/);
}

void CExecView::OnUpdateEditPaste(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(EnablePaste());
}

bool CExecView::EnableUndo ()
{
  return (GetDocument()->UndoMem.EnableUndo());
}

void CExecView::OnUpdateEditUndo (QAction* action)
{

  // TODO: Add your command handler code here

  if (editCtlVisible)
    action->setEnabled(false);
  else
    GetDocument()->UndoMem.OnUpdateEditUndo(action);
}

void CExecView::OnUpdateEditRedo (QAction* action)
{

  // TODO: Add your command handler code here

  if (editCtlVisible)
    action->setEnabled(false);
  else
    GetDocument()->UndoMem.OnUpdateEditRedo(action);
}

void CExecView::OnUpdateEq(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateDeclare(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection)
    && text->currentSynObj->ReadOnlyContext() != roClause);
}

void CExecView::OnUpdateFalse(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo()
    && text->currentSynObj->ExpressionSelected(text->currentSelection)
    && (text->currentSynObj->BoolAdmissibleOnly(text->ckd)
        || text->currentSynObj->parentObject->primaryToken == elsif_T
        || text->currentSynObj->parentObject->primaryToken == elseX_T
        || text->currentSynObj->parentObject->primaryToken == ifx_T));
}

void CExecView::OnUpdateForeach(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateGe(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateGt(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateIf(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateIfdef(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateIfExpr(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here
  SynObject *synObj=text->currentSynObj;
    //text->currentSynObj->primaryToken == TDOD_T?text->currentSynObj->parentObject:text->currentSynObj;

  pb->setEnabled(!Taboo()
    && synObj->ExpressionSelected(text->currentSelection)
    && !synObj->parentObject->IsUnaryOp()
    && (!synObj->parentObject->IsBinaryOp()
        || synObj->whereInParent != (address)&((BinaryOp*)synObj->parentObject)->operand1.ptr)
    && (!synObj->parentObject->IsMultOp()
        || synObj->whereInParent != (address)((MultipleOp*)synObj->parentObject)->operands.first)
    && (!synObj->parentObject->IsFuncInvocation()
        || synObj->whereInParent != (address)&((FuncExpression*)synObj->parentObject)->handle.ptr));
}

void CExecView::OnUpdateElseExpr(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here
  SynObject *synObj=text->currentSynObj;
    //text->currentSynObj->primaryToken == TDOD_T?text->currentSynObj->parentObject:text->currentSynObj;

  pb->setEnabled(!Taboo()
    && synObj->ExpressionSelected(text->currentSelection)
    && !synObj->parentObject->IsUnaryOp()
    && (!synObj->parentObject->IsBinaryOp()
        || synObj->whereInParent != (address)&((BinaryOp*)synObj->parentObject)->operand1.ptr)
    && (!synObj->parentObject->IsMultOp()
        || synObj->whereInParent != (address)((MultipleOp*)synObj->parentObject)->operands.first)
    && (!synObj->parentObject->IsFuncInvocation()
        || synObj->whereInParent != (address)&((FuncExpression*)synObj->parentObject)->handle.ptr));
}

void CExecView::OnUpdateIn(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

bool CExecView::EnableInsert()
{
  if (Ignorable() || !text->currentSynObj->parentObject || GetDocument()->changeNothing)
    return false;

  return (
    ( (text->currentSynObj->ExpressionSelected(text->currentSelection)
       || (text->currentSynObj->primaryToken == TDOD_T
           && text->currentSynObj->parentObject->parentObject->primaryToken == ifdef_T))
      && (( text->currentSynObj->primaryToken == TDOD_T
             && text->currentSynObj->parentObject->containingChain)
          || (text->currentSynObj->primaryToken != TDOD_T
             && text->currentSynObj->containingChain)))
    || (text->currentSynObj->StatementSelected(text->currentSelection)
        && (text->currentSynObj->containingChain
            || text->currentSynObj->primaryToken == Stm_T))
    || (  (text->currentSelection->data.token == FuncPH_T
          || text->currentSelection->data.token == FuncDisabled_T
          || text->currentSelection->data.token == FuncRef_T)
        && text->currentSynObj->primaryToken == text->currentSelection->data.token
        && text->currentSynObj->parentObject->parentObject
        && text->currentSynObj->parentObject->parentObject->IsMultOp())
    || (  (text->currentSelection->data.token == FuncPH_T
          || text->currentSelection->data.token == FuncDisabled_T
          || text->currentSelection->data.token == FuncRef_T)
        && text->currentSynObj->primaryToken != text->currentSelection->data.token
        && text->currentSynObj->parentObject
        && text->currentSynObj->parentObject->IsMultOp())
    || (  (text->currentSelection->data.token == TypePH_T
          || text->currentSelection->data.token == TypeRef_T)
        && text->currentSynObj->parentObject->primaryToken == qua_T)
    || (text->currentSelection->data.token == then_T
        || text->currentSelection->data.token == elsif_T
        || text->currentSelection->data.token == from_T
        || (text->currentSelection->data.token == in_T
            && text->currentSynObj->primaryToken == quant_T)
        || text->currentSelection->data.token == case_T
        || text->currentSelection->data.token == caseType_T)
    || (text->currentSynObj->primaryToken == quant_T)
    || ((text->currentSelection->data.token == TypePH_T
        || text->currentSelection->data.token == TypePHopt_T
        || text->currentSelection->data.token == TypeRef_T
        || text->currentSelection->data.token == CrtblPH_T
        || text->currentSelection->data.token == Callee_T
        || text->currentSelection->data.token == CompObj_T
        || text->currentSelection->data.token == CrtblRef_T
        || text->currentSelection->data.token == SetPH_T
        || text->currentSelection->data.token == ObjRef_T
        || text->currentSelection->data.token == Tilde_T
        || text->currentSelection->data.token == Dollar_T)
        && text->currentSynObj->parentObject->primaryToken == quant_T)
    || ((text->currentSelection->data.token == Tilde_T
        || text->currentSelection->data.token == Dollar_T
        || text->currentSelection->data.token == Equal_T)
        && text->currentSynObj->primaryToken == TypeRef_T)
    || ((text->currentSelection->data.token == VarPH_T
        || text->currentSelection->data.token == VarName_T)
        && text->currentSynObj->parentObject->primaryToken == quant_T)
  );
}

void CExecView::OnUpdateInsert(QAction* action)
{
  // TODO: Add your command update UI handler code here

  if (Taboo() || Ignorable() || !text->currentSynObj->parentObject) {
    action->setEnabled(false);
    return;
  }
  action->setEnabled(
    ( (text->currentSynObj->ExpressionSelected(text->currentSelection)
       || (text->currentSynObj->primaryToken == TDOD_T
           && text->currentSynObj->parentObject->parentObject->primaryToken == ifdef_T))
      && (( text->currentSynObj->primaryToken == TDOD_T
             && text->currentSynObj->parentObject->containingChain)
          || (text->currentSynObj->primaryToken != TDOD_T
             && text->currentSynObj->containingChain)))
    || (text->currentSynObj->StatementSelected(text->currentSelection)
        /*&& (text->currentSynObj->containingChain
            || text->currentSynObj->primaryToken == Stm_T)*/)
    || (  (text->currentSelection->data.token == FuncPH_T
          || text->currentSelection->data.token == FuncDisabled_T
          || text->currentSelection->data.token == FuncRef_T)
        && text->currentSynObj->parentObject->parentObject
        && text->currentSynObj->parentObject->parentObject->IsMultOp())
    || (  (text->currentSelection->data.token == TypePH_T
          || text->currentSelection->data.token == TypeRef_T)
        && text->currentSynObj->parentObject->primaryToken == qua_T)
    || (text->currentSelection->data.token == then_T
        || text->currentSelection->data.token == elsif_T
        || text->currentSelection->data.token == from_T
        || (text->currentSelection->data.token == in_T
            && text->currentSynObj->primaryToken == quant_T)
        || text->currentSelection->data.token == catch_T
        || text->currentSelection->data.token == case_T
        || text->currentSelection->data.token == caseType_T)
    || (text->currentSynObj->primaryToken == quant_T)
    || ((text->currentSelection->data.token == TypePH_T
        || text->currentSelection->data.token == TypePHopt_T
        || text->currentSelection->data.token == TypeRef_T
        || text->currentSelection->data.token == CrtblPH_T
        || text->currentSelection->data.token == Callee_T
        || text->currentSelection->data.token == CompObj_T
        || text->currentSelection->data.token == CrtblRef_T
        || text->currentSelection->data.token == SetPH_T
        || text->currentSelection->data.token == ObjRef_T
        || text->currentSelection->data.token == Tilde_T
        || text->currentSelection->data.token == Dollar_T)
        && text->currentSynObj->parentObject->primaryToken == quant_T)
    || ((text->currentSelection->data.token == Tilde_T
        || text->currentSelection->data.token == Dollar_T
        || text->currentSelection->data.token == Equal_T)
        && text->currentSynObj->primaryToken == TypeRef_T)
    || ((text->currentSelection->data.token == VarPH_T
        || text->currentSelection->data.token == VarName_T)
        && text->currentSynObj->parentObject->primaryToken == quant_T)
  );
}

void CExecView::OnUpdateInsertBefore(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(true);
  action->setChecked(insertBefore);
}

void CExecView::OnUpdateInvert(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateLe(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateLshift(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateLt(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateMult(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateNe(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateShowComments(QAction* action)
{
  // TODO: Add your command update UI handler code here

  if (GetDocument()->changeNothing) {
    action->setEnabled(false);
    return;
  }

  action->setEnabled(true);
  action->setChecked(text->showComments);
}

void CExecView::OnUpdateNot(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateNull(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo()
    && ((text->currentSynObj->ExpressionSelected(text->currentSelection)
         && text->currentSynObj->NullAdmissible(text->ckd))
        || ((text->currentSynObj->primaryToken == FuncPH_T
            || text->currentSynObj->primaryToken == FuncRef_T)
           && text->currentSynObj->parentObject->primaryToken == disconnect_T)));
}

void CExecView::OnUpdateOr(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdatePlus(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdatePlusMinus(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateOrd(QAction* action)
{
        // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo()
    && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateRshift(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo() && text->currentSynObj->ExpressionSelected(text->currentSelection));
}

void CExecView::OnUpdateShowOptionals(QAction* action)
{
  // TODO: Add your command update UI handler code here

  SynObject *lookAt = text->currentSynObj;
  Quantifier *quantPtr;

  if (GetDocument()->changeNothing) {
    action->setEnabled(false);
    return;
  }

  if (Ignorable()) {
    action->setEnabled(false);
    return;
  }

  if (text->currentSynObj->parentObject
  && text->currentSynObj->parentObject->primaryToken == quant_T
  && text->currentSynObj->parentObject->parentObject->primaryToken != declare_T
  && (text->currentSelection->data.token == TypePHopt_T
      || text->currentSelection->data.token == TypeRef_T
      || text->currentSelection->data.token == SetPH_T
      || text->currentSelection->data.token == ObjRef_T)) {
    lookAt = text->currentSynObj->parentObject;
  }
  else if (text->currentSynObj->parentObject
  && text->currentSynObj->parentObject->primaryToken == quant_T
  && (text->currentSelection->data.token == VarPH_T
      || text->currentSelection->data.token == VarName_T)) {
    lookAt = text->currentSynObj;
    quantPtr = (Quantifier*)lookAt->parentObject;
  }
  else if (text->currentSynObj->parentObject
  && text->currentSynObj->parentObject->primaryToken == assignFS_T) {
    if (text->currentSelection->data.token == FuncRef_T
    || text->currentSelection->data.token == FuncPH_T) {
      lookAt = text->currentSynObj->parentObject;
    }
  }

  action->setChecked(false);
  if (lookAt->HasOptionalParts())
    action->setEnabled(true);
  else {
    action->setEnabled(false);
    return;
  }
}

void CExecView::OnUpdateSwitch(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}

void CExecView::OnUpdateTrue(QAction* action)
{
  // TODO: Add your command update UI handler code here

  action->setEnabled(!Taboo()
    && text->currentSynObj->ExpressionSelected(text->currentSelection)
    && (text->currentSynObj->BoolAdmissibleOnly(text->ckd)
        || text->currentSynObj->parentObject->primaryToken == elsif_T
        || text->currentSynObj->parentObject->primaryToken == elseX_T
        || text->currentSynObj->parentObject->primaryToken == ifx_T));
}

void CExecView::OnUpdateXor(QToolButton *pb)
{
  // TODO: Add your command update UI handler code here

  pb->setEnabled(!Taboo() && text->currentSynObj->StatementSelected(text->currentSelection));
}


void CExecView::UpdateErrMsg (QString &helpMsg) {
  CHAINX emptyChain;
  QString errorMsg;
  SynObject *errorObj=text->currentSynObj;

  if (!active || !text->currentSelection/* || errMsgUpdated*/) return;

  if (text->currentSynObj->primaryToken == ifx_T)
    errorObj = (IfxThen*)((CHE*)((IfExpression*)errorObj)->ifThens.first)->data;
  if (errorObj->lastError
  && (text->currentSelection == text->currentSynObj->primaryTokenNode
      || text->currentSelection->data.token == Rbracket_T)) {
    myDoc->SetPEError(errorObj->errorChain,true/*nextError*/);
    errorMsg = *((CLavaError*)((CHE*)errorObj->errorChain.first)->data)->IDS;
    errorMsg = QString(((CLavaError*)((CHE*)errorObj->errorChain.first)->data)->textParam.c) + errorMsg;
    statusBar->showMessage(errorMsg);
    nextError = false;
    return;
  }
  else if (nextError)
    myDoc->SetPEError(plhChain,true);
  else {
    myDoc->SetPEError(emptyChain,false);
    nextError = false;
        }

  if (text->currentSynObj->comment.ptr)
    statusBar->showMessage(text->currentSynObj->comment.ptr->str.c);
  else
    statusBar->showMessage(helpMsg);
  errMsgUpdated = true;
}

ObjComboUpdate::ObjComboUpdate (CExecView *ev) {
  bool criticalScope=false;

  execView = ev;
  doc = ev->myDoc;
  ev->m_ComboBar->RemoveLocals();
}

bool ObjComboUpdate::Action (CheckData &ckd, VarName *varName, TID &tid) {
  TID varID;

  if (ckd.criticalScope)
    ckd.criticalScope = false;
  else if (tid.nID != -1 && (!ckd.handleOpd || ckd.inQuant)) {
    varID = varName->varID;
    ADJUST4(varID);
    ADJUST4(tid);
    execView->m_ComboBar->AddLocal(varID,varName->varName,tid);
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////
//What next? help:


void CExecView::on_whatNextAction_triggered()
{
  redCtl->setWhatsThis(text->currentSynObj->whatNextText());
  QCoreApplication::sendEvent(redCtl,
    new WhatNextEvent(UEV_WhatNext,
    text->currentSynObj->startToken->data.rect.topLeft(),
    redCtl->mapToGlobal(text->currentSynObj->startToken->data.rect.topLeft())));
}


void CExecView::on_DbgBreakpointAct_triggered() {
  CHEProgPoint* cheBreak;
  SynObject *stopObj=text->currentSynObj;

  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  for (cheBreak = (CHEProgPoint*)LBaseData->ContData->BrkPnts.first;
       cheBreak && (cheBreak->data.SynObjID != stopObj->synObjectID);
       cheBreak = (CHEProgPoint*)cheBreak->successor);
  if (cheBreak) {
    if (stopObj->workFlags.Contains(isBrkPnt))
      stopObj->workFlags.EXCL(isBrkPnt);
    else
      stopObj->workFlags.INCL(isBrkPnt);
    cheBreak->data.Activate = stopObj->workFlags.Contains(isBrkPnt);
  }
  else {
    cheBreak = new CHEProgPoint;;
//    cheBreak->data.SynObj = stopObj;
    cheBreak->data.SynObjID = stopObj->synObjectID;
    cheBreak->data.ExecType = myDECL->DeclType;
    cheBreak->data.FuncDoc = (address)GetDocument();
    cheBreak->data.FuncDocName = ((CHESimpleSyntax*)GetDocument()->IDTable.mySynDef->SynDefTree.first)->data.SyntaxName;
    cheBreak->data.FuncDocDir = GetDocument()->IDTable.DocDir;
    cheBreak->data.FuncID.nID = myID.nID;
    if (stopObj->workFlags.Contains(isBrkPnt))
      stopObj->workFlags.EXCL(isBrkPnt);
    else
      stopObj->workFlags.INCL(isBrkPnt);
    cheBreak->data.Activate = stopObj->workFlags.Contains(isBrkPnt);
    LBaseData->ContData->BrkPnts.Append(cheBreak);
  }
  redCtl->update();
}

void CExecView::on_DbgRunToSelAct_triggered() {
  if (!LBaseData->ContData)
    LBaseData->ContData = new DbgContData;
  LBaseData->ContData->ContType = dbg_RunTo;
  LBaseData->ContData->RunToPnt.ptr = new ProgPoint;
  ProgPoint * pp = LBaseData->ContData->RunToPnt.ptr;
  pp->SynObj = text->currentSynObj;
  pp->SynObjID = text->currentSynObj->synObjectID;
  pp->ExecType = myDECL->DeclType;
  pp->FuncDocName = ((CHESimpleSyntax*)myDoc->IDTable.mySynDef->SynDefTree.first)->data.SyntaxName;
  pp->FuncDocDir = myDoc->IDTable.DocDir;
  pp->FuncDoc = (address)myDoc;
  pp->FuncID.nID = myID.nID;
  pp->FuncID.nINCL = LBaseData->debugger->myDoc->IDTable.GetINCL(pp->FuncDocName,pp->FuncDocDir);
  if (pp->FuncID.nINCL < 0) {
    LBaseData->ContData->RunToPnt.Destroy();
    QMessageBox::critical(this,qApp->applicationName(),"This run-to-selection is not part of the debugged lava program",QMessageBox::Ok|QMessageBox::Default,Qt::NoButton);
    return;
  }
  DbgMessage* mess = new DbgMessage(Dbg_Continue);
  QApplication::postEvent(wxTheApp,new CustomEvent(UEV_LavaDebugRq,(void*)mess));
}


void CExecView::OnUpdateDbgStart(QAction* action) {
  if (!myDoc->mySynDef)
    return;
  action->setEnabled(LBaseData->debugger->sendPending);
}

void CExecView::OnUpdateDbgStepNext(QAction* action) {
  action->setEnabled(LBaseData->debugger->isConnected && LBaseData->debugger->sendPending);
}

void CExecView::OnUpdateDbgStepNextFunction(QAction* action) {
  action->setEnabled(LBaseData->debugger->isConnected && LBaseData->debugger->sendPending);
}

void CExecView::OnUpdateDbgStepinto(QAction* action) {
  action->setEnabled(LBaseData->debugger->isConnected && LBaseData->debugger->sendPending);
}

void CExecView::OnUpdateDbgStepout(QAction* action) {
  action->setEnabled(LBaseData->debugger->isConnected && LBaseData->debugger->sendPending);
}

void CExecView::OnUpdateDbgRunToSel(QAction* action)
{
  action->setEnabled(!Taboo(true)
    && text->currentSynObj->IsExecutable()
    && LBaseData->debugger->isConnected && LBaseData->debugger->sendPending);
}

void CExecView::OnUpdateDbgBreakpoint(QAction* action)
{
  action->setEnabled(!Taboo(true)
    && text->currentSynObj->IsExecutable()
    && !text->currentSynObj->IsPlaceHolder()
    && !text->currentSynObj->IsConstant()
    && !(text->currentSynObj->primaryToken == declare_T)
    && !(text->currentSynObj->primaryToken == Semicolon_T));
}

void CExecView::OnUpdateDbgClearBreakpoints(QAction* action) {
  action->setEnabled(true);
}

void CExecView::OnUpdateDbgStop(QAction* action) {
  action->setEnabled(LBaseData->debugger->isConnected);
}

void CComment::on_okButton_clicked()
{
  accept();
}


void CComment::on_cancelButton_clicked()
{
  reject();
}
