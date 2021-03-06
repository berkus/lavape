
/* Lava programming environment and Lava interpreter.
   Copyright (C) 2001 Fraunhofer-Gesellschaft.
   The GNU GPL license applies. For details see the file
   "license.txt" in the top level directory. */

// ScrollPane.cpp : implementation file
//

#include "LavaGUIPopup.h"
#include "GUIProgBase.h"
#include "qpushbutton.h"
//#include "q3vbox.h"
#include "qlayout.h"
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>

#pragma hdrstop


#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;


/////////////////////////////////////////////////////////////////////////////
// CLavaGUIPopup


CLavaGUIPopup::CLavaGUIPopup(QWidget* parent, CGUIProgBase *guiPr, CHEFormNode* data)
    : QMainWindow(parent)
{
  setObjectName("LavaGUIPopup");
  myFormNode = data;
  GUIProg = guiPr;
  QPalette palette;
  palette.setColor(QPalette::Active,QPalette::Window, GUIProg->ViewWin->palette().window().color());
  setPalette(palette);
//  setPaletteBackgroundColor(GUIProg->ViewWin->paletteBackgroundColor());
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


CLavaGUIPopup::~CLavaGUIPopup()
{
  if (wxTheApp->appExit)
    return;
  CHEFormNode* subNode;
  if (hasFocNode(myFormNode)) 
    GUIProg->setFocNode(0);
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


CLavaGUIPopupD::CLavaGUIPopupD(QWidget* parent, CGUIProgBase *guiPr, CHEFormNode* data)
: QDialog(parent)
{
  setModal(true);
  setObjectName("LavaGUIPopupD");
  myFormNode = data;
  GUIProg = guiPr;
  QPalette palette;
  palette.setColor(QPalette::Active,QPalette::Window, GUIProg->ViewWin->palette().window().color());
  setPalette(palette);
//  setPaletteBackgroundColor(GUIProg->ViewWin->palette().window().color());
  view = new GUIScrollView(this, true);
  QWidget* hb = new QWidget(this);
  QPushButton* okButton = new QPushButton("Close", hb);
  //QPushButton* cancelButton = new QPushButton("Cancel", hb);
  QVBoxLayout* qvbl = new QVBoxLayout(this);
  QHBoxLayout* hbl = new QHBoxLayout(hb);
  qvbl->addWidget(view);
  qvbl->addWidget(hb);
  hbl->addWidget(okButton);
//  hbl->addWidget(cancelButton);
  connect(okButton,SIGNAL(clicked()),SLOT(on_cancelButton_clicked()));
//  connect(cancelButton,SIGNAL(clicked()),SLOT(OnCancel()));
  hide();
}

void CLavaGUIPopupD::on_cancelButton_clicked()
{
  myFormNode->data.FormSyntax->WorkFlags.EXCL(poppedUp);
  hide();
  QDialog::accept();

}

bool CLavaGUIPopupD::hasFocNode(CHEFormNode *actFNode)
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


CLavaGUIPopupD::~CLavaGUIPopupD()
{
  if (wxTheApp->appExit)
    return;
  CHEFormNode* subNode;
  if (hasFocNode(myFormNode)) 
    GUIProg->setFocNode(0);
  for (subNode = (CHEFormNode*)myFormNode->data.SubTree.first;
      subNode;
      subNode = (CHEFormNode*)subNode->successor)
    ((CGUIProg*)GUIProg)->LavaForm.DeletePopups(subNode);
  myFormNode->data.FIP.popupShell = 0;
  myFormNode->data.FIP.poppedUp = false;
}

void CLavaGUIPopupD::closeEvent(QCloseEvent *e)
{
  myFormNode->data.FormSyntax->WorkFlags.EXCL(poppedUp);
  QWidget::closeEvent(e);
}
