
/* Lava programming environment and Lava interpreter.
   Copyright (C) 2001 Fraunhofer-Gesellschaft.
   The GNU GPL license applies. For details see the file
   "license.txt" in the top level directory. */

// ScrollPane.cpp : implementation file
//

#include "LavaGUIPopup.h"
#include "GUIProgBase.h"
//#include "stdafx.h"

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;



#ifdef __GNUC__
#pragma implementation
#endif


/////////////////////////////////////////////////////////////////////////////
// CLavaGUIPopup


CLavaGUIPopup::CLavaGUIPopup(QWidget* parent, CGUIProgBase *guiPr, CHEFormNode* data)
    : QMainWindow(parent,"LavaGUIPopup",WDestructiveClose)// | WType_TopLevel)
{
  myFormNode = data;
  GUIProg = guiPr;
  setPaletteBackgroundColor(GUIProg->ViewWin->paletteBackgroundColor());
  view = new GUIScrollView(this, true);
  setCentralWidget(view);
  hide();
}

bool CLavaGUIPopup::hasFocNode(CHEFormNode *actFNode)
{
  CHEFormNode *subNode;
  if (!GUIProg->focNode)
    return false;
  for (subNode = (CHEFormNode*)actFNode->data.SubTree.first;
      subNode;
      subNode = (CHEFormNode*)subNode->successor) {
    if (!GUIProg->focNode)
      return false;
    if (subNode == GUIProg->focNode)
      return true;
    if (hasFocNode(subNode))
      return true;
  }
  return false;
}

void CLavaGUIPopup::moveEvent(QMoveEvent* ev)
{
  //myFormNode->data.FormSyntax->SectionInfo1 = ev->pos().y();
  //myFormNode->data.FormSyntax->SectionInfo2 = ev->pos().x();
}

CLavaGUIPopup::~CLavaGUIPopup()
{
  if (wxTheApp->appExit)
    return;
  CHEFormNode* subNode;
  if (hasFocNode(myFormNode)) 
    GUIProg->focNode = 0;
  for (subNode = (CHEFormNode*)myFormNode->data.SubTree.first;
      subNode;
      subNode = (CHEFormNode*)subNode->successor)
    ((CGUIProg*)GUIProg)->LavaForm.DeletePopups(subNode);
  myFormNode->data.FIP.popupShell = 0;
  myFormNode->data.FIP.poppedUp = false;
}

void CLavaGUIPopup::closeEvent(QCloseEvent *e)
{
  myFormNode->data.FormSyntax->WorkFlags.EXCL(poppedUp);
  QWidget::closeEvent(e);

}
