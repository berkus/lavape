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



#include "GUIProg.h"
#include "LavaMenu.h"
#include "GUIProgBase.h"
#include "FormWid.h"
#include "qlineedit.h"
//Added by qt3to4:
#include <QFrame>
#include <QMenu>


/////////////////////////////////////////////////////////////////////////////
// CLavaMenu



CLavaMenu::~CLavaMenu()
{
  if (myMenu)
    delete myMenu;
}

CLavaMenu::CLavaMenu(CGUIProgBase *guiPr, CHEFormNode* data,
       QWidget* pParentWnd, const char* WindowName, QString label, int cx)
       :QLineEdit(label, pParentWnd, "LavaMenu")
{
  myFormNode = data;
  isEnter = false;
  count = 0;
  GUIProg = guiPr;
  myMenu = 0;
  myFormNode->data.ownTFont = GUIProg->SetTFont(this, myFormNode);
  GUIProg->SetColor(this, myFormNode);
  //if (GUIProg->Font)
  //  setFont(*GUIProg->Font);
  QSize size = GUIProg->CalcTextRect(cx, 1, font());
  int bord = ((QFrame*)pParentWnd)->lineWidth();
  setGeometry(QRect(QPoint(bord,bord), size));
  setReadOnly(true);
  show();
  if (myFormNode->data.IoSigFlags.Contains(DONTPUT)
      || !myFormNode->data.IoSigFlags.Contains(Flag_INPUT))
    setEnabled(false);
  else {
    myMenu = new QMenu(this);
    //myMenu->CreatePopupMenu();
  }
}

void CLavaMenu::SetItemData(unsigned pos, CHEFormNode* data)
{
  while (count < pos) {
    userItemData[count] = 0;
    count++;
  }
  if (count < pos +1)
    count = pos +1;
  userItemData[pos] = data;
}

/*
BEGIN_MESSAGE_MAP(CLavaMenu, CEdit)
  //{{AFX_MSG_MAP(CLavaMenu)
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_SETFOCUS()
  ON_WM_KEYDOWN()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLavaMenu message handlers

bool CLavaMenu::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  int nCode = LOWORD(wParam);
  if (!HIWORD(wParam) && (nCode >= 0)) {
    myFormNode->data.D = nCode;
    ((CGUIProg*)GUIProg)->CmdExec.HandleButtonClick(userItemData[nCode], myFormNode );
    myFormNode->data.IoSigFlags.INCL(trueValue);
    return true;
  }
  else
    return QWidget::OnCommand(wParam, lParam);
}


void CLavaMenu::OnLButtonDown(UINT nFlags, QPoint point) 
{
  ClientToScreen(&point);
  QRect rect;
  GetWindowRect(rect);
  if (myMenu && rect.PtInRect(point))
    myMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTALIGN |TPM_LEFTBUTTON, point.x, point.y,this);
  QWidget::OnLButtonDown(nFlags, point);
}

void CLavaMenu::OnRButtonDown(UINT nFlags, QPoint point) 
{
  ClientToScreen(&point);
  QRect rect;
  GetWindowRect(rect);
  if (myMenu && rect.PtInRect(point))
    myMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTALIGN |TPM_LEFTBUTTON, point.x, point.y,this);
  QWidget::OnRButtonDown(nFlags, point);
}

void CLavaMenu::OnSetFocus(QWidget* pOldWnd) 
{
  GUIProg->editNode = 0;
  GUIProg->butNode = 0;
  GUIProg->focNode = myFormNode;
  GUIProg->CurPTR = myFormNode;
  GUIProg->ScrollIntoFrame(this);
  CEdit::OnSetFocus(pOldWnd);
}

void CLavaMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if (nChar == VK_TAB)
    ((CGUIProg*)GUIProg)->OnTab(false, this);
  CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

*/
